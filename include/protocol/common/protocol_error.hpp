#ifndef CAN_RAIL_PROTOCOL_COMMON_PROTOCOL_ERROR_HPP
#define CAN_RAIL_PROTOCOL_COMMON_PROTOCOL_ERROR_HPP

namespace can_rail::protocol {

enum class ProtocolError {
    BufferTooShort,
    BufferFull,
    InvalidCanIdentifier,
    InvalidDataLength,
    InvalidEnumValue,
    UnsupportedCommand,
};

} // namespace can_rail::protocol

#endif // CAN_RAIL_PROTOCOL_COMMON_PROTOCOL_ERROR_HPP
