#include <unity.h>

#include "protocol/can/can_codec.hpp"

#include <array>
#include <variant>

using can_rail::protocol::ProtocolError;
using can_rail::protocol::can::CanCodec;
using can_rail::protocol::can::CanCommand;
using can_rail::protocol::can::CanFrame;
using can_rail::protocol::can::CanId;
using can_rail::protocol::can::CanMessage;
using can_rail::protocol::can::UserPing;

void test_user_ping_decodes_query_and_response() {
    const CanId queryId{0, CanCommand::UserPing, false, 0x0300};
    const CanId responseId{0, CanCommand::UserPing, true, 0x0300};
    const CanFrame queryFrame{
        .identifier = queryId.encode(),
        .length = 5,
        .data = {0x43, 0x53, 0x32, 0x08, 0x00, 0x00, 0x00, 0x00},
    };
    const CanFrame responseFrame{
        .identifier = responseId.encode(),
        .length = 8,
        .data = {0x43, 0x53, 0x32, 0x08, 0x01, 0x02, 0x03, 0x04},
    };

    const auto query = CanCodec::decode(queryFrame);
    const auto response = CanCodec::decode(responseFrame);

    TEST_ASSERT_TRUE(query);
    TEST_ASSERT_TRUE(response);
    TEST_ASSERT_TRUE(std::holds_alternative<UserPing>(query.value().payload));
    TEST_ASSERT_TRUE(std::holds_alternative<UserPing>(response.value().payload));
    TEST_ASSERT_EQUAL_UINT32(0x43533208, std::get<UserPing>(query.value().payload).deviceUid.value());
    TEST_ASSERT_FALSE(std::get<UserPing>(query.value().payload).hasVersionDb);
    TEST_ASSERT_EQUAL_UINT8(1, std::get<UserPing>(response.value().payload).versionDb[0]);
    TEST_ASSERT_EQUAL_UINT8(2, std::get<UserPing>(response.value().payload).versionDb[1]);
    TEST_ASSERT_EQUAL_UINT8(3, std::get<UserPing>(response.value().payload).versionDb[2]);
    TEST_ASSERT_EQUAL_UINT8(4, std::get<UserPing>(response.value().payload).versionDb[3]);
    TEST_ASSERT_TRUE(std::get<UserPing>(response.value().payload).hasVersionDb);
}

void test_user_ping_encodes_query_and_response() {
    const CanId id{0, CanCommand::UserPing, false, 0x0300};
    const CanMessage queryMessage{
        .id = id,
        .payload = UserPing{0x43533208, {}, false},
    };
    const CanMessage responseMessage{
        .id = id,
        .payload = UserPing{0x43533208, {1, 2, 3, 4}, true},
    };

    const auto query = CanCodec::encode(queryMessage);
    const auto response = CanCodec::encode(responseMessage);

    TEST_ASSERT_TRUE(query);
    TEST_ASSERT_TRUE(response);
    TEST_ASSERT_EQUAL_UINT8(5, query.value().length);
    TEST_ASSERT_EQUAL_UINT8(0x43, query.value().data[0]);
    TEST_ASSERT_EQUAL_UINT8(0x53, query.value().data[1]);
    TEST_ASSERT_EQUAL_UINT8(0x32, query.value().data[2]);
    TEST_ASSERT_EQUAL_UINT8(0x08, query.value().data[3]);
    TEST_ASSERT_EQUAL_UINT8(8, response.value().length);
    TEST_ASSERT_EQUAL_UINT8(0x01, response.value().data[4]);
    TEST_ASSERT_EQUAL_UINT8(0x02, response.value().data[5]);
    TEST_ASSERT_EQUAL_UINT8(0x03, response.value().data[6]);
    TEST_ASSERT_EQUAL_UINT8(0x04, response.value().data[7]);
}

void test_user_ping_rejects_wrong_length() {
    const CanId id{0, CanCommand::UserPing, false, 0x0300};
    const CanFrame frame{
        .identifier = id.encode(),
        .length = 6,
        .data = {0x43, 0x53, 0x32, 0x08, 0x00, 0x00, 0x00, 0x00},
    };

    const auto decoded = CanCodec::decode(frame);

    TEST_ASSERT_FALSE(decoded);
    TEST_ASSERT_EQUAL(static_cast<int>(ProtocolError::InvalidDataLength),
                      static_cast<int>(decoded.error()));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_user_ping_decodes_query_and_response);
    RUN_TEST(test_user_ping_encodes_query_and_response);
    RUN_TEST(test_user_ping_rejects_wrong_length);
    return UNITY_END();
}
