/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2019 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include <Arduino.h>
#include <EEPROM.h>

#include "eeprom_config.h"

void EEPROM_Config::write() {
    uint8_t buf[EEPROM_CFG_LEN] = {0x0};
    uint8_t i;

    buf[ 0] = (magic >>  0) & 0xff;
    buf[ 1] = (magic >>  8) & 0xff;
    buf[ 2] = (magic >> 16) & 0xff;
    buf[ 3] = (magic >> 24) & 0xff;
    buf[ 4] = version;
    buf[ 5] = max_volume;
    buf[ 6] = min_volume;
    buf[ 7] = step_volume;
    buf[ 8] = init_volume;

    for (i = 0; i < EEPROM_CFG_SIZE; i++) {
        EEPROM.write(i, buf[i]);
        Serial.println(buf[i]);
    }
}

void EEPROM_Config::read() {
    uint8_t buf[EEPROM_CFG_LEN] = {0x0};
    uint8_t i;

    for (i = 0; i < EEPROM_CFG_SIZE; i++) {
        buf[i] = EEPROM.read(i);
        Serial.println(buf[i]);
    }

    magic = ((uint32_t)buf[3] << 24) | ((uint32_t)buf[2] << 16) | \
            ((uint32_t)buf[1] << 8) | buf[0];
    version = buf[4];

    max_volume = buf[5];
    min_volume = buf[6];
    step_volume = buf[7];
    init_volume = buf[8];
}

void EEPROM_Config::init() {
    magic = EEPROM_MAGIC;
    version = EEPROM_VERSION;

    max_volume = MAX_VOLUME;
    min_volume = MIN_VOLUME;
    step_volume = STEP_VOLUME;
    init_volume = INITIAL_VOLUME;
}

bool EEPROM_Config::check() {
    if (magic != EEPROM_MAGIC)
        return false;

    if (version != EEPROM_VERSION)
        return false;

    return true;
}
