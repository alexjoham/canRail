#ifndef CAN_RAIL_PROTOCOL_CAN_CAN_FRAME_HPP
#define CAN_RAIL_PROTOCOL_CAN_CAN_FRAME_HPP

#include <array>
#include <cstddef>
#include <cstdint>

namespace can_rail::protocol::can {

struct CanFrame {
    static constexpr std::size_t MaxDataLength = 8;

    std::uint32_t identifier{0};
    std::uint8_t length{0};
    std::array<std::uint8_t, MaxDataLength> data{};
};

} // namespace can_rail::protocol::can

#endif // CAN_RAIL_PROTOCOL_CAN_CAN_FRAME_HPP
