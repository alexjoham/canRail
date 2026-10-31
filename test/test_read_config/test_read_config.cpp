#include <unity.h>

#include "protocol/can/can_codec.hpp"

#include <variant>

using can_rail::protocol::ProtocolError;
using can_rail::protocol::can::CanCodec;
using can_rail::protocol::can::CanCommand;
using can_rail::protocol::can::CanFrame;
using can_rail::protocol::can::CanId;
using can_rail::protocol::can::CanMessage;
using can_rail::protocol::can::ReadConfigRequest;
using can_rail::protocol::can::ReadConfigResponse;

void test_read_config_decodes_request_and_responses() {
    const CanId requestId{0, CanCommand::ReadConfig, false, 0x0300};
    const CanId responseId{0, CanCommand::ReadConfig, true, 0x0300};
    const CanFrame requestFrame{
        .identifier = requestId.encode(),
        .length = 7,
        .data = {0x43, 0x53, 0x32, 0x08, 0x81, 0x2C, 0x03, 0x00},
    };
    const CanFrame responseFrame{
        .identifier = responseId.encode(),
        .length = 7,
        .data = {0x43, 0x53, 0x32, 0x08, 0x81, 0x2C, 0x7F, 0x00},
    };
    const CanFrame missingValueFrame{
        .identifier = responseId.encode(),
        .length = 6,
        .data = {0x43, 0x53, 0x32, 0x08, 0x81, 0x2C, 0x00, 0x00},
    };

    const auto request = CanCodec::decode(requestFrame);
    const auto response = CanCodec::decode(responseFrame);
    const auto missingValue = CanCodec::decode(missingValueFrame);

    TEST_ASSERT_TRUE(request);
    TEST_ASSERT_TRUE(response);
    TEST_ASSERT_TRUE(missingValue);
    TEST_ASSERT_EQUAL_UINT8(32, std::get<ReadConfigRequest>(request.value().payload).cvIndex);
    TEST_ASSERT_EQUAL_UINT16(300, std::get<ReadConfigRequest>(request.value().payload).cvNumber);
    TEST_ASSERT_EQUAL_UINT8(3, std::get<ReadConfigRequest>(request.value().payload).count);
    TEST_ASSERT_EQUAL_UINT8(0x7F,
                            std::get<ReadConfigResponse>(response.value().payload).value.value());
    TEST_ASSERT_FALSE(std::get<ReadConfigResponse>(missingValue.value().payload).value.has_value());
}

void test_read_config_encodes_request_and_responses() {
    const CanId requestId{0, CanCommand::ReadConfig, false, 0x0300};
    const CanId responseId{0, CanCommand::ReadConfig, true, 0x0300};
    const CanMessage request{.id = requestId, .payload = ReadConfigRequest{0x43533208, 32, 300, 3}};
    const CanMessage response{.id = responseId,
                              .payload = ReadConfigResponse{0x43533208, 32, 300, 0x7F}};
    const CanMessage missingValue{.id = responseId,
                                  .payload = ReadConfigResponse{0x43533208, 32, 300, {}}};

    const auto requestFrame = CanCodec::encode(request);
    const auto responseFrame = CanCodec::encode(response);
    const auto missingValueFrame = CanCodec::encode(missingValue);

    TEST_ASSERT_TRUE(requestFrame);
    TEST_ASSERT_TRUE(responseFrame);
    TEST_ASSERT_TRUE(missingValueFrame);
    TEST_ASSERT_EQUAL_UINT8(7, requestFrame.value().length);
    TEST_ASSERT_EQUAL_UINT8(0x81, requestFrame.value().data[4]);
    TEST_ASSERT_EQUAL_UINT8(0x2C, requestFrame.value().data[5]);
    TEST_ASSERT_EQUAL_UINT8(3, requestFrame.value().data[6]);
    TEST_ASSERT_EQUAL_UINT8(0x7F, responseFrame.value().data[6]);
    TEST_ASSERT_EQUAL_UINT8(6, missingValueFrame.value().length);
}

void test_read_config_rejects_invalid_address_and_length() {
    const CanId id{0, CanCommand::ReadConfig, false, 0x0300};
    const CanMessage invalidAddress{.id = id, .payload = ReadConfigRequest{0x43533208, 64, 0, 1}};
    const CanFrame invalidLength{.identifier = id.encode(), .length = 5};

    const auto encoded = CanCodec::encode(invalidAddress);
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
    RUN_TEST(test_read_config_decodes_request_and_responses);
    RUN_TEST(test_read_config_encodes_request_and_responses);
    RUN_TEST(test_read_config_rejects_invalid_address_and_length);
    return UNITY_END();
}
