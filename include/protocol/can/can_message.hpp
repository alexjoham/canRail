#ifndef CAN_RAIL_PROTOCOL_CAN_CAN_MESSAGE_HPP
#define CAN_RAIL_PROTOCOL_CAN_CAN_MESSAGE_HPP

#include "can_id.hpp"
#include "can_messages.hpp"

#include <variant>

namespace can_rail::protocol::can {

using CanPayload =
    std::variant<TrainSpeed, TrainDirection, TrainFunction, SystemStop, SystemGo, SystemHalt,
                 SystemTrainEmergencyHalt, SystemTrainCycleStop, SystemTrainDataProtocol,
                 SystemSwitchingTime, SystemTrackProtocolActivate, SystemMfxNewRegistrationCounter,
                 SystemOverload, SystemStatus, UnknownCanMessage>;

struct CanMessage {
    CanId id;
    CanPayload payload;
};

} // namespace can_rail::protocol::can

#endif // CAN_RAIL_PROTOCOL_CAN_CAN_MESSAGE_HPP
