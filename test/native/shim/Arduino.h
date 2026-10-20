// Minimal Arduino shim for a native build of the bridge's emulator.
#pragma once
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
using std::max;
using std::min;

#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1
#define CONFIG_IDF_TARGET "native"

extern uint32_t fakeMillis;
inline uint32_t millis() { return fakeMillis; }
void delay(uint32_t ms);            // advances the fake clock
inline void delayMicroseconds(uint32_t) {}
inline void pinMode(int, int) {}
inline void digitalWrite(int, int) {}
inline int digitalRead(int) { return 0; }

class String {
public:
    String() {}
    String(const char *s) { if (s) s_ = s; }
    String(const std::string &s) : s_(s) {}
    String(int n) : s_(std::to_string(n)) {}
    String &operator=(const char *s) { s_ = s ? s : ""; return *this; }
    bool concat(const char *s) { s_ += s; return true; }
    const char *c_str() const { return s_.c_str(); }
    size_t length() const { return s_.size(); }
    bool isEmpty() const { return s_.empty(); }
    bool operator==(const char *o) const { return s_ == o; }
    bool operator==(const String &o) const { return s_ == o.s_; }
    String &operator+=(const String &o) { s_ += o.s_; return *this; }
    friend String operator+(const String &a, const String &b) { return String(a.s_ + b.s_); }
    friend String operator+(const String &a, const char *b) { return String(a.s_ + b); }
    friend String operator+(const char *a, const String &b) { return String(a + b.s_); }
    std::string s_;
};

class Stream {
public:
    virtual ~Stream() = default;
    virtual int available() = 0;
    virtual size_t readBytes(uint8_t *buf, size_t len) = 0;
    virtual size_t write(const uint8_t *buf, size_t len) = 0;
    virtual void flush() {}
};

struct EspClass {
    bool restarted = false;
    void restart() { restarted = true; }
    uint64_t getEfuseMac() { return 0x123456789ABCULL; }
};
extern EspClass ESP;
