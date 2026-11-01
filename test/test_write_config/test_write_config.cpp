#include <unity.h>

#include "protocol/can/can_codec.hpp"

#include <variant>

using can_rail::protocol::ProtocolError;
using can_rail::protocol::can::CanCodec;
using can_rail::protocol::can::CanCommand;
using can_rail::protocol::can::CanFrame;
using can_rail::protocol::can::CanId;
using can_rail::protocol::can::CanMessage;
using can_rail::protocol::can::WriteConfigRequest;
using can_rail::protocol::can::WriteConfigResponse;

void test_write_config_decodes_request_and_response() {
    const CanId requestId{0, CanCommand::WriteConfig, false, 0x0300};
    const CanId responseId{0, CanCommand::WriteConfig, true, 0x0300};
    const CanFrame requestFrame{
        .identifier = requestId.encode(),
        .length = 8,
        .data = {0x43, 0x53, 0x32, 0x08, 0x81, 0x2C, 0x7F, 0x01},
    };
    const CanFrame responseFrame{
        .identifier = responseId.encode(),
        .length = 8,
        .data = {0x43, 0x53, 0x32, 0x08, 0x81, 0x2C, 0x7F, 0x03},
    };

    const auto request = CanCodec::decode(requestFrame);
    const auto response = CanCodec::decode(responseFrame);

    TEST_ASSERT_TRUE(request);
    TEST_ASSERT_TRUE(response);
    TEST_ASSERT_EQUAL_UINT8(32, std::get<WriteConfigRequest>(request.value().payload).cvIndex);
    TEST_ASSERT_EQUAL_UINT16(300, std::get<WriteConfigRequest>(request.value().payload).cvNumber);
    TEST_ASSERT_EQUAL_UINT8(0x7F, std::get<WriteConfigRequest>(request.value().payload).value);
    TEST_ASSERT_EQUAL_UINT8(1, std::get<WriteConfigRequest>(request.value().payload).control);
    TEST_ASSERT_EQUAL_UINT8(3, std::get<WriteConfigResponse>(response.value().payload).result);
}

void test_write_config_encodes_request_and_response() {
    const CanId requestId{0, CanCommand::WriteConfig, false, 0x0300};
    const CanId responseId{0, CanCommand::WriteConfig, true, 0x0300};
    const CanMessage request{.id = requestId,
                             .payload = WriteConfigRequest{0x43533208, 32, 300, 0x7F, 1}};
    const CanMessage response{.id = responseId,
                              .payload = WriteConfigResponse{0x43533208, 32, 300, 0x7F, 3}};

    const auto requestFrame = CanCodec::encode(request);
    const auto responseFrame = CanCodec::encode(response);

    TEST_ASSERT_TRUE(requestFrame);
    TEST_ASSERT_TRUE(responseFrame);
    TEST_ASSERT_EQUAL_UINT8(8, requestFrame.value().length);
    TEST_ASSERT_EQUAL_UINT8(0x81, requestFrame.value().data[4]);
    TEST_ASSERT_EQUAL_UINT8(0x2C, requestFrame.value().data[5]);
    TEST_ASSERT_EQUAL_UINT8(0x7F, requestFrame.value().data[6]);
    TEST_ASSERT_EQUAL_UINT8(1, requestFrame.value().data[7]);
    TEST_ASSERT_EQUAL_UINT8(3, responseFrame.value().data[7]);
}

void test_write_config_rejects_invalid_flags_and_length() {
    const CanId id{0, CanCommand::WriteConfig, false, 0x0300};
    const CanMessage invalidFlags{.id = id,
                                  .payload = WriteConfigRequest{0x43533208, 0, 1, 0x7F, 4}};
    const CanFrame invalidLength{.identifier = id.encode(), .length = 7};

    const auto encoded = CanCodec::encode(invalidFlags);
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
    RUN_TEST(test_write_config_decodes_request_and_response);
    RUN_TEST(test_write_config_encodes_request_and_response);
    RUN_TEST(test_write_config_rejects_invalid_flags_and_length);
    return UNITY_END();
}
