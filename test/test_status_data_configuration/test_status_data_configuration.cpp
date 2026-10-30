#include <unity.h>

#include "protocol/can/can_codec.hpp"

#include <variant>

using can_rail::protocol::ProtocolError;
using can_rail::protocol::can::CanCodec;
using can_rail::protocol::can::CanCommand;
using can_rail::protocol::can::CanFrame;
using can_rail::protocol::can::CanId;
using can_rail::protocol::can::CanMessage;
using can_rail::protocol::can::StatusDataQuery;
using can_rail::protocol::can::StatusDataResponse;
using can_rail::protocol::can::StatusDataStream;

void test_status_data_decodes_query_stream_and_response() {
    const CanId queryId{0, CanCommand::StatusDataConfiguration, false, 0x0300};
    const CanId streamId{0, CanCommand::StatusDataConfiguration, true, 7};
    const CanFrame queryFrame{
        .identifier = queryId.encode(),
        .length = 5,
        .data = {0x43, 0x53, 0x32, 0x08, 0x00, 0x00, 0x00, 0x00},
    };
    const CanFrame streamFrame{
        .identifier = streamId.encode(),
        .length = 8,
        .data = {0, 1, 2, 3, 4, 5, 6, 7},
    };
    const CanFrame responseFrame{
        .identifier = queryId.encode(),
        .length = 6,
        .data = {0x43, 0x53, 0x32, 0x08, 0x00, 0x03, 0x00, 0x00},
    };

    const auto query = CanCodec::decode(queryFrame);
    const auto stream = CanCodec::decode(streamFrame);
    const auto response = CanCodec::decode(responseFrame);

    TEST_ASSERT_TRUE(query);
    TEST_ASSERT_TRUE(stream);
    TEST_ASSERT_TRUE(response);
    TEST_ASSERT_EQUAL_UINT8(0, std::get<StatusDataQuery>(query.value().payload).index);
    TEST_ASSERT_EQUAL_UINT8(7, std::get<StatusDataStream>(stream.value().payload).data[7]);
    TEST_ASSERT_EQUAL_UINT8(3, std::get<StatusDataResponse>(response.value().payload).packetCount);
}

void test_status_data_encodes_all_frame_forms() {
    const CanId id{0, CanCommand::StatusDataConfiguration, false, 0x0300};
    const CanMessage query{.id = id, .payload = StatusDataQuery{0x43533208, 2}};
    const CanMessage stream{.id = id, .payload = StatusDataStream{{1, 2, 3, 4, 5, 6, 7, 8}}};
    const CanMessage response{.id = id, .payload = StatusDataResponse{0x43533208, 2, 4}};

    const auto queryFrame = CanCodec::encode(query);
    const auto streamFrame = CanCodec::encode(stream);
    const auto responseFrame = CanCodec::encode(response);

    TEST_ASSERT_TRUE(queryFrame);
    TEST_ASSERT_TRUE(streamFrame);
    TEST_ASSERT_TRUE(responseFrame);
    TEST_ASSERT_EQUAL_UINT8(5, queryFrame.value().length);
    TEST_ASSERT_EQUAL_UINT8(8, streamFrame.value().length);
    TEST_ASSERT_EQUAL_UINT8(6, responseFrame.value().length);
    TEST_ASSERT_EQUAL_UINT8(8, streamFrame.value().data[7]);
    TEST_ASSERT_EQUAL_UINT8(4, responseFrame.value().data[5]);
}

void test_status_data_rejects_invalid_length() {
    const CanId id{0, CanCommand::StatusDataConfiguration, false, 0x0300};
    const CanFrame frame{.identifier = id.encode(), .length = 7};

    const auto decoded = CanCodec::decode(frame);

    TEST_ASSERT_FALSE(decoded);
    TEST_ASSERT_EQUAL(static_cast<int>(ProtocolError::InvalidDataLength),
                      static_cast<int>(decoded.error()));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_status_data_decodes_query_stream_and_response);
    RUN_TEST(test_status_data_encodes_all_frame_forms);
    RUN_TEST(test_status_data_rejects_invalid_length);
    return UNITY_END();
}
