#include <unity.h>

#include "protocol/can/can_message.hpp"

#include <type_traits>
#include <variant>

using can_rail::protocol::Direction;
using can_rail::protocol::SystemState;
using can_rail::protocol::TrackProtocol;
using can_rail::protocol::can::CanCommand;
using can_rail::protocol::can::CanMessage;
using can_rail::protocol::can::CanPayload;
using can_rail::protocol::can::SystemGo;
using can_rail::protocol::can::SystemHalt;
using can_rail::protocol::can::SystemStop;
using can_rail::protocol::can::TrainDirection;
using can_rail::protocol::can::TrainFunction;
using can_rail::protocol::can::TrainSpeed;
using can_rail::protocol::can::UnknownCanMessage;

void test_protocol_enums_have_expected_values() {
    TEST_ASSERT_EQUAL_UINT8(0, static_cast<std::uint8_t>(Direction::NoChange));
    TEST_ASSERT_EQUAL_UINT8(1, static_cast<std::uint8_t>(Direction::Forward));
    TEST_ASSERT_EQUAL_UINT8(2, static_cast<std::uint8_t>(Direction::Backward));
    TEST_ASSERT_EQUAL_UINT8(3, static_cast<std::uint8_t>(Direction::Change));
    TEST_ASSERT_EQUAL_UINT8(0, static_cast<std::uint8_t>(TrackProtocol::MM2));
    TEST_ASSERT_EQUAL_UINT8(1, static_cast<std::uint8_t>(TrackProtocol::Mfx));
    TEST_ASSERT_EQUAL_UINT8(2, static_cast<std::uint8_t>(TrackProtocol::Dcc));
    TEST_ASSERT_EQUAL_UINT8(0, static_cast<std::uint8_t>(SystemState::Stop));
    TEST_ASSERT_EQUAL_UINT8(1, static_cast<std::uint8_t>(SystemState::Go));
    TEST_ASSERT_EQUAL_UINT8(2, static_cast<std::uint8_t>(SystemState::Halt));
}

void test_each_supported_message_is_a_valid_payload() {
    TEST_ASSERT_TRUE(std::holds_alternative<TrainSpeed>(CanPayload{TrainSpeed{0x12345678, 500}}));
    TEST_ASSERT_TRUE(std::holds_alternative<TrainDirection>(
        CanPayload{TrainDirection{0x12345678, Direction::Forward}}));
    TEST_ASSERT_TRUE(
        std::holds_alternative<TrainFunction>(CanPayload{TrainFunction{0x12345678, 3, 1}}));
    TEST_ASSERT_TRUE(std::holds_alternative<SystemStop>(CanPayload{SystemStop{0x12345678}}));
    TEST_ASSERT_TRUE(std::holds_alternative<SystemGo>(CanPayload{SystemGo{0x12345678}}));
    TEST_ASSERT_TRUE(std::holds_alternative<SystemHalt>(CanPayload{SystemHalt{0x12345678}}));
    TEST_ASSERT_TRUE(std::holds_alternative<UnknownCanMessage>(
        CanPayload{UnknownCanMessage{CanCommand::System, 0, {}}}));
}

void test_can_message_contains_identifier_and_payload() {
    const CanMessage message{
        .id = {7, CanCommand::TrainSpeed, false, 0x1234},
        .payload = TrainSpeed{0x12345678, 500},
    };

    TEST_ASSERT_EQUAL_UINT8(7, message.id.priority);
    TEST_ASSERT_EQUAL_UINT8(static_cast<std::uint8_t>(CanCommand::TrainSpeed),
                            static_cast<std::uint8_t>(message.id.command));
    TEST_ASSERT_FALSE(message.id.response);
    TEST_ASSERT_EQUAL_UINT16(0x1234, message.id.hash);
    TEST_ASSERT_TRUE(std::holds_alternative<TrainSpeed>(message.payload));
    TEST_ASSERT_EQUAL_UINT32(0x12345678, std::get<TrainSpeed>(message.payload).locomotiveUid);
    TEST_ASSERT_TRUE(std::get<TrainSpeed>(message.payload).speed.has_value());
    TEST_ASSERT_EQUAL_UINT16(500, *std::get<TrainSpeed>(message.payload).speed);
}

void test_can_payload_can_be_dispatched_with_visit() {
    const CanPayload payload = TrainDirection{0x12345678, Direction::Backward};
    bool visitedTrainDirection = false;

    std::visit(
        [&visitedTrainDirection](const auto &message) {
            using MessageType = std::decay_t<decltype(message)>;
            visitedTrainDirection = std::is_same_v<MessageType, TrainDirection>;
        },
        payload);

    TEST_ASSERT_TRUE(visitedTrainDirection);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_protocol_enums_have_expected_values);
    RUN_TEST(test_each_supported_message_is_a_valid_payload);
    RUN_TEST(test_can_message_contains_identifier_and_payload);
    RUN_TEST(test_can_payload_can_be_dispatched_with_visit);
    return UNITY_END();
}
