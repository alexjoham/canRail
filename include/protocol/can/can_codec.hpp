#ifndef CAN_RAIL_PROTOCOL_CAN_CAN_CODEC_HPP
#define CAN_RAIL_PROTOCOL_CAN_CAN_CODEC_HPP

#include "../common/result.hpp"
#include "can_frame.hpp"
#include "can_message.hpp"

namespace can_rail::protocol::can {

class CanCodec {
  public:
    static Result<CanMessage> decode(const CanFrame &frame);
    static Result<CanFrame> encode(const CanMessage &message);
};

} // namespace can_rail::protocol::can

#endif // CAN_RAIL_PROTOCOL_CAN_CAN_CODEC_HPP
