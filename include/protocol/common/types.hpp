#ifndef CAN_RAIL_PROTOCOL_COMMON_TYPES_HPP
#define CAN_RAIL_PROTOCOL_COMMON_TYPES_HPP

#include <cstdint>

namespace can_rail::protocol {

enum class Direction : std::uint8_t {
    NoChange = 0,
    Forward = 1,
    Backward = 2,
    Change = 3,
};

enum class TrackProtocol : std::uint8_t {
    MM2 = 0,
    Mfx = 1,
    Dcc = 2,
};

enum class SystemState : std::uint8_t {
    Stop,
    Go,
    Halt,
};

} // namespace can_rail::protocol

#endif // CAN_RAIL_PROTOCOL_COMMON_TYPES_HPP
