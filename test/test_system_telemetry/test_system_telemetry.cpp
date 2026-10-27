#include <unity.h>

#include "protocol/can/can_codec.hpp"

#include <variant>

using can_rail::protocol::ProtocolError;
using can_rail::protocol::can::CanCodec;
using can_rail::protocol::can::CanCommand;
using can_rail::protocol::can::CanFrame;
using can_rail::protocol::can::CanId;
using can_rail::protocol::can::CanMessage;
using can_rail::protocol::can::SystemMfxNewRegistrationCounter;
using can_rail::protocol::can::SystemOverload;

void test_system_telemetry_decodes_typed_messages() {
    const CanId id{0, CanCommand::System, false, 0x0300};
    const CanFrame counterFrame{
        .identifier = id.encode(),
        .length = 7,
        .data = {0x43, 0x53, 0x32, 0x08, 0x09, 0x00, 0x02, 0x00},
    };
    const CanFrame overloadFrame{
        .identifier = id.encode(),
        .length = 6,
        .data = {0x43, 0x53, 0x32, 0x08, 0x0A, 0x01, 0x00, 0x00},
    };

    const auto counter = CanCodec::decode(counterFrame);
    const auto overload = CanCodec::decode(overloadFrame);

    TEST_ASSERT_TRUE(counter);
    TEST_ASSERT_TRUE(overload);
    TEST_ASSERT_EQUAL_UINT32(
        0x43533208, std::get<SystemMfxNewRegistrationCounter>(counter.value().payload).deviceUid);
    TEST_ASSERT_EQUAL_UINT16(
        2, std::get<SystemMfxNewRegistrationCounter>(counter.value().payload).counter);
    TEST_ASSERT_EQUAL_UINT32(0x43533208,
                             std::get<SystemOverload>(overload.value().payload).deviceUid);
    TEST_ASSERT_EQUAL_UINT8(1, std::get<SystemOverload>(overload.value().payload).channel);
}

void test_system_telemetry_encodes_protocol_frames() {
    const CanId id{0, CanCommand::System, true, 0x0300};
    const CanMessage counterMessage{
        .id = id,
        .payload = SystemMfxNewRegistrationCounter{0x43533208, 2},
    };
    const CanMessage overloadMessage{
        .id = id,
        .payload = SystemOverload{0x43533208, 1},
    };

    const auto counter = CanCodec::encode(counterMessage);
    const auto overload = CanCodec::encode(overloadMessage);

    TEST_ASSERT_TRUE(counter);
    TEST_ASSERT_TRUE(overload);
    TEST_ASSERT_EQUAL_UINT8(7, counter.value().length);
    TEST_ASSERT_EQUAL_UINT8(0x09, counter.value().data[4]);
    TEST_ASSERT_EQUAL_UINT8(0x00, counter.value().data[5]);
    TEST_ASSERT_EQUAL_UINT8(0x02, counter.value().data[6]);
    TEST_ASSERT_EQUAL_UINT8(6, overload.value().length);
    TEST_ASSERT_EQUAL_UINT8(0x0A, overload.value().data[4]);
    TEST_ASSERT_EQUAL_UINT8(1, overload.value().data[5]);
}

void test_system_telemetry_rejects_wrong_length() {
    const CanId id{0, CanCommand::System, false, 0x0300};
    const CanFrame frame{
        .identifier = id.encode(),
        .length = 5,
        .data = {0x43, 0x53, 0x32, 0x08, 0x0A, 0x00, 0x00, 0x00},
    };

    const auto decoded = CanCodec::decode(frame);

    TEST_ASSERT_FALSE(decoded);
    TEST_ASSERT_EQUAL(static_cast<int>(ProtocolError::InvalidDataLength),
                      static_cast<int>(decoded.error()));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_system_telemetry_decodes_typed_messages);
    RUN_TEST(test_system_telemetry_encodes_protocol_frames);
    RUN_TEST(test_system_telemetry_rejects_wrong_length);
    return UNITY_END();
}
