#ifndef CAN_RAIL_PROTOCOL_CAN_CAN_COMMANDS_HPP
#define CAN_RAIL_PROTOCOL_CAN_CAN_COMMANDS_HPP

#include <cstdint>

namespace can_rail::protocol::can {

enum class CanCommand : std::uint8_t {
    System = 0x00,
    TrainDiscovery = 0x01,
    MfxBind = 0x02,
    MfxVerify = 0x03,
    TrainSpeed = 0x04,
    TrainDirection = 0x05,
    TrainFunction = 0x06,
    ReadConfig = 0x07,
    WriteConfig = 0x08,
    AccessoriesSwitch = 0x0B,
    AccessoriesConfig = 0x0C,
    S88Polling = 0x10,
    S88Event = 0x11,
    Sx1Event = 0x12,
    UserPing = 0x18,
    UpdateOffer = 0x19,
    ReadConfigData = 0x1A,
    BootloaderCan = 0x1B,
    BootloaderTracks = 0x1C,
    StatusDataConfiguration = 0x1D,
    RequestConfigData = 0x20,
    ConfigDataStream = 0x21,
};

enum class SystemSubcommand : std::uint8_t {
    Stop = 0x00,
    Go = 0x01,
    Halt = 0x02,
    TrainEmergencyHalt = 0x03,
    TrainCycleStop = 0x04,
    TrainDataProtocol = 0x05,
    SwitchingTime = 0x06,
    FastReadMfx = 0x07,
    TrackProtocolActivate = 0x08,
    MfxNew = 0x09,
    Overthrottle = 0x0A,
    Status = 0x0B,
    Identifier = 0x0C,
    MfxSeek = 0x30,
    Reset = 0x80,
};

} // namespace can_rail::protocol::can

#endif // CAN_RAIL_PROTOCOL_CAN_CAN_COMMANDS_HPP
