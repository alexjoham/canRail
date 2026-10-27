#include <unity.h>

#include "protocol/can/can_codec.hpp"

#include <variant>

using can_rail::protocol::Direction;
using can_rail::protocol::ProtocolError;
using can_rail::protocol::can::CanCodec;
using can_rail::protocol::can::CanCommand;
using can_rail::protocol::can::CanFrame;
using can_rail::protocol::can::CanId;
using can_rail::protocol::can::CanMessage;
using can_rail::protocol::can::TrainDirection;
using can_rail::protocol::can::TrainFunction;

void test_train_direction_decodes_set_frame() {
    const CanId id{0, CanCommand::TrainDirection, false, 0x0300};
    const CanFrame frame{
        .identifier = id.encode(),
        .length = 5,
        .data = {0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00},
    };

    const auto decoded = CanCodec::decode(frame);

    TEST_ASSERT_TRUE(decoded);
    const auto &direction = std::get<TrainDirection>(decoded.value().payload);
    TEST_ASSERT_EQUAL_UINT32(1, direction.locomotiveUid);
    TEST_ASSERT_TRUE(direction.direction.has_value());
    TEST_ASSERT_EQUAL_UINT8(static_cast<std::uint8_t>(Direction::Backward),
                            static_cast<std::uint8_t>(*direction.direction));
}

void test_train_direction_encodes_set_frame() {
    const CanMessage message{
        .id = {0, CanCommand::TrainDirection, true, 0x0300},
        .payload = TrainDirection{1, Direction::Forward},
    };

    const auto encoded = CanCodec::encode(message);

    TEST_ASSERT_TRUE(encoded);
    TEST_ASSERT_EQUAL_UINT8(5, encoded.value().length);
    TEST_ASSERT_EQUAL_UINT8(0x00, encoded.value().data[0]);
    TEST_ASSERT_EQUAL_UINT8(0x00, encoded.value().data[1]);
    TEST_ASSERT_EQUAL_UINT8(0x00, encoded.value().data[2]);
    TEST_ASSERT_EQUAL_UINT8(0x01, encoded.value().data[3]);
    TEST_ASSERT_EQUAL_UINT8(0x01, encoded.value().data[4]);
}

void test_train_direction_decodes_query_frame() {
    const CanId id{0, CanCommand::TrainDirection, false, 0x0300};
    const CanFrame frame{
        .identifier = id.encode(),
        .length = 4,
        .data = {0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00},
    };

    const auto decoded = CanCodec::decode(frame);

    TEST_ASSERT_TRUE(decoded);
    TEST_ASSERT_FALSE(std::get<TrainDirection>(decoded.value().payload).direction.has_value());
}

void test_train_direction_encodes_query_frame() {
    const CanMessage message{
        .id = {0, CanCommand::TrainDirection, false, 0x0300},
        .payload = TrainDirection{1, {}},
    };

    const auto encoded = CanCodec::encode(message);

    TEST_ASSERT_TRUE(encoded);
    TEST_ASSERT_EQUAL_UINT8(4, encoded.value().length);
}

void test_train_direction_rejects_invalid_value() {
    const CanId id{0, CanCommand::TrainDirection, false, 0x0300};
    const CanFrame frame{
        .identifier = id.encode(),
        .length = 5,
        .data = {0x00, 0x00, 0x00, 0x01, 0x04, 0x00, 0x00, 0x00},
    };

    const auto decoded = CanCodec::decode(frame);

    TEST_ASSERT_FALSE(decoded);
    TEST_ASSERT_EQUAL(static_cast<int>(ProtocolError::InvalidEnumValue),
                      static_cast<int>(decoded.error()));
}

void test_train_function_decodes_set_frame() {
    const CanId id{0, CanCommand::TrainFunction, false, 0x0300};
    const CanFrame frame{
        .identifier = id.encode(),
        .length = 6,
        .data = {0x00, 0x00, 0x00, 0x01, 0x1F, 0x1F, 0x00, 0x00},
    };

    const auto decoded = CanCodec::decode(frame);

    TEST_ASSERT_TRUE(decoded);
    const auto &function = std::get<TrainFunction>(decoded.value().payload);
    TEST_ASSERT_EQUAL_UINT32(1, function.locomotiveUid);
    TEST_ASSERT_EQUAL_UINT8(31, function.functionNumber);
    TEST_ASSERT_TRUE(function.value.has_value());
    TEST_ASSERT_EQUAL_UINT8(31, *function.value);
}

void test_train_function_encodes_set_frame() {
    const CanMessage message{
        .id = {0, CanCommand::TrainFunction, true, 0x0300},
        .payload = TrainFunction{1, 3, 1},
    };

    const auto encoded = CanCodec::encode(message);

    TEST_ASSERT_TRUE(encoded);
    TEST_ASSERT_EQUAL_UINT8(6, encoded.value().length);
    TEST_ASSERT_EQUAL_UINT8(0x00, encoded.value().data[0]);
    TEST_ASSERT_EQUAL_UINT8(0x00, encoded.value().data[1]);
    TEST_ASSERT_EQUAL_UINT8(0x00, encoded.value().data[2]);
    TEST_ASSERT_EQUAL_UINT8(0x01, encoded.value().data[3]);
    TEST_ASSERT_EQUAL_UINT8(0x03, encoded.value().data[4]);
    TEST_ASSERT_EQUAL_UINT8(0x01, encoded.value().data[5]);
}

void test_train_function_decodes_query_frame() {
    const CanId id{0, CanCommand::TrainFunction, false, 0x0300};
    const CanFrame frame{
        .identifier = id.encode(),
        .length = 5,
        .data = {0x00, 0x00, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00},
    };

    const auto decoded = CanCodec::decode(frame);

    TEST_ASSERT_TRUE(decoded);
    TEST_ASSERT_FALSE(std::get<TrainFunction>(decoded.value().payload).value.has_value());
}

void test_train_function_encodes_query_frame() {
    const CanMessage message{
        .id = {0, CanCommand::TrainFunction, false, 0x0300},
        .payload = TrainFunction{1, 3, {}},
    };

    const auto encoded = CanCodec::encode(message);

    TEST_ASSERT_TRUE(encoded);
    TEST_ASSERT_EQUAL_UINT8(5, encoded.value().length);
}

void test_train_function_rejects_out_of_range_value() {
    const CanId id{0, CanCommand::TrainFunction, false, 0x0300};
    const CanFrame frame{
        .identifier = id.encode(),
        .length = 6,
        .data = {0x00, 0x00, 0x00, 0x01, 0x20, 0x00, 0x00, 0x00},
    };

    const auto decoded = CanCodec::decode(frame);

    TEST_ASSERT_FALSE(decoded);
    TEST_ASSERT_EQUAL(static_cast<int>(ProtocolError::InvalidEnumValue),
                      static_cast<int>(decoded.error()));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_train_direction_decodes_set_frame);
    RUN_TEST(test_train_direction_encodes_set_frame);
    RUN_TEST(test_train_direction_decodes_query_frame);
    RUN_TEST(test_train_direction_encodes_query_frame);
    RUN_TEST(test_train_direction_rejects_invalid_value);
    RUN_TEST(test_train_function_decodes_set_frame);
    RUN_TEST(test_train_function_encodes_set_frame);
    RUN_TEST(test_train_function_decodes_query_frame);
    RUN_TEST(test_train_function_encodes_query_frame);
    RUN_TEST(test_train_function_rejects_out_of_range_value);
    return UNITY_END();
}
