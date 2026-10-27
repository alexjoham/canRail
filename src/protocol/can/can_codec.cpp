#include "protocol/can/can_codec.hpp"

#include "protocol/common/byte_reader.hpp"
#include "protocol/common/byte_writer.hpp"

namespace can_rail::protocol::can {

namespace {

constexpr std::uint8_t TrainSpeedLength = 6;

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

    return Result<CanMessage>::failure(ProtocolError::UnsupportedCommand);
}

Result<CanFrame> CanCodec::encode(const CanMessage &message) {
    const auto *trainSpeed = std::get_if<TrainSpeed>(&message.payload);
    if (trainSpeed == nullptr || message.id.command != CanCommand::TrainSpeed) {
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
