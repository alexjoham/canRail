#include <unity.h>

#include "protocol/can/can_frame.hpp"
#include "protocol/can/can_id.hpp"

using can_rail::protocol::ProtocolError;
using can_rail::protocol::can::CanCommand;
using can_rail::protocol::can::CanFrame;
using can_rail::protocol::can::CanId;

void test_can_id_round_trip() {
    const CanId original{7, CanCommand::TrainSpeed, true, 0x1234};
    const auto decoded = CanId::decode(original.encode());

    TEST_ASSERT_TRUE(decoded);
    TEST_ASSERT_EQUAL_UINT8(original.priority, decoded.value().priority);
    TEST_ASSERT_EQUAL_UINT8(static_cast<std::uint8_t>(original.command),
                            static_cast<std::uint8_t>(decoded.value().command));
    TEST_ASSERT_TRUE(decoded.value().response);
    TEST_ASSERT_EQUAL_UINT16(original.hash, decoded.value().hash);
}

void test_can_id_encodes_field_boundaries() {
    const CanId original{15, CanCommand::ConfigDataStream, false, 0xFFFF};
    const auto decoded = CanId::decode(original.encode());

    TEST_ASSERT_TRUE(decoded);
    TEST_ASSERT_EQUAL_UINT8(15, decoded.value().priority);
    TEST_ASSERT_EQUAL_UINT8(static_cast<std::uint8_t>(CanCommand::ConfigDataStream),
                            static_cast<std::uint8_t>(decoded.value().command));
    TEST_ASSERT_FALSE(decoded.value().response);
    TEST_ASSERT_EQUAL_UINT16(0xFFFF, decoded.value().hash);
}

void test_can_id_rejects_value_outside_standard_identifier_range() {
    const auto decoded = CanId::decode(0x20000000U);

    TEST_ASSERT_FALSE(decoded);
    TEST_ASSERT_EQUAL(static_cast<int>(ProtocolError::InvalidCanIdentifier),
                      static_cast<int>(decoded.error()));
}

void test_can_frame_starts_empty() {
    const CanFrame frame{};

    TEST_ASSERT_EQUAL_UINT32(0, frame.identifier);
    TEST_ASSERT_EQUAL_UINT8(0, frame.length);
    TEST_ASSERT_EQUAL_UINT32(CanFrame::MaxDataLength, frame.data.size());
    for (const auto byte : frame.data) {
        TEST_ASSERT_EQUAL_UINT8(0, byte);
    }
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_can_id_round_trip);
    RUN_TEST(test_can_id_encodes_field_boundaries);
    RUN_TEST(test_can_id_rejects_value_outside_standard_identifier_range);
    RUN_TEST(test_can_frame_starts_empty);
    return UNITY_END();
}
