#
# TonUINO Makefile
#
# Author: Alexander Willner
# Date: 2020-01-03

# Config
## Arduino Nano (old bootloader)
MCU = arduino:avr:nano:cpu=atmega328old
BOARD = arduino:avr
SERIAL = /dev/ttyUSB0
## Main
SKETCH = .
## Helper
OS = $(shell uname)
.PHONY: help

help:
	$(info TonUINO Makefile)
	$(info ================)
	$(info )
	$(info Configured to use MCU "$(MCU)" attached to port "$(SERIAL)".)
	$(info )
	$(info Available commands:)
	$(info - install : installation of required binaries (arduino-cli))
	$(info - init    : initialize environment (arduino-cli))
	$(info - pio-init: initialize environment (PlatformIO))
	$(info - compile : create binary)
	$(info - upload  : store binary on board flash)
	$(info - find    : get information about the pluged-in board)
	$(info - test    : run some basic tests on the code)
	$(info - clean   : delete temporary files)
	@true

install:
ifeq ($(OS),Darwin)
ifeq (, $(shell which brew))
	$(error "No brew in PATH, consider installing http://brew.sh")
else
	@brew install arduino-cli
endif
else
	@echo "todo: auto setup for OS"
endif

init:
	@arduino-cli config init
	@arduino-cli core update-index
	@arduino-cli core install $(BOARD)
	@arduino-cli lib install "DFPlayer Mini Mp3 by Makuna"
	@arduino-cli lib install "MFRC522"
	@arduino-cli lib install "JC_Button"

pio-init:
	@platformio lib install 1561 # DFPlayer Mini Mp3 by Makuna
	@platformio lib install 2284 # EEPROM
	@platformio lib install 77   # JC_Button
	@platformio lib install 63   # MFRC522

compile:
	@arduino-cli compile --fqbn $(MCU) --warnings none "$(SKETCH)"

find:
	@arduino-cli board list

upload:
	@arduino-cli upload -p $(SERIAL) --fqbn $(MCU) --verify "$(SKETCH)"

test:
	@arduino-cli compile --fqbn $(MCU) --warnings more "$(SKETCH)"

tags: *.ino *.h
	@../Tools/gen_tags.sh

check: *.ino *.h
	@cppcheck --enable=all --std=c++20 --language=c++ *.ino *.h

clean:
	@rm -rf ..arduino*
	@rm -rf ".pio/build/"
