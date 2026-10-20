// ISO 14443-4 (ISO-DEP) block protocol for chips that only send and
// receive frames (RC522, PN5180); the PN532 does this itself.
#pragma once

#include <Arduino.h>

class FrameLink {
public:
    virtual ~FrameLink() = default;
    // Send a frame with CRC_A added; the card's answer with its CRC checked
    // and removed, or -1.
    virtual int transceiveFrame(const uint8_t *data, size_t len, uint8_t *out, size_t max,
                                uint32_t timeoutMs) = 0;
};

class IsoDep {
public:
    explicit IsoDep(FrameLink &link) : link_(link) {}
    // RATS to a selected card.
    bool activate();
    // I-blocks without CID or NAD. APDUs longer than the card's frame size
    // are sent chained; chained answers are acknowledged and joined.
    int exchange(const uint8_t *apdu, size_t len, uint8_t *out, size_t expect);
    // S(DESELECT): the card halts, a WUPA wakes it.
    void deselect();
    bool active() const { return active_; }
    void reset() { active_ = false; }
    // The ATS from the last activation (length byte TL first).
    const uint8_t *ats() const { return ats_; }
    size_t atsLen() const { return active_ ? atsLen_ : 0; }

private:
    FrameLink &link_;
    bool active_ = false;
    uint8_t blockNumber_ = 0;
    size_t fsc_ = 32;           // the card's frame size, with PCB and CRC
    uint32_t fwtMs_ = 5;        // the card's frame waiting time
    uint8_t ats_[32] = {};
    size_t atsLen_ = 0;
};
