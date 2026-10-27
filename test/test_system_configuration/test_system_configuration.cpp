#include <unity.h>

#include "protocol/can/can_codec.hpp"

#include <variant>

using can_rail::protocol::ProtocolError;
using can_rail::protocol::can::CanCodec;
using can_rail::protocol::can::CanCommand;
using can_rail::protocol::can::CanFrame;
using can_rail::protocol::can::CanId;
using can_rail::protocol::can::CanMessage;
using can_rail::protocol::can::SystemSwitchingTime;
using can_rail::protocol::can::SystemTrackProtocolActivate;
using can_rail::protocol::can::SystemTrainDataProtocol;

void test_system_configuration_decodes_typed_messages() {
    const CanId id{0, CanCommand::System, false, 0x0300};
    const CanFrame trainDataFrame{
        .identifier = id.encode(),
        .length = 6,
        .data = {0x00, 0x00, 0x00, 0x48, 0x05, 0x02, 0x00, 0x00},
    };
    const CanFrame switchingFrame{
        .identifier = id.encode(),
        .length = 7,
        .data = {0x43, 0x53, 0x32, 0x08, 0x06, 0x01, 0xF4, 0x00},
    };
    const CanFrame activationFrame{
        .identifier = id.encode(),
        .length = 6,
        .data = {0x43, 0x53, 0x32, 0x08, 0x08, 0x05, 0x00, 0x00},
    };

    const auto trainData = CanCodec::decode(trainDataFrame);
    const auto switching = CanCodec::decode(switchingFrame);
    const auto activation = CanCodec::decode(activationFrame);

    TEST_ASSERT_TRUE(trainData);
    TEST_ASSERT_TRUE(switching);
    TEST_ASSERT_TRUE(activation);
    TEST_ASSERT_EQUAL_UINT32(
        0x48, std::get<SystemTrainDataProtocol>(trainData.value().payload).locomotiveUid);
    TEST_ASSERT_EQUAL_UINT8(2,
                            std::get<SystemTrainDataProtocol>(trainData.value().payload).protocol);
    TEST_ASSERT_EQUAL_UINT32(0x43533208,
                             std::get<SystemSwitchingTime>(switching.value().payload).deviceUid);
    TEST_ASSERT_EQUAL_UINT16(500, std::get<SystemSwitchingTime>(switching.value().payload).time);
    TEST_ASSERT_EQUAL_UINT8(
        5, std::get<SystemTrackProtocolActivate>(activation.value().payload).protocolMask);
}

void test_system_configuration_encodes_typed_messages() {
    const CanId id{0, CanCommand::System, true, 0x0300};
    const CanMessage trainDataMessage{
        .id = id,
        .payload = SystemTrainDataProtocol{0x48, 2},
    };
    const CanMessage switchingMessage{
        .id = id,
        .payload = SystemSwitchingTime{0x43533208, 500},
    };
    const CanMessage activationMessage{
        .id = id,
        .payload = SystemTrackProtocolActivate{0x43533208, 5},
    };

    const auto trainData = CanCodec::encode(trainDataMessage);
    const auto switching = CanCodec::encode(switchingMessage);
    const auto activation = CanCodec::encode(activationMessage);

    TEST_ASSERT_TRUE(trainData);
    TEST_ASSERT_TRUE(switching);
    TEST_ASSERT_TRUE(activation);
    TEST_ASSERT_EQUAL_UINT8(6, trainData.value().length);
    TEST_ASSERT_EQUAL_UINT8(0x05, trainData.value().data[4]);
    TEST_ASSERT_EQUAL_UINT8(2, trainData.value().data[5]);
    TEST_ASSERT_EQUAL_UINT8(7, switching.value().length);
    TEST_ASSERT_EQUAL_UINT8(0x01, switching.value().data[5]);
    TEST_ASSERT_EQUAL_UINT8(0xF4, switching.value().data[6]);
    TEST_ASSERT_EQUAL_UINT8(6, activation.value().length);
    TEST_ASSERT_EQUAL_UINT8(0x08, activation.value().data[4]);
    TEST_ASSERT_EQUAL_UINT8(5, activation.value().data[5]);
}

void test_system_configuration_rejects_wrong_length() {
    const CanId id{0, CanCommand::System, false, 0x0300};
    const CanFrame frame{
        .identifier = id.encode(),
        .length = 5,
        .data = {0x00, 0x00, 0x00, 0x48, 0x05, 0x00, 0x00, 0x00},
    };

    const auto decoded = CanCodec::decode(frame);

    TEST_ASSERT_FALSE(decoded);
    TEST_ASSERT_EQUAL(static_cast<int>(ProtocolError::InvalidDataLength),
                      static_cast<int>(decoded.error()));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_system_configuration_decodes_typed_messages);
    RUN_TEST(test_system_configuration_encodes_typed_messages);
    RUN_TEST(test_system_configuration_rejects_wrong_length);
    return UNITY_END();
}
