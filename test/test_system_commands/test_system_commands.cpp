#include <unity.h>

#include "protocol/can/can_codec.hpp"

#include <variant>

using can_rail::protocol::ProtocolError;
using can_rail::protocol::can::CanCodec;
using can_rail::protocol::can::CanCommand;
using can_rail::protocol::can::CanFrame;
using can_rail::protocol::can::CanId;
using can_rail::protocol::can::CanMessage;
using can_rail::protocol::can::SystemGo;
using can_rail::protocol::can::SystemHalt;
using can_rail::protocol::can::SystemStop;

void test_system_commands_decode_to_typed_messages() {
    const CanId id{0, CanCommand::System, false, 0x0300};
    const CanFrame stopFrame{
        .identifier = id.encode(),
        .length = 5,
        .data = {0x43, 0x53, 0x32, 0x08, 0x00, 0x00, 0x00, 0x00},
    };
    const CanFrame goFrame{
        .identifier = id.encode(),
        .length = 5,
        .data = {0x43, 0x53, 0x32, 0x08, 0x01, 0x00, 0x00, 0x00},
    };
    const CanFrame haltFrame{
        .identifier = id.encode(),
        .length = 5,
        .data = {0x43, 0x53, 0x32, 0x08, 0x02, 0x00, 0x00, 0x00},
    };

    const auto stop = CanCodec::decode(stopFrame);
    const auto go = CanCodec::decode(goFrame);
    const auto halt = CanCodec::decode(haltFrame);

    TEST_ASSERT_TRUE(stop);
    TEST_ASSERT_TRUE(go);
    TEST_ASSERT_TRUE(halt);
    TEST_ASSERT_TRUE(std::holds_alternative<SystemStop>(stop.value().payload));
    TEST_ASSERT_TRUE(std::holds_alternative<SystemGo>(go.value().payload));
    TEST_ASSERT_TRUE(std::holds_alternative<SystemHalt>(halt.value().payload));
    TEST_ASSERT_EQUAL_UINT32(0x43533208, std::get<SystemStop>(stop.value().payload).deviceUid);
    TEST_ASSERT_EQUAL_UINT32(0x43533208, std::get<SystemGo>(go.value().payload).deviceUid);
    TEST_ASSERT_EQUAL_UINT32(0x43533208, std::get<SystemHalt>(halt.value().payload).deviceUid);
}

void test_system_commands_encode_protocol_frames() {
    const CanId id{0, CanCommand::System, true, 0x0300};
    const CanMessage stopMessage{.id = id, .payload = SystemStop{0}};
    const CanMessage goMessage{.id = id, .payload = SystemGo{0x43533208}};
    const CanMessage haltMessage{.id = id, .payload = SystemHalt{0x43533208}};

    const auto stop = CanCodec::encode(stopMessage);
    const auto go = CanCodec::encode(goMessage);
    const auto halt = CanCodec::encode(haltMessage);

    TEST_ASSERT_TRUE(stop);
    TEST_ASSERT_TRUE(go);
    TEST_ASSERT_TRUE(halt);
    TEST_ASSERT_EQUAL_UINT8(5, stop.value().length);
    TEST_ASSERT_EQUAL_UINT8(0x00, stop.value().data[4]);
    TEST_ASSERT_EQUAL_UINT8(0x01, go.value().data[4]);
    TEST_ASSERT_EQUAL_UINT8(0x02, halt.value().data[4]);
    TEST_ASSERT_EQUAL_UINT8(0x43, go.value().data[0]);
    TEST_ASSERT_EQUAL_UINT8(0x53, go.value().data[1]);
    TEST_ASSERT_EQUAL_UINT8(0x32, go.value().data[2]);
    TEST_ASSERT_EQUAL_UINT8(0x08, go.value().data[3]);
}

void test_system_commands_reject_unknown_subcommand() {
    const CanId id{0, CanCommand::System, false, 0x0300};
    const CanFrame frame{
        .identifier = id.encode(),
        .length = 5,
        .data = {0x00, 0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00},
    };

    const auto decoded = CanCodec::decode(frame);

    TEST_ASSERT_FALSE(decoded);
    TEST_ASSERT_EQUAL(static_cast<int>(ProtocolError::InvalidEnumValue),
                      static_cast<int>(decoded.error()));
}

void test_system_commands_reject_wrong_length() {
    const CanId id{0, CanCommand::System, false, 0x0300};
    const CanFrame frame{.identifier = id.encode(), .length = 4};

    const auto decoded = CanCodec::decode(frame);

    TEST_ASSERT_FALSE(decoded);
    TEST_ASSERT_EQUAL(static_cast<int>(ProtocolError::InvalidDataLength),
                      static_cast<int>(decoded.error()));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_system_commands_decode_to_typed_messages);
    RUN_TEST(test_system_commands_encode_protocol_frames);
    RUN_TEST(test_system_commands_reject_unknown_subcommand);
    RUN_TEST(test_system_commands_reject_wrong_length);
    return UNITY_END();
}
