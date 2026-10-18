#include "can_handler.hpp"
#include <Arduino.h>
#include <HardwareSerial.h>

can_rail::CanHandler canHandler;

void setup() {
    Serial.begin(115200);
    delay(1000);

    canHandler.begin();
}

void loop() {
}
