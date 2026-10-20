# Native tests of the PN532 emulator (Linux or WSL, g++):
#   make test   the emulator against a mock card
#   make sim    the emulator behind a pty (/tmp/bridge-pty) with a mock NTAG215
#               holding an OpenSpool record, for host software to talk to:
#               ./sim & python3 ../../tools/bridgectl.py --port /tmp/bridge-pty scan
# ArduinoJson comes from PlatformIO's library folder: build any env once first.

SRC = ../../src
JSON = $(firstword $(wildcard ../../.pio/libdeps/*/ArduinoJson/src))
CXXFLAGS = -std=gnu++17 -Wall -I shim -I $(SRC) -I $(JSON) -DARDUINO=100 	-DARDUINOJSON_ENABLE_ARDUINO_STREAM=0 -DARDUINOJSON_ENABLE_ARDUINO_PRINT=0 	-DARDUINOJSON_ENABLE_PROGMEM=0
LIB = $(SRC)/emulator.cpp $(SRC)/config.cpp

test: test.cpp $(LIB)
	$(CXX) $(CXXFLAGS) test.cpp $(LIB) -o $@
	./test

sim: sim.cpp test.cpp $(LIB)
	$(CXX) $(CXXFLAGS) sim.cpp $(LIB) -o $@ -lutil

clean:
	rm -f test sim

.PHONY: test clean
