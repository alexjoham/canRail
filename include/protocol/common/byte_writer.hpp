#ifndef CAN_RAIL_PROTOCOL_COMMON_BYTE_WRITER_HPP
#define CAN_RAIL_PROTOCOL_COMMON_BYTE_WRITER_HPP

#include "protocol_error.hpp"
#include "result.hpp"

#include <cstddef>
#include <cstdint>

namespace can_rail::protocol {

class ByteWriter {
  public:
    ByteWriter(std::uint8_t *data, std::size_t capacity) : data_(data), capacity_(capacity) {
    }

    Result<bool> writeU8(std::uint8_t value) {
        if (remaining() < 1) {
            return Result<bool>::failure(ProtocolError::BufferFull);
        }
        data_[position_++] = value;
        return Result<bool>::success(true);
    }

    Result<bool> writeU16BigEndian(std::uint16_t value) {
        if (remaining() < 2) {
            return Result<bool>::failure(ProtocolError::BufferFull);
        }
        data_[position_++] = static_cast<std::uint8_t>(value >> 8);
        data_[position_++] = static_cast<std::uint8_t>(value);
        return Result<bool>::success(true);
    }

    Result<bool> writeU32BigEndian(std::uint32_t value) {
        if (remaining() < 4) {
            return Result<bool>::failure(ProtocolError::BufferFull);
        }
        data_[position_++] = static_cast<std::uint8_t>(value >> 24);
        data_[position_++] = static_cast<std::uint8_t>(value >> 16);
        data_[position_++] = static_cast<std::uint8_t>(value >> 8);
        data_[position_++] = static_cast<std::uint8_t>(value);
        return Result<bool>::success(true);
    }

    std::size_t size() const {
        return position_;
    }

    std::size_t remaining() const {
        return capacity_ - position_;
    }

  private:
    std::uint8_t *data_;
    std::size_t capacity_;
    std::size_t position_{0};
};

} // namespace can_rail::protocol

#endif // CAN_RAIL_PROTOCOL_COMMON_BYTE_WRITER_HPP
