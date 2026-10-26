#ifndef CAN_RAIL_PROTOCOL_CAN_CAN_MESSAGES_HPP
#define CAN_RAIL_PROTOCOL_CAN_CAN_MESSAGES_HPP

#include "../common/types.hpp"
#include "can_commands.hpp"

#include <array>
#include <cstdint>
#include <optional>

namespace can_rail::protocol::can {

struct TrainSpeed {
    std::uint32_t locomotiveUid;
    std::uint16_t speed;
};

struct TrainDirection {
    std::uint32_t locomotiveUid;
    Direction direction;
};

struct TrainFunction {
    std::uint32_t locomotiveUid;
    std::uint8_t functionNumber;
    bool enabled;
};

struct SystemStop {
    std::uint32_t deviceUid;
};

struct SystemGo {
    std::uint32_t deviceUid;
};

struct SystemHalt {
    std::uint32_t deviceUid;
};

struct UnknownCanMessage {
    CanCommand command;
    std::uint8_t length;
    std::array<std::uint8_t, 8> data{};
};

} // namespace can_rail::protocol::can

#endif // CAN_RAIL_PROTOCOL_CAN_CAN_MESSAGES_HPP
