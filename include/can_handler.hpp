#ifndef CAN_RAIL_CAN_HANDLER_HPP
#define CAN_RAIL_CAN_HANDLER_HPP
#include <mcp_can.h>

namespace can_rail {
class CanHandler {
  private:
    const int CAN_CS = 10; // Chip Select pin

    MCP_CAN CAN;

  public:
    CanHandler() : CAN(CAN_CS) {
    }

    void begin() {
        if (CAN.begin(MCP_ANY, CAN_250KBPS, MCP_8MHZ) == CAN_OK) {
            Serial.println("CAN BUS Shield initialized!");
        } else {
            Serial.println("Can bus Shield initialization failed!");
            while (1)
                ;
        }
        CAN.setMode(MCP_NORMAL);
    }
};
} // namespace can_rail

#endif // CAN_RAIL_CAN_HANDLER_HPP
