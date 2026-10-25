#ifndef CAN_RAIL_PROTOCOL_COMMON_BYTE_READER_HPP
#define CAN_RAIL_PROTOCOL_COMMON_BYTE_READER_HPP

#include "protocol_error.hpp"
#include "result.hpp"

#include <cstddef>
#include <cstdint>

namespace can_rail::protocol {

class ByteReader {
  public:
    ByteReader(const std::uint8_t *data, std::size_t length) : data_(data), length_(length) {
    }

    Result<std::uint8_t> readU8() {
        if (remaining() < 1) {
            return Result<std::uint8_t>::failure(ProtocolError::BufferTooShort);
        }
        return Result<std::uint8_t>::success(data_[position_++]);
    }

    Result<std::uint16_t> readU16BigEndian() {
        if (remaining() < 2) {
            return Result<std::uint16_t>::failure(ProtocolError::BufferTooShort);
        }
        const auto value = static_cast<std::uint16_t>(data_[position_]) << 8 |
                           static_cast<std::uint16_t>(data_[position_ + 1]);
        position_ += 2;
        return Result<std::uint16_t>::success(value);
    }

    Result<std::uint32_t> readU32BigEndian() {
        if (remaining() < 4) {
            return Result<std::uint32_t>::failure(ProtocolError::BufferTooShort);
        }
        const auto value = static_cast<std::uint32_t>(data_[position_]) << 24 |
                           static_cast<std::uint32_t>(data_[position_ + 1]) << 16 |
                           static_cast<std::uint32_t>(data_[position_ + 2]) << 8 |
                           static_cast<std::uint32_t>(data_[position_ + 3]);
        position_ += 4;
        return Result<std::uint32_t>::success(value);
    }

    std::size_t remaining() const {
        return length_ - position_;
    }

  private:
    const std::uint8_t *data_;
    std::size_t length_;
    std::size_t position_{0};
};

} // namespace can_rail::protocol

#endif // CAN_RAIL_PROTOCOL_COMMON_BYTE_READER_HPP
