#include <unity.h>

#include "protocol/can/can_codec.hpp"

#include <variant>

using can_rail::protocol::ProtocolError;
using can_rail::protocol::can::CanCodec;
using can_rail::protocol::can::CanCommand;
using can_rail::protocol::can::CanFrame;
using can_rail::protocol::can::CanId;
using can_rail::protocol::can::CanMessage;
using can_rail::protocol::can::MfxBind;

void test_mfx_bind_decodes_uid_and_sid() {
    const CanId requestId{0, CanCommand::MfxBind, false, 0x0300};
    const CanId responseId{0, CanCommand::MfxBind, true, 0x0300};
    const CanFrame requestFrame{
        .identifier = requestId.encode(),
        .length = 6,
        .data = {0xFF, 0xFA, 0x8C, 0x43, 0x00, 0x05, 0x00, 0x00},
    };
    const CanFrame responseFrame{
        .identifier = responseId.encode(),
        .length = 6,
        .data = {0xFF, 0xFA, 0x8C, 0x43, 0x00, 0x05, 0x00, 0x00},
    };

    const auto request = CanCodec::decode(requestFrame);
    const auto response = CanCodec::decode(responseFrame);

    TEST_ASSERT_TRUE(request);
    TEST_ASSERT_TRUE(response);
    TEST_ASSERT_EQUAL_UINT32(0xFFFA8C43, std::get<MfxBind>(request.value().payload).mfxUid);
    TEST_ASSERT_EQUAL_UINT16(5, std::get<MfxBind>(response.value().payload).mfxSid);
}

void test_mfx_bind_encodes_uid_and_sid() {
    const CanId id{0, CanCommand::MfxBind, true, 0x0300};
    const CanMessage message{.id = id, .payload = MfxBind{0xFFFA8C43, 5}};

    const auto encoded = CanCodec::encode(message);

    TEST_ASSERT_TRUE(encoded);
    TEST_ASSERT_EQUAL_UINT8(6, encoded.value().length);
    TEST_ASSERT_EQUAL_UINT8(0xFF, encoded.value().data[0]);
    TEST_ASSERT_EQUAL_UINT8(0xFA, encoded.value().data[1]);
    TEST_ASSERT_EQUAL_UINT8(0x8C, encoded.value().data[2]);
    TEST_ASSERT_EQUAL_UINT8(0x43, encoded.value().data[3]);
    TEST_ASSERT_EQUAL_UINT8(0x00, encoded.value().data[4]);
    TEST_ASSERT_EQUAL_UINT8(0x05, encoded.value().data[5]);
}

void test_mfx_bind_rejects_wrong_length() {
    const CanId id{0, CanCommand::MfxBind, false, 0x0300};
    const CanFrame frame{.identifier = id.encode(), .length = 5};

    const auto decoded = CanCodec::decode(frame);

    TEST_ASSERT_FALSE(decoded);
    TEST_ASSERT_EQUAL(static_cast<int>(ProtocolError::InvalidDataLength),
                      static_cast<int>(decoded.error()));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_mfx_bind_decodes_uid_and_sid);
    RUN_TEST(test_mfx_bind_encodes_uid_and_sid);
    RUN_TEST(test_mfx_bind_rejects_wrong_length);
    return UNITY_END();
}
