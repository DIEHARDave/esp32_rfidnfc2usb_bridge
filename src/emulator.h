// A PN532 on the host's serial port. The host sends PN532 HSU frames
// (the PN532 User Manual, UM0701-02, section 6.2) and gets the PN532's
// answers, whatever chip the bridge carries:
//
// - PN532: commands are passed through to the chip, except the few the
//   bridge must handle itself (polling retries, baud rate, power down).
// - RC522 / PN5180: the ISO 14443A initiator commands are emulated on top
//   of the chip (InListPassiveTarget, InDataExchange with MIFARE Classic
//   authentication and writes, InCommunicateThru, InAutoPoll, ...).
//
// Commands 0xF0-0xFA are the bridge's own (docs/protocol.md): bridge info,
// ISO 15693 on the PN5180, and settings. A real PN532 answers them with a
// syntax error, which tells the two apart.
#pragma once

#include <Arduino.h>
#include <functional>

#include "backend.h"

class Emulator {
public:
    // setBaud changes the host link's baud rate (UART hosts); nullptr when
    // the link is USB CDC, where the baud rate means nothing.
    Emulator(Stream &host, Backend &backend, Config &config, std::function<void(uint32_t)> setBaud)
        : host_(host), backend_(backend), config_(config), setBaud_(setBaud) {}

    // Call often: reads the host's bytes and answers complete frames.
    void poll();
    // No frame half received, so a slow chip probe will not stall one.
    bool idle() const { return rxLen_ == 0; }
    // The host has a card selected (for the status LED).
    bool cardSelected() const { return selected_; }

private:
    enum class Frame { None, Ack, Nack, Data };

    Frame nextFrame(uint8_t *&body, size_t &len);
    void consume(size_t count);
    void sendAck();
    void sendFrame(const uint8_t *body, size_t len);
    void sendSyntaxError();
    // Any byte from the host while a command runs aborts it, as on the PN532
    // (normally an ACK frame, which is then read and dropped).
    bool aborted() { return host_.available() > 0; }
    bool waitAborted(uint32_t ms);

    // A command's response data (after D5 and the response code) into out:
    // its length, -1 for a syntax error frame, -2 for no answer (aborted).
    int handle(uint8_t cmd, const uint8_t *p, size_t len, uint8_t *out);
    int passThrough(uint8_t cmd, const uint8_t *p, size_t len, uint8_t *out, uint32_t timeoutMs = 1000);
    int listPassiveTarget(const uint8_t *p, size_t len, uint8_t *out);
    int autoPoll(const uint8_t *p, size_t len, uint8_t *out);
    // One activation attempt: the InListPassiveTarget response (NbTg first).
    int listOnce(uint8_t maxTg, uint8_t brTy, const uint8_t *init, size_t initLen, uint8_t *out);
    int dataExchange(const uint8_t *p, size_t len, uint8_t *out);
    int mifareWrite(const uint8_t *data, size_t len);
    int communicateThru(const uint8_t *p, size_t len, uint8_t *out);
    int rfConfiguration(const uint8_t *p, size_t len, uint8_t *out);
    int generalStatus(uint8_t *out);
    int bridgeInfo(uint8_t *out);
    int iso15693Inventory(uint8_t *out);
    int iso15693Exchange(const uint8_t *p, size_t len, uint8_t *out);
    int getConfig(uint8_t *out);
    int setConfig(const uint8_t *p, size_t len, uint8_t *out);
    void releaseTarget();

    uint8_t readRegister(uint16_t address) const;
    void writeRegister(uint16_t address, uint8_t value);

    Stream &host_;
    Backend &backend_;
    Config &config_;
    std::function<void(uint32_t)> setBaud_;

    uint8_t rx_[300];
    size_t rxLen_ = 0;
    uint32_t lastRx_ = 0;
    uint8_t last_[300];         // the last response frame, for a NACK
    size_t lastLen_ = 0;
    uint32_t pendingBaud_ = 0;  // SetSerialBaudRate: switch after the host's ACK
    uint32_t baudAt_ = 0;
    bool restart_ = false;      // restart once the response is sent

    // PN532 state the host can set.
    uint8_t mxRtyPassive_ = 0xFF;   // RFConfiguration item 5; 0xFF: forever
    uint8_t parameters_ = 0x14;     // SetParameters: fAutomaticRATS, fAutomaticATR_RES
    struct Reg {
        uint16_t address;
        uint8_t value;
    };
    Reg regs_[48];
    size_t regCount_ = 0;

    // The emulated target (RC522 / PN5180).
    bool selected_ = false;     // after InListPassiveTarget, until released
    bool known_ = false;        // InDeselect keeps it for InSelect
    bool vicinity_ = false;     // an ISO 15693 card was found last
    NfcTarget target_;
};
