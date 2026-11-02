#ifndef CAN_RAIL_PROTOCOL_CAN_CAN_MESSAGES_HPP
#define CAN_RAIL_PROTOCOL_CAN_CAN_MESSAGES_HPP

#include "../common/types.hpp"
#include "can_commands.hpp"

#include <array>
#include <cstdint>
#include <optional>

namespace can_rail::protocol::can {

struct TrainSpeed {
    std::uint32_t locomotiveUid;
    std::optional<std::uint16_t> speed;
};

struct TrainDirection {
    std::uint32_t locomotiveUid;
    std::optional<Direction> direction;
};

struct TrainFunction {
    std::uint32_t locomotiveUid;
    std::uint8_t functionNumber;
    std::optional<std::uint8_t> value;
};

struct SystemStop {
    std::uint32_t deviceUid;
};

struct SystemGo {
    std::uint32_t deviceUid;
};

struct SystemHalt {
    std::uint32_t deviceUid;
};

struct SystemTrainEmergencyHalt {
    std::uint32_t locomotiveUid;
};

struct SystemTrainCycleStop {
    std::uint32_t locomotiveUid;
};

struct SystemTrainDataProtocol {
    std::uint32_t locomotiveUid;
    std::uint8_t protocol;
};

struct SystemSwitchingTime {
    std::uint32_t deviceUid;
    std::uint16_t time;
};

struct SystemTrackProtocolActivate {
    std::uint32_t deviceUid;
    std::uint8_t protocolMask;
};

struct SystemMfxNewRegistrationCounter {
    std::uint32_t deviceUid;
    std::uint16_t counter;
};

struct SystemOverload {
    std::uint32_t deviceUid;
    std::uint8_t channel;
};

struct SystemStatus {
    std::uint32_t deviceUid;
    std::uint8_t channel;
    std::optional<std::uint16_t> measurement;
};

struct UserPing {
    std::optional<std::uint32_t> deviceUid;
    std::array<std::uint8_t, 4> versionDb{};
    bool hasVersionDb{false};
};

struct StatusDataQuery {
    std::uint32_t deviceUid;
    std::uint8_t index;
};

struct StatusDataStream {
    std::array<std::uint8_t, 8> data{};
};

struct StatusDataResponse {
    std::uint32_t deviceUid;
    std::uint8_t index;
    std::uint8_t packetCount;
};

struct ReadConfigRequest {
    std::uint32_t locomotiveUid;
    std::uint8_t cvIndex;
    std::uint16_t cvNumber;
    std::uint8_t count;
};

struct ReadConfigResponse {
    std::uint32_t locomotiveUid;
    std::uint8_t cvIndex;
    std::uint16_t cvNumber;
    std::optional<std::uint8_t> value;
};

struct WriteConfigRequest {
    std::uint32_t locomotiveUid;
    std::uint8_t cvIndex;
    std::uint16_t cvNumber;
    std::uint8_t value;
    std::uint8_t control;
};

struct WriteConfigResponse {
    std::uint32_t locomotiveUid;
    std::uint8_t cvIndex;
    std::uint16_t cvNumber;
    std::uint8_t value;
    std::uint8_t result;
};

struct AccessoriesSwitch {
    std::uint32_t accessoryUid;
    std::uint8_t position;
    std::uint8_t power;
    std::optional<std::uint16_t> switchingTime;
};

struct LocDiscoveryRequest {
    std::optional<std::uint8_t> protocol;
    std::optional<std::uint32_t> mfxUid;
    std::optional<std::uint8_t> range;
};

struct LocDiscoveryResponse {
    std::uint32_t mfxUid;
    std::uint8_t range;
    std::optional<std::uint8_t> askRatio;
};

struct MfxBind {
    std::uint32_t mfxUid;
    std::uint16_t mfxSid;
};

struct UnknownCanMessage {
    CanCommand command;
    std::uint8_t length;
    std::array<std::uint8_t, 8> data{};
};

} // namespace can_rail::protocol::can

#endif // CAN_RAIL_PROTOCOL_CAN_CAN_MESSAGES_HPP
