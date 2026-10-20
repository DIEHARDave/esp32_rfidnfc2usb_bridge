#pragma once
#include <Arduino.h>
#define ESP_OK 0
class TwoWire {
public:
    void begin(int, int, uint32_t) {}
    void end() {}
    void setTimeOut(int) {}
    void beginTransmission(uint8_t) {}
    size_t write(const uint8_t *, size_t n) { return n; }
    uint8_t endTransmission() { return 2; }
};
extern TwoWire Wire;
