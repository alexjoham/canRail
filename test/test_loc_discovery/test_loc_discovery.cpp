#include <unity.h>

#include "protocol/can/can_codec.hpp"

#include <variant>

using can_rail::protocol::ProtocolError;
using can_rail::protocol::can::CanCodec;
using can_rail::protocol::can::CanCommand;
using can_rail::protocol::can::CanFrame;
using can_rail::protocol::can::CanId;
using can_rail::protocol::can::CanMessage;
using can_rail::protocol::can::LocDiscoveryRequest;
using can_rail::protocol::can::LocDiscoveryResponse;

void test_loc_discovery_decodes_request_and_response_forms() {
    const CanId requestId{0, CanCommand::TrainDiscovery, false, 0x0300};
    const CanId responseId{0, CanCommand::TrainDiscovery, true, 0x0300};
    const CanFrame protocolFrame{.identifier = requestId.encode(), .length = 1, .data = {0x20}};
    const CanFrame addressFrame{.identifier = requestId.encode(),
                                .length = 5,
                                .data = {0xFF, 0xFA, 0x8C, 0x43, 0x20, 0, 0, 0}};
    const CanFrame responseFrame{.identifier = responseId.encode(),
                                 .length = 6,
                                 .data = {0xFF, 0xFA, 0x8C, 0x43, 0x20, 0xD1, 0, 0}};

    const auto protocol = CanCodec::decode(protocolFrame);
    const auto address = CanCodec::decode(addressFrame);
    const auto response = CanCodec::decode(responseFrame);

    TEST_ASSERT_TRUE(protocol);
    TEST_ASSERT_TRUE(address);
    TEST_ASSERT_TRUE(response);
    TEST_ASSERT_EQUAL_UINT8(
        0x20, std::get<LocDiscoveryRequest>(protocol.value().payload).protocol.value());
    TEST_ASSERT_EQUAL_UINT32(0xFFFA8C43,
                             std::get<LocDiscoveryRequest>(address.value().payload).mfxUid.value());
    TEST_ASSERT_EQUAL_UINT8(
        0xD1, std::get<LocDiscoveryResponse>(response.value().payload).askRatio.value());
}

void test_loc_discovery_encodes_empty_protocol_and_response_forms() {
    const CanId requestId{0, CanCommand::TrainDiscovery, false, 0x0300};
    const CanId responseId{0, CanCommand::TrainDiscovery, true, 0x0300};
    const CanMessage empty{.id = requestId, .payload = LocDiscoveryRequest{}};
    const CanMessage response{.id = responseId,
                              .payload = LocDiscoveryResponse{0xFFFA8C43, 32, 0xD1}};

    const auto emptyFrame = CanCodec::encode(empty);
    const auto responseFrame = CanCodec::encode(response);

    TEST_ASSERT_TRUE(emptyFrame);
    TEST_ASSERT_TRUE(responseFrame);
    TEST_ASSERT_EQUAL_UINT8(0, emptyFrame.value().length);
    TEST_ASSERT_EQUAL_UINT8(6, responseFrame.value().length);
    TEST_ASSERT_EQUAL_UINT8(0xD1, responseFrame.value().data[5]);
}

void test_loc_discovery_rejects_invalid_length_and_direction() {
    const CanId id{0, CanCommand::TrainDiscovery, false, 0x0300};
    const CanFrame invalidLength{.identifier = id.encode(), .length = 2};
    const CanMessage invalidResponse{.id = id, .payload = LocDiscoveryResponse{0xFFFA8C43, 32, {}}};

    const auto decoded = CanCodec::decode(invalidLength);
    const auto encoded = CanCodec::encode(invalidResponse);

    TEST_ASSERT_FALSE(decoded);
    TEST_ASSERT_EQUAL(static_cast<int>(ProtocolError::InvalidDataLength),
                      static_cast<int>(decoded.error()));
    TEST_ASSERT_FALSE(encoded);
    TEST_ASSERT_EQUAL(static_cast<int>(ProtocolError::InvalidEnumValue),
                      static_cast<int>(encoded.error()));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_loc_discovery_decodes_request_and_response_forms);
    RUN_TEST(test_loc_discovery_encodes_empty_protocol_and_response_forms);
    RUN_TEST(test_loc_discovery_rejects_invalid_length_and_direction);
    return UNITY_END();
}
