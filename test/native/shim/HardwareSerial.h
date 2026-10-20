#pragma once
#include <Arduino.h>
#define SERIAL_8N1 0
class HardwareSerial {
public:
    void setRxBufferSize(size_t) {}
    void begin(uint32_t, int, int, int) {}
    void end() {}
    int available() { return 0; }
    int read() { return -1; }
    size_t write(const uint8_t *, size_t n) { return n; }
};
extern HardwareSerial Serial1;
