#include <unity.h>

#include "protocol/can/can_codec.hpp"

#include <variant>

using can_rail::protocol::ProtocolError;
using can_rail::protocol::can::AccessoriesSwitch;
using can_rail::protocol::can::CanCodec;
using can_rail::protocol::can::CanCommand;
using can_rail::protocol::can::CanFrame;
using can_rail::protocol::can::CanId;
using can_rail::protocol::can::CanMessage;

void test_accessories_switch_decodes_default_and_timed_frames() {
    const CanId id{0, CanCommand::AccessoriesSwitch, false, 0x0300};
    const CanFrame defaultFrame{
        .identifier = id.encode(),
        .length = 6,
        .data = {0x43, 0x53, 0x32, 0x08, 0x01, 0x1F, 0x00, 0x00},
    };
    const CanFrame timedFrame{
        .identifier = id.encode(),
        .length = 8,
        .data = {0x43, 0x53, 0x32, 0x08, 0x02, 0x01, 0x00, 0x64},
    };

    const auto defaultMessage = CanCodec::decode(defaultFrame);
    const auto timedMessage = CanCodec::decode(timedFrame);

    TEST_ASSERT_TRUE(defaultMessage);
    TEST_ASSERT_TRUE(timedMessage);
    TEST_ASSERT_FALSE(
        std::get<AccessoriesSwitch>(defaultMessage.value().payload).switchingTime.has_value());
    TEST_ASSERT_EQUAL_UINT32(
        0x43533208, std::get<AccessoriesSwitch>(timedMessage.value().payload).accessoryUid);
    TEST_ASSERT_EQUAL_UINT8(2, std::get<AccessoriesSwitch>(timedMessage.value().payload).position);
    TEST_ASSERT_EQUAL_UINT8(1, std::get<AccessoriesSwitch>(timedMessage.value().payload).power);
    TEST_ASSERT_EQUAL_UINT16(
        100, std::get<AccessoriesSwitch>(timedMessage.value().payload).switchingTime.value());
}

void test_accessories_switch_encodes_default_and_timed_frames() {
    const CanId id{0, CanCommand::AccessoriesSwitch, false, 0x0300};
    const CanMessage defaultMessage{.id = id, .payload = AccessoriesSwitch{0x43533208, 1, 31, {}}};
    const CanMessage timedMessage{.id = id, .payload = AccessoriesSwitch{0x43533208, 2, 1, 100}};

    const auto defaultFrame = CanCodec::encode(defaultMessage);
    const auto timedFrame = CanCodec::encode(timedMessage);

    TEST_ASSERT_TRUE(defaultFrame);
    TEST_ASSERT_TRUE(timedFrame);
    TEST_ASSERT_EQUAL_UINT8(6, defaultFrame.value().length);
    TEST_ASSERT_EQUAL_UINT8(31, defaultFrame.value().data[5]);
    TEST_ASSERT_EQUAL_UINT8(8, timedFrame.value().length);
    TEST_ASSERT_EQUAL_UINT8(0x00, timedFrame.value().data[6]);
    TEST_ASSERT_EQUAL_UINT8(0x64, timedFrame.value().data[7]);
}

void test_accessories_switch_rejects_invalid_power_and_length() {
    const CanId id{0, CanCommand::AccessoriesSwitch, false, 0x0300};
    const CanMessage invalidPower{.id = id, .payload = AccessoriesSwitch{0x43533208, 1, 32, {}}};
    const CanFrame invalidLength{.identifier = id.encode(), .length = 7};

    const auto encoded = CanCodec::encode(invalidPower);
    const auto decoded = CanCodec::decode(invalidLength);

    TEST_ASSERT_FALSE(encoded);
    TEST_ASSERT_EQUAL(static_cast<int>(ProtocolError::InvalidEnumValue),
                      static_cast<int>(encoded.error()));
    TEST_ASSERT_FALSE(decoded);
    TEST_ASSERT_EQUAL(static_cast<int>(ProtocolError::InvalidDataLength),
                      static_cast<int>(decoded.error()));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_accessories_switch_decodes_default_and_timed_frames);
    RUN_TEST(test_accessories_switch_encodes_default_and_timed_frames);
    RUN_TEST(test_accessories_switch_rejects_invalid_power_and_length);
    return UNITY_END();
}
