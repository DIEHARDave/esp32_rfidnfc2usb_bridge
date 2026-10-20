#pragma once
#include <Arduino.h>
#include <map>
extern std::map<std::string, std::string> fakeNvs;
class Preferences {
public:
    bool begin(const char *, bool) { return true; }
    void end() {}
    String getString(const char *key, const char *def) {
        auto it = fakeNvs.find(key);
        return it == fakeNvs.end() ? String(def) : String(it->second);
    }
    void putString(const char *key, const String &v) { fakeNvs[key] = v.s_; }
    void clear() { fakeNvs.clear(); }
};
