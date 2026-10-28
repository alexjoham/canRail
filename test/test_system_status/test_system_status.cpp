#include <unity.h>

#include "protocol/can/can_codec.hpp"

#include <variant>

using can_rail::protocol::ProtocolError;
using can_rail::protocol::can::CanCodec;
using can_rail::protocol::can::CanCommand;
using can_rail::protocol::can::CanFrame;
using can_rail::protocol::can::CanId;
using can_rail::protocol::can::CanMessage;
using can_rail::protocol::can::SystemStatus;

void test_system_status_decodes_query_and_response() {
    const CanId queryId{0, CanCommand::System, false, 0x0300};
    const CanId responseId{0, CanCommand::System, true, 0x0300};
    const CanFrame queryFrame{
        .identifier = queryId.encode(),
        .length = 6,
        .data = {0x43, 0x53, 0x32, 0x08, 0x0B, 0x01, 0x00, 0x00},
    };
    const CanFrame responseFrame{
        .identifier = responseId.encode(),
        .length = 8,
        .data = {0x43, 0x53, 0x32, 0x08, 0x0B, 0x01, 0x00, 0x03},
    };

    const auto query = CanCodec::decode(queryFrame);
    const auto response = CanCodec::decode(responseFrame);

    TEST_ASSERT_TRUE(query);
    TEST_ASSERT_TRUE(response);
    const auto &queryStatus = std::get<SystemStatus>(query.value().payload);
    const auto &responseStatus = std::get<SystemStatus>(response.value().payload);
    TEST_ASSERT_EQUAL_UINT32(0x43533208, queryStatus.deviceUid);
    TEST_ASSERT_EQUAL_UINT8(1, queryStatus.channel);
    TEST_ASSERT_FALSE(queryStatus.measurement.has_value());
    TEST_ASSERT_EQUAL_UINT16(3, responseStatus.measurement.value());
}

void test_system_status_encodes_query_and_response() {
    const CanId id{0, CanCommand::System, false, 0x0300};
    const CanMessage queryMessage{
        .id = id,
        .payload = SystemStatus{0x43533208, 1, {}},
    };
    const CanMessage responseMessage{
        .id = id,
        .payload = SystemStatus{0x43533208, 1, 3},
    };

    const auto query = CanCodec::encode(queryMessage);
    const auto response = CanCodec::encode(responseMessage);

    TEST_ASSERT_TRUE(query);
    TEST_ASSERT_TRUE(response);
    TEST_ASSERT_EQUAL_UINT8(6, query.value().length);
    TEST_ASSERT_EQUAL_UINT8(0x0B, query.value().data[4]);
    TEST_ASSERT_EQUAL_UINT8(1, query.value().data[5]);
    TEST_ASSERT_EQUAL_UINT8(8, response.value().length);
    TEST_ASSERT_EQUAL_UINT8(0x0B, response.value().data[4]);
    TEST_ASSERT_EQUAL_UINT8(1, response.value().data[5]);
    TEST_ASSERT_EQUAL_UINT8(0x00, response.value().data[6]);
    TEST_ASSERT_EQUAL_UINT8(0x03, response.value().data[7]);
}

void test_system_status_rejects_wrong_length() {
    const CanId id{0, CanCommand::System, false, 0x0300};
    const CanFrame frame{
        .identifier = id.encode(),
        .length = 7,
        .data = {0x43, 0x53, 0x32, 0x08, 0x0B, 0x01, 0x00, 0x00},
    };

    const auto decoded = CanCodec::decode(frame);

    TEST_ASSERT_FALSE(decoded);
    TEST_ASSERT_EQUAL(static_cast<int>(ProtocolError::InvalidDataLength),
                      static_cast<int>(decoded.error()));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_system_status_decodes_query_and_response);
    RUN_TEST(test_system_status_encodes_query_and_response);
    RUN_TEST(test_system_status_rejects_wrong_length);
    return UNITY_END();
}
