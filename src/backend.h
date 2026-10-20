// The reader chip behind the bridge: the configured one, or the first that
// answers when the setting is "auto". Re-initialised when it stops answering.
#pragma once

#include "config.h"
#include "pn5180.h"
#include "pn532.h"
#include "rc522.h"

class Backend {
public:
    void begin(const Config &config);
    // Call when the host is idle: finds a missing chip again (at most every
    // few seconds) and re-initialises one that keeps failing.
    void service();

    // The chip in use, or nullptr when none answers.
    NfcChip *chip() { return chip_ && chip_->ready() ? chip_ : nullptr; }
    // The chip when it is a PN532, which host commands are passed through to.
    Pn532 *pn532() { return chip_ == &pn532_ && pn532_.ready() ? &pn532_ : nullptr; }
    String describe() const;

private:
    bool tryChip(ChipSetting chip, bool probing);
    bool detect();

    Config config_;
    Pn532 pn532_;
    Rc522 rc522_;
    Pn5180 pn5180_;
    NfcChip *chip_ = nullptr;
    ChipSetting found_ = ChipSetting::Auto;   // the chip in use (for auto)
    uint32_t nextTry_ = 0;
};
