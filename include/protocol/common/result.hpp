#ifndef CAN_RAIL_PROTOCOL_COMMON_RESULT_HPP
#define CAN_RAIL_PROTOCOL_COMMON_RESULT_HPP

#include "protocol_error.hpp"

#include <utility>

namespace can_rail::protocol {

template <typename Value> class Result {
  public:
    static Result success(Value value) {
        return Result(std::move(value));
    }

    static Result failure(ProtocolError error) {
        return Result(error);
    }

    bool hasValue() const {
        return hasValue_;
    }

    explicit operator bool() const {
        return hasValue();
    }

    const Value &value() const {
        return value_;
    }

    ProtocolError error() const {
        return error_;
    }

  private:
    explicit Result(Value value) : value_(std::move(value)), hasValue_(true) {
    }

    explicit Result(ProtocolError error) : error_(error), hasValue_(false) {
    }

    Value value_{};
    ProtocolError error_{ProtocolError::InvalidDataLength};
    bool hasValue_{false};
};

} // namespace can_rail::protocol

#endif // CAN_RAIL_PROTOCOL_COMMON_RESULT_HPP
