#include <unity.h>

#include "protocol/can/can_codec.hpp"

#include <variant>

using can_rail::protocol::ProtocolError;
using can_rail::protocol::can::CanCodec;
using can_rail::protocol::can::CanCommand;
using can_rail::protocol::can::CanFrame;
using can_rail::protocol::can::CanId;
using can_rail::protocol::can::CanMessage;
using can_rail::protocol::can::TrainSpeed;

void test_train_speed_decodes_protocol_frame() {
    const CanId id{0, CanCommand::TrainSpeed, false, 0x0300};
    CanFrame frame{
        .identifier = id.encode(),
        .length = 6,
        .data = {0x00, 0x00, 0x00, 0x01, 0x03, 0x20, 0x00, 0x00},
    };

    const auto decoded = CanCodec::decode(frame);

    TEST_ASSERT_TRUE(decoded);
    TEST_ASSERT_EQUAL_UINT32(id.encode(), decoded.value().id.encode());
    TEST_ASSERT_TRUE(std::holds_alternative<TrainSpeed>(decoded.value().payload));
    const auto &speed = std::get<TrainSpeed>(decoded.value().payload);
    TEST_ASSERT_EQUAL_UINT32(1, speed.locomotiveUid);
    TEST_ASSERT_TRUE(speed.speed.has_value());
    TEST_ASSERT_EQUAL_UINT16(0x0320, *speed.speed);
}

void test_train_speed_decodes_query_length() {
    const CanId id{0, CanCommand::TrainSpeed, false, 0x0300};
    CanFrame frame{.identifier = id.encode(), .length = 4};

    const auto decoded = CanCodec::decode(frame);

    TEST_ASSERT_TRUE(decoded);
    const auto &speed = std::get<TrainSpeed>(decoded.value().payload);
    TEST_ASSERT_FALSE(speed.speed.has_value());
}

void test_train_speed_rejects_unsupported_command() {
    const CanId id{0, CanCommand::System, false, 0x0300};
    CanFrame frame{.identifier = id.encode(), .length = 0};

    const auto decoded = CanCodec::decode(frame);

    TEST_ASSERT_FALSE(decoded);
    TEST_ASSERT_EQUAL(static_cast<int>(ProtocolError::UnsupportedCommand),
                      static_cast<int>(decoded.error()));
}

void test_train_speed_encodes_protocol_frame() {
    const CanMessage message{
        .id = {0, CanCommand::TrainSpeed, true, 0x0300},
        .payload = TrainSpeed{1, 0x0320},
    };

    const auto encoded = CanCodec::encode(message);

    TEST_ASSERT_TRUE(encoded);
    TEST_ASSERT_EQUAL_UINT32(message.id.encode(), encoded.value().identifier);
    TEST_ASSERT_EQUAL_UINT8(6, encoded.value().length);
    TEST_ASSERT_EQUAL_UINT8(0x00, encoded.value().data[0]);
    TEST_ASSERT_EQUAL_UINT8(0x00, encoded.value().data[1]);
    TEST_ASSERT_EQUAL_UINT8(0x00, encoded.value().data[2]);
    TEST_ASSERT_EQUAL_UINT8(0x01, encoded.value().data[3]);
    TEST_ASSERT_EQUAL_UINT8(0x03, encoded.value().data[4]);
    TEST_ASSERT_EQUAL_UINT8(0x20, encoded.value().data[5]);
}

void test_train_speed_encodes_query_frame() {
    const CanMessage message{
        .id = {0, CanCommand::TrainSpeed, false, 0x0300},
        .payload = TrainSpeed{1, {}},
    };

    const auto encoded = CanCodec::encode(message);

    TEST_ASSERT_TRUE(encoded);
    TEST_ASSERT_EQUAL_UINT8(4, encoded.value().length);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_train_speed_decodes_protocol_frame);
    RUN_TEST(test_train_speed_decodes_query_length);
    RUN_TEST(test_train_speed_rejects_unsupported_command);
    RUN_TEST(test_train_speed_encodes_protocol_frame);
    RUN_TEST(test_train_speed_encodes_query_frame);
    return UNITY_END();
}
