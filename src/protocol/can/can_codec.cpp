#include "protocol/can/can_codec.hpp"

#include "protocol/common/byte_reader.hpp"
#include "protocol/common/byte_writer.hpp"

namespace can_rail::protocol::can {

namespace {

constexpr std::uint8_t TrainSpeedLength = 6;
constexpr std::uint8_t TrainDirectionLength = 5;
constexpr std::uint8_t TrainFunctionLength = 6;

Result<CanMessage> decodeTrainSpeed(const CanId &id, const CanFrame &frame) {
    if (frame.length != TrainSpeedLength) {
        return Result<CanMessage>::failure(ProtocolError::InvalidDataLength);
    }

    ::can_rail::protocol::ByteReader reader(frame.data.data(), frame.length);
    const auto locomotiveUid = reader.readU32BigEndian();
    const auto speed = reader.readU16BigEndian();
    if (!locomotiveUid || !speed) {
        return Result<CanMessage>::failure(ProtocolError::BufferTooShort);
    }

    return Result<CanMessage>::success(
        CanMessage{id, TrainSpeed{locomotiveUid.value(), speed.value()}});
}

Result<CanMessage> decodeTrainDirection(const CanId &id, const CanFrame &frame) {
    if (frame.length != TrainDirectionLength) {
        return Result<CanMessage>::failure(ProtocolError::InvalidDataLength);
    }

    ::can_rail::protocol::ByteReader reader(frame.data.data(), frame.length);
    const auto locomotiveUid = reader.readU32BigEndian();
    const auto direction = reader.readU8();
    if (!locomotiveUid || !direction) {
        return Result<CanMessage>::failure(ProtocolError::BufferTooShort);
    }
    if (direction.value() > static_cast<std::uint8_t>(Direction::Change)) {
        return Result<CanMessage>::failure(ProtocolError::InvalidEnumValue);
    }

    return Result<CanMessage>::success(CanMessage{
        id, TrainDirection{locomotiveUid.value(), static_cast<Direction>(direction.value())}});
}

Result<CanMessage> decodeTrainFunction(const CanId &id, const CanFrame &frame) {
    if (frame.length != TrainFunctionLength) {
        return Result<CanMessage>::failure(ProtocolError::InvalidDataLength);
    }

    ::can_rail::protocol::ByteReader reader(frame.data.data(), frame.length);
    const auto locomotiveUid = reader.readU32BigEndian();
    const auto functionNumber = reader.readU8();
    const auto value = reader.readU8();
    if (!locomotiveUid || !functionNumber || !value) {
        return Result<CanMessage>::failure(ProtocolError::BufferTooShort);
    }
    if (functionNumber.value() > 31 || value.value() > 31) {
        return Result<CanMessage>::failure(ProtocolError::InvalidEnumValue);
    }

    return Result<CanMessage>::success(CanMessage{
        id, TrainFunction{locomotiveUid.value(), functionNumber.value(), value.value()}});
}

} // namespace

Result<CanMessage> CanCodec::decode(const CanFrame &frame) {
    if (frame.length > CanFrame::MaxDataLength) {
        return Result<CanMessage>::failure(ProtocolError::InvalidDataLength);
    }

    const auto id = CanId::decode(frame.identifier);
    if (!id) {
        return Result<CanMessage>::failure(id.error());
    }

    if (id.value().command == CanCommand::TrainSpeed) {
        return decodeTrainSpeed(id.value(), frame);
    }
    if (id.value().command == CanCommand::TrainDirection) {
        return decodeTrainDirection(id.value(), frame);
    }
    if (id.value().command == CanCommand::TrainFunction) {
        return decodeTrainFunction(id.value(), frame);
    }

    return Result<CanMessage>::failure(ProtocolError::UnsupportedCommand);
}

Result<CanFrame> CanCodec::encode(const CanMessage &message) {
    const auto *trainSpeed = std::get_if<TrainSpeed>(&message.payload);
    if (trainSpeed == nullptr || message.id.command != CanCommand::TrainSpeed) {
        const auto *trainDirection = std::get_if<TrainDirection>(&message.payload);
        if (trainDirection != nullptr && message.id.command == CanCommand::TrainDirection) {
            const auto direction = static_cast<std::uint8_t>(trainDirection->direction);
            if (direction > static_cast<std::uint8_t>(Direction::Change)) {
                return Result<CanFrame>::failure(ProtocolError::InvalidEnumValue);
            }

            CanFrame frame;
            frame.identifier = message.id.encode();
            frame.length = TrainDirectionLength;
            ::can_rail::protocol::ByteWriter writer(frame.data.data(), frame.data.size());
            if (!writer.writeU32BigEndian(trainDirection->locomotiveUid) ||
                !writer.writeU8(direction)) {
                return Result<CanFrame>::failure(ProtocolError::BufferFull);
            }
            return Result<CanFrame>::success(frame);
        }

        const auto *trainFunction = std::get_if<TrainFunction>(&message.payload);
        if (trainFunction != nullptr && message.id.command == CanCommand::TrainFunction) {
            if (trainFunction->functionNumber > 31 || trainFunction->value > 31) {
                return Result<CanFrame>::failure(ProtocolError::InvalidEnumValue);
            }

            CanFrame frame;
            frame.identifier = message.id.encode();
            frame.length = TrainFunctionLength;
            ::can_rail::protocol::ByteWriter writer(frame.data.data(), frame.data.size());
            if (!writer.writeU32BigEndian(trainFunction->locomotiveUid) ||
                !writer.writeU8(trainFunction->functionNumber) ||
                !writer.writeU8(trainFunction->value)) {
                return Result<CanFrame>::failure(ProtocolError::BufferFull);
            }
            return Result<CanFrame>::success(frame);
        }

        return Result<CanFrame>::failure(ProtocolError::UnsupportedCommand);
    }

    CanFrame frame;
    frame.identifier = message.id.encode();
    frame.length = TrainSpeedLength;

    ::can_rail::protocol::ByteWriter writer(frame.data.data(), frame.data.size());
    if (!writer.writeU32BigEndian(trainSpeed->locomotiveUid) ||
        !writer.writeU16BigEndian(trainSpeed->speed)) {
        return Result<CanFrame>::failure(ProtocolError::BufferFull);
    }

    return Result<CanFrame>::success(frame);
}

} // namespace can_rail::protocol::can
