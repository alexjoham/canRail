#ifndef CAN_RAIL_PROTOCOL_CAN_CAN_ID_HPP
#define CAN_RAIL_PROTOCOL_CAN_CAN_ID_HPP

#include "../common/result.hpp"
#include "can_commands.hpp"

#include <cstdint>

namespace can_rail::protocol::can {

struct CanId {
    std::uint8_t priority{0};
    CanCommand command{CanCommand::System};
    bool response{false};
    std::uint16_t hash{0};

    std::uint32_t encode() const {
        const auto priorityHigh = static_cast<std::uint32_t>((priority >> 2) & 0x03);
        const auto priorityLow = static_cast<std::uint32_t>(priority & 0x03);

        return (priorityHigh << 27) | (priorityLow << 25) |
               (static_cast<std::uint32_t>(command) << 17) |
               (static_cast<std::uint32_t>(response) << 16) | hash;
    }

    static Result<CanId> decode(std::uint32_t raw) {
        if (raw > 0x1FFFFFFFU) {
            return Result<CanId>::failure(ProtocolError::InvalidCanIdentifier);
        }

        CanId id;
        id.priority = static_cast<std::uint8_t>(((raw >> 27) & 0x03) << 2 | ((raw >> 25) & 0x03));
        id.command = static_cast<CanCommand>((raw >> 17) & 0xFF);
        id.response = ((raw >> 16) & 0x01) != 0;
        id.hash = static_cast<std::uint16_t>(raw & 0xFFFF);
        return Result<CanId>::success(id);
    }
};

} // namespace can_rail::protocol::can

#endif // CAN_RAIL_PROTOCOL_CAN_CAN_ID_HPP
