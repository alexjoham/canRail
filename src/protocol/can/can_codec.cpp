#include "protocol/can/can_codec.hpp"

#include "protocol/common/byte_reader.hpp"
#include "protocol/common/byte_writer.hpp"

namespace can_rail::protocol::can {

namespace {

constexpr std::uint8_t TrainSpeedLength = 6;
constexpr std::uint8_t TrainSpeedQueryLength = 4;
constexpr std::uint8_t TrainDirectionLength = 5;
constexpr std::uint8_t TrainDirectionQueryLength = 4;
constexpr std::uint8_t TrainFunctionLength = 6;
constexpr std::uint8_t SystemCommandLength = 5;
constexpr std::uint8_t SystemTrainDataProtocolLength = 6;
constexpr std::uint8_t SystemSwitchingTimeLength = 7;
constexpr std::uint8_t SystemTrackProtocolActivateLength = 6;
constexpr std::uint8_t SystemMfxNewRegistrationCounterLength = 7;
constexpr std::uint8_t SystemOverloadLength = 6;
constexpr std::uint8_t TrainFunctionQueryLength = 5;

Result<CanMessage> decodeTrainSpeed(const CanId &id, const CanFrame &frame) {
    if (frame.length != TrainSpeedLength && frame.length != TrainSpeedQueryLength) {
        return Result<CanMessage>::failure(ProtocolError::InvalidDataLength);
    }

    ::can_rail::protocol::ByteReader reader(frame.data.data(), frame.length);
    const auto locomotiveUid = reader.readU32BigEndian();
    if (!locomotiveUid) {
        return Result<CanMessage>::failure(ProtocolError::BufferTooShort);
    }

    if (frame.length == TrainSpeedQueryLength) {
        return Result<CanMessage>::success(CanMessage{id, TrainSpeed{locomotiveUid.value(), {}}});
    }

    const auto speed = reader.readU16BigEndian();
    if (!speed) {
        return Result<CanMessage>::failure(ProtocolError::BufferTooShort);
    }

    return Result<CanMessage>::success(
        CanMessage{id, TrainSpeed{locomotiveUid.value(), speed.value()}});
}

Result<CanMessage> decodeTrainDirection(const CanId &id, const CanFrame &frame) {
    if (frame.length != TrainDirectionLength && frame.length != TrainDirectionQueryLength) {
        return Result<CanMessage>::failure(ProtocolError::InvalidDataLength);
    }

    ::can_rail::protocol::ByteReader reader(frame.data.data(), frame.length);
    const auto locomotiveUid = reader.readU32BigEndian();
    if (!locomotiveUid) {
        return Result<CanMessage>::failure(ProtocolError::BufferTooShort);
    }

    if (frame.length == TrainDirectionQueryLength) {
        return Result<CanMessage>::success(
            CanMessage{id, TrainDirection{locomotiveUid.value(), {}}});
    }

    const auto direction = reader.readU8();
    if (!direction) {
        return Result<CanMessage>::failure(ProtocolError::BufferTooShort);
    }
    if (direction.value() > static_cast<std::uint8_t>(Direction::Change)) {
        return Result<CanMessage>::failure(ProtocolError::InvalidEnumValue);
    }

    return Result<CanMessage>::success(CanMessage{
        id, TrainDirection{locomotiveUid.value(), static_cast<Direction>(direction.value())}});
}

Result<CanMessage> decodeTrainFunction(const CanId &id, const CanFrame &frame) {
    if (frame.length != TrainFunctionLength && frame.length != TrainFunctionQueryLength) {
        return Result<CanMessage>::failure(ProtocolError::InvalidDataLength);
    }

    ::can_rail::protocol::ByteReader reader(frame.data.data(), frame.length);
    const auto locomotiveUid = reader.readU32BigEndian();
    const auto functionNumber = reader.readU8();
    if (!locomotiveUid || !functionNumber) {
        return Result<CanMessage>::failure(ProtocolError::BufferTooShort);
    }
    if (functionNumber.value() > 31) {
        return Result<CanMessage>::failure(ProtocolError::InvalidEnumValue);
    }

    if (frame.length == TrainFunctionQueryLength) {
        return Result<CanMessage>::success(
            CanMessage{id, TrainFunction{locomotiveUid.value(), functionNumber.value(), {}}});
    }

    const auto value = reader.readU8();
    if (!value) {
        return Result<CanMessage>::failure(ProtocolError::BufferTooShort);
    }
    if (value.value() > 31) {
        return Result<CanMessage>::failure(ProtocolError::InvalidEnumValue);
    }

    return Result<CanMessage>::success(CanMessage{
        id, TrainFunction{locomotiveUid.value(), functionNumber.value(), value.value()}});
}

Result<CanMessage> decodeSystemCommand(const CanId &id, const CanFrame &frame) {
    const auto subcommand = frame.length > 4 ? frame.data[4] : static_cast<std::uint8_t>(0xFF);
    const auto expectedLength = [&]() {
        switch (static_cast<SystemSubcommand>(subcommand)) {
        case SystemSubcommand::TrainDataProtocol:
        case SystemSubcommand::TrackProtocolActivate:
            return SystemTrainDataProtocolLength;
        case SystemSubcommand::SwitchingTime:
            return SystemSwitchingTimeLength;
        case SystemSubcommand::MfxNew:
            return SystemMfxNewRegistrationCounterLength;
        case SystemSubcommand::Overthrottle:
            return SystemOverloadLength;
        default:
            return SystemCommandLength;
        }
    }();
    if (frame.length != expectedLength) {
        return Result<CanMessage>::failure(ProtocolError::InvalidDataLength);
    }

    ::can_rail::protocol::ByteReader reader(frame.data.data(), frame.length);
    const auto deviceUid = reader.readU32BigEndian();
    const auto subcommandValue = reader.readU8();
    if (!deviceUid || !subcommandValue) {
        return Result<CanMessage>::failure(ProtocolError::BufferTooShort);
    }

    switch (static_cast<SystemSubcommand>(subcommandValue.value())) {
    case SystemSubcommand::Stop:
        return Result<CanMessage>::success(CanMessage{id, SystemStop{deviceUid.value()}});
    case SystemSubcommand::Go:
        return Result<CanMessage>::success(CanMessage{id, SystemGo{deviceUid.value()}});
    case SystemSubcommand::Halt:
        return Result<CanMessage>::success(CanMessage{id, SystemHalt{deviceUid.value()}});
    case SystemSubcommand::TrainEmergencyHalt:
        return Result<CanMessage>::success(
            CanMessage{id, SystemTrainEmergencyHalt{deviceUid.value()}});
    case SystemSubcommand::TrainCycleStop:
        return Result<CanMessage>::success(CanMessage{id, SystemTrainCycleStop{deviceUid.value()}});
    case SystemSubcommand::TrainDataProtocol: {
        const auto protocol = reader.readU8();
        if (!protocol) {
            return Result<CanMessage>::failure(ProtocolError::BufferTooShort);
        }
        return Result<CanMessage>::success(
            CanMessage{id, SystemTrainDataProtocol{deviceUid.value(), protocol.value()}});
    }
    case SystemSubcommand::SwitchingTime: {
        const auto time = reader.readU16BigEndian();
        if (!time) {
            return Result<CanMessage>::failure(ProtocolError::BufferTooShort);
        }
        return Result<CanMessage>::success(
            CanMessage{id, SystemSwitchingTime{deviceUid.value(), time.value()}});
    }
    case SystemSubcommand::TrackProtocolActivate: {
        const auto protocolMask = reader.readU8();
        if (!protocolMask) {
            return Result<CanMessage>::failure(ProtocolError::BufferTooShort);
        }
        return Result<CanMessage>::success(
            CanMessage{id, SystemTrackProtocolActivate{deviceUid.value(), protocolMask.value()}});
    }
    case SystemSubcommand::MfxNew: {
        const auto counter = reader.readU16BigEndian();
        if (!counter) {
            return Result<CanMessage>::failure(ProtocolError::BufferTooShort);
        }
        return Result<CanMessage>::success(
            CanMessage{id, SystemMfxNewRegistrationCounter{deviceUid.value(), counter.value()}});
    }
    case SystemSubcommand::Overthrottle: {
        const auto channel = reader.readU8();
        if (!channel) {
            return Result<CanMessage>::failure(ProtocolError::BufferTooShort);
        }
        return Result<CanMessage>::success(
            CanMessage{id, SystemOverload{deviceUid.value(), channel.value()}});
    }
    default:
        return Result<CanMessage>::failure(ProtocolError::InvalidEnumValue);
    }
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
    if (id.value().command == CanCommand::System) {
        return decodeSystemCommand(id.value(), frame);
    }

    return Result<CanMessage>::failure(ProtocolError::UnsupportedCommand);
}

Result<CanFrame> CanCodec::encode(const CanMessage &message) {
    const auto encodeSystem = [&message](std::uint32_t uid, SystemSubcommand subcommand) {
        CanFrame frame;
        frame.identifier = message.id.encode();
        frame.length = SystemCommandLength;
        ::can_rail::protocol::ByteWriter writer(frame.data.data(), frame.data.size());
        if (!writer.writeU32BigEndian(uid) ||
            !writer.writeU8(static_cast<std::uint8_t>(subcommand))) {
            return Result<CanFrame>::failure(ProtocolError::BufferFull);
        }
        return Result<CanFrame>::success(frame);
    };

    if (message.id.command == CanCommand::System) {
        if (const auto *stop = std::get_if<SystemStop>(&message.payload)) {
            return encodeSystem(stop->deviceUid, SystemSubcommand::Stop);
        }
        if (const auto *go = std::get_if<SystemGo>(&message.payload)) {
            return encodeSystem(go->deviceUid, SystemSubcommand::Go);
        }
        if (const auto *halt = std::get_if<SystemHalt>(&message.payload)) {
            return encodeSystem(halt->deviceUid, SystemSubcommand::Halt);
        }
        if (const auto *emergencyHalt = std::get_if<SystemTrainEmergencyHalt>(&message.payload)) {
            return encodeSystem(emergencyHalt->locomotiveUid, SystemSubcommand::TrainEmergencyHalt);
        }
        if (const auto *cycleStop = std::get_if<SystemTrainCycleStop>(&message.payload)) {
            return encodeSystem(cycleStop->locomotiveUid, SystemSubcommand::TrainCycleStop);
        }
        if (const auto *trainData = std::get_if<SystemTrainDataProtocol>(&message.payload)) {
            CanFrame frame;
            frame.identifier = message.id.encode();
            frame.length = SystemTrainDataProtocolLength;
            ::can_rail::protocol::ByteWriter writer(frame.data.data(), frame.data.size());
            if (!writer.writeU32BigEndian(trainData->locomotiveUid) ||
                !writer.writeU8(static_cast<std::uint8_t>(SystemSubcommand::TrainDataProtocol)) ||
                !writer.writeU8(trainData->protocol)) {
                return Result<CanFrame>::failure(ProtocolError::BufferFull);
            }
            return Result<CanFrame>::success(frame);
        }
        if (const auto *switchingTime = std::get_if<SystemSwitchingTime>(&message.payload)) {
            CanFrame frame;
            frame.identifier = message.id.encode();
            frame.length = SystemSwitchingTimeLength;
            ::can_rail::protocol::ByteWriter writer(frame.data.data(), frame.data.size());
            if (!writer.writeU32BigEndian(switchingTime->deviceUid) ||
                !writer.writeU8(static_cast<std::uint8_t>(SystemSubcommand::SwitchingTime)) ||
                !writer.writeU16BigEndian(switchingTime->time)) {
                return Result<CanFrame>::failure(ProtocolError::BufferFull);
            }
            return Result<CanFrame>::success(frame);
        }
        if (const auto *activate = std::get_if<SystemTrackProtocolActivate>(&message.payload)) {
            CanFrame frame;
            frame.identifier = message.id.encode();
            frame.length = SystemTrackProtocolActivateLength;
            ::can_rail::protocol::ByteWriter writer(frame.data.data(), frame.data.size());
            if (!writer.writeU32BigEndian(activate->deviceUid) ||
                !writer.writeU8(
                    static_cast<std::uint8_t>(SystemSubcommand::TrackProtocolActivate)) ||
                !writer.writeU8(activate->protocolMask)) {
                return Result<CanFrame>::failure(ProtocolError::BufferFull);
            }
            return Result<CanFrame>::success(frame);
        }
        if (const auto *counter = std::get_if<SystemMfxNewRegistrationCounter>(&message.payload)) {
            CanFrame frame;
            frame.identifier = message.id.encode();
            frame.length = SystemMfxNewRegistrationCounterLength;
            ::can_rail::protocol::ByteWriter writer(frame.data.data(), frame.data.size());
            if (!writer.writeU32BigEndian(counter->deviceUid) ||
                !writer.writeU8(static_cast<std::uint8_t>(SystemSubcommand::MfxNew)) ||
                !writer.writeU16BigEndian(counter->counter)) {
                return Result<CanFrame>::failure(ProtocolError::BufferFull);
            }
            return Result<CanFrame>::success(frame);
        }
        if (const auto *overload = std::get_if<SystemOverload>(&message.payload)) {
            CanFrame frame;
            frame.identifier = message.id.encode();
            frame.length = SystemOverloadLength;
            ::can_rail::protocol::ByteWriter writer(frame.data.data(), frame.data.size());
            if (!writer.writeU32BigEndian(overload->deviceUid) ||
                !writer.writeU8(static_cast<std::uint8_t>(SystemSubcommand::Overthrottle)) ||
                !writer.writeU8(overload->channel)) {
                return Result<CanFrame>::failure(ProtocolError::BufferFull);
            }
            return Result<CanFrame>::success(frame);
        }
        return Result<CanFrame>::failure(ProtocolError::UnsupportedCommand);
    }

    const auto *trainSpeed = std::get_if<TrainSpeed>(&message.payload);
    if (trainSpeed == nullptr || message.id.command != CanCommand::TrainSpeed) {
        const auto *trainDirection = std::get_if<TrainDirection>(&message.payload);
        if (trainDirection != nullptr && message.id.command == CanCommand::TrainDirection) {
            if (!trainDirection->direction.has_value()) {
                CanFrame frame;
                frame.identifier = message.id.encode();
                frame.length = TrainDirectionQueryLength;
                ::can_rail::protocol::ByteWriter writer(frame.data.data(), frame.data.size());
                if (!writer.writeU32BigEndian(trainDirection->locomotiveUid)) {
                    return Result<CanFrame>::failure(ProtocolError::BufferFull);
                }
                return Result<CanFrame>::success(frame);
            }

            const auto direction = static_cast<std::uint8_t>(*trainDirection->direction);
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
            if (trainFunction->functionNumber > 31) {
                return Result<CanFrame>::failure(ProtocolError::InvalidEnumValue);
            }
            if (trainFunction->value.has_value() && *trainFunction->value > 31) {
                return Result<CanFrame>::failure(ProtocolError::InvalidEnumValue);
            }

            CanFrame frame;
            frame.identifier = message.id.encode();
            frame.length =
                trainFunction->value.has_value() ? TrainFunctionLength : TrainFunctionQueryLength;
            ::can_rail::protocol::ByteWriter writer(frame.data.data(), frame.data.size());
            if (!writer.writeU32BigEndian(trainFunction->locomotiveUid) ||
                !writer.writeU8(trainFunction->functionNumber)) {
                return Result<CanFrame>::failure(ProtocolError::BufferFull);
            }
            if (trainFunction->value.has_value() && !writer.writeU8(*trainFunction->value)) {
                return Result<CanFrame>::failure(ProtocolError::BufferFull);
            }
            return Result<CanFrame>::success(frame);
        }

        return Result<CanFrame>::failure(ProtocolError::UnsupportedCommand);
    }

    CanFrame frame;
    frame.identifier = message.id.encode();
    frame.length = trainSpeed->speed.has_value() ? TrainSpeedLength : TrainSpeedQueryLength;

    ::can_rail::protocol::ByteWriter writer(frame.data.data(), frame.data.size());
    if (!writer.writeU32BigEndian(trainSpeed->locomotiveUid)) {
        return Result<CanFrame>::failure(ProtocolError::BufferFull);
    }
    if (trainSpeed->speed.has_value() && !writer.writeU16BigEndian(*trainSpeed->speed)) {
        return Result<CanFrame>::failure(ProtocolError::BufferFull);
    }

    return Result<CanFrame>::success(frame);
}

} // namespace can_rail::protocol::can
