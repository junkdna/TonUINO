# SPDX-License-Identifier: GPL-3.0-only
# Copyright 2020 Alexander Willner
# Copyright 2019-2020 Tillmann Heidsieck <theidsieck@leenox.de>
#
# TonUINO Makefile

# Config
## Arduino Nano (old bootloader)
MCU = arduino:avr:nano:cpu=atmega328old
BOARD = arduino:avr
SERIAL = /dev/ttyUSB0
## Main
SKETCH = .
## Helper
OS = $(shell uname)
.PHONY: help install clean

PIO = $(shell which platformio)
ifeq (, $(PIO))
	PIO = $(HOME)/.local/bin/platformio
endif

ACLI = $(shell which arduino-cli)
ifeq (, $(ACLI))
	ACLI = $(shell pwd)/bin/arduino-cli
endif

help:
	$(info TonUINO Makefile)
	$(info ================)
	$(info )
	$(info Configured to use MCU "$(MCU)" attached to port "$(SERIAL)".)
	$(info )
	$(info Available commands:)
	$(info - install : installation of required binaries (arduino-cli/platformio))
	$(info - init    : initialize environment (arduino-cli/platformio))
	$(info - compile : create binary)
	$(info - upload  : store binary on board flash)
	$(info - find    : get information about the pluged-in board)
	$(info - test    : run some basic tests on the code)
	$(info - check   : run some static checks on the code)
	$(info - tags    : generate ctags and cscope files)
	$(info - clean   : delete temporary files)
	@true

install:
ifeq ($(OS),Darwin)
ifeq (, $(shell which brew))
	$(error "No brew in PATH, consider installing http://brew.sh")
else
	@brew install platformio arduino-cli
endif
endif

ifeq ($(OS),Linux)
ifeq (,$(shell find $(ACLI)))
	@curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
endif
ifeq (,$(shell find $(PIO)))
	@pip3 install -U --user setuptools wheel
	@pip3 install -U --user platformio
endif
endif

init:
	@$(ACLI) config init
	@$(ACLI) core update-index
	@$(ACLI) core install $(BOARD)
	@$(ACLI) lib install "DFPlayer Mini Mp3 by Makuna"
	@$(ACLI) lib install "MFRC522"
	@$(ACLI) lib install "JC_Button"
	@$(PIO) lib install 1561 # DFPlayer Mini Mp3 by Makuna
	@$(PIO) lib install 2284 # EEPROM
	@$(PIO) lib install 77   # JC_Button
	@$(PIO) lib install 63   # MFRC522

compile: *.ino *.h
	@$(ACLI) compile --fqbn $(MCU) --warnings none "$(SKETCH)"

find:
	@$(ACLI) board list

upload: compile
	@$(ACLI) upload -p $(SERIAL) --fqbn $(MCU) --verify "$(SKETCH)"

test:
	@$(ACLI) compile --fqbn $(MCU) --warnings more "$(SKETCH)"

tags: *.ino *.h
	@../Tools/gen_tags.sh

check: *.ino *.h
	@cppcheck --enable=all --std=c++20 --language=c++ *.ino *.h

clean:
	@rm -rf ..arduino*
	@rm -rf ".pio/build/"
	@rm -rf tags cscope*
