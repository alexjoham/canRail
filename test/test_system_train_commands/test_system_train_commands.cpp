#include <unity.h>

#include "protocol/can/can_codec.hpp"

#include <variant>

using can_rail::protocol::ProtocolError;
using can_rail::protocol::can::CanCodec;
using can_rail::protocol::can::CanCommand;
using can_rail::protocol::can::CanFrame;
using can_rail::protocol::can::CanId;
using can_rail::protocol::can::CanMessage;
using can_rail::protocol::can::SystemTrainCycleStop;
using can_rail::protocol::can::SystemTrainEmergencyHalt;

void test_train_system_commands_decode_to_typed_messages() {
    const CanId id{0, CanCommand::System, false, 0x0300};
    const CanFrame emergencyFrame{
        .identifier = id.encode(),
        .length = 5,
        .data = {0x00, 0x00, 0x00, 0x48, 0x03, 0x00, 0x00, 0x00},
    };
    const CanFrame cycleFrame{
        .identifier = id.encode(),
        .length = 5,
        .data = {0x00, 0x00, 0x00, 0x48, 0x04, 0x00, 0x00, 0x00},
    };

    const auto emergency = CanCodec::decode(emergencyFrame);
    const auto cycle = CanCodec::decode(cycleFrame);

    TEST_ASSERT_TRUE(emergency);
    TEST_ASSERT_TRUE(cycle);
    TEST_ASSERT_TRUE(std::holds_alternative<SystemTrainEmergencyHalt>(emergency.value().payload));
    TEST_ASSERT_TRUE(std::holds_alternative<SystemTrainCycleStop>(cycle.value().payload));
    TEST_ASSERT_EQUAL_UINT32(
        0x48, std::get<SystemTrainEmergencyHalt>(emergency.value().payload).locomotiveUid);
    TEST_ASSERT_EQUAL_UINT32(0x48,
                             std::get<SystemTrainCycleStop>(cycle.value().payload).locomotiveUid);
}

void test_train_system_commands_encode_protocol_frames() {
    const CanId id{0, CanCommand::System, true, 0x0300};
    const CanMessage emergencyMessage{
        .id = id,
        .payload = SystemTrainEmergencyHalt{0x00000048},
    };
    const CanMessage cycleMessage{
        .id = id,
        .payload = SystemTrainCycleStop{0x00000048},
    };

    const auto emergency = CanCodec::encode(emergencyMessage);
    const auto cycle = CanCodec::encode(cycleMessage);

    TEST_ASSERT_TRUE(emergency);
    TEST_ASSERT_TRUE(cycle);
    TEST_ASSERT_EQUAL_UINT8(5, emergency.value().length);
    TEST_ASSERT_EQUAL_UINT8(0x03, emergency.value().data[4]);
    TEST_ASSERT_EQUAL_UINT8(0x04, cycle.value().data[4]);
    TEST_ASSERT_EQUAL_UINT8(0x00, cycle.value().data[0]);
    TEST_ASSERT_EQUAL_UINT8(0x48, cycle.value().data[3]);
}

void test_train_system_commands_reject_wrong_length() {
    const CanId id{0, CanCommand::System, false, 0x0300};
    const CanFrame frame{.identifier = id.encode(), .length = 4};

    const auto decoded = CanCodec::decode(frame);

    TEST_ASSERT_FALSE(decoded);
    TEST_ASSERT_EQUAL(static_cast<int>(ProtocolError::InvalidDataLength),
                      static_cast<int>(decoded.error()));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_train_system_commands_decode_to_typed_messages);
    RUN_TEST(test_train_system_commands_encode_protocol_frames);
    RUN_TEST(test_train_system_commands_reject_wrong_length);
    return UNITY_END();
}
