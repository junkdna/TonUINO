/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2019 Tillmann Heidsieck <theidsieck@leenox.de>
 */
#include <EEPROM.h>

#include "eeprom_config.h"

void EEPROM_Config::write() {
    uint8_t buf[EEPROM_CFG_LEN] = {0x0};
    uint8_t i;

    buf[0] = (magic >>  0) & 0xff;
    buf[1] = (magic >>  8) & 0xff;
    buf[2] = (magic >> 16) & 0xff;
    buf[3] = (magic >> 24) & 0xff;
    buf[4] = version;
    buf[5] = (id >>  0) & 0xff;
    buf[6] = (id >>  8) & 0xff;
    buf[7] = (id >> 16) & 0xff;
    buf[8] = (id >> 24) & 0xff;

    for (i = 0; i < EEPROM_CFG_SIZE; i++)
        EEPROM.write(i, buf[i]);
}

void EEPROM_Config::read() {
    uint8_t buf[EEPROM_CFG_LEN] = {0x0};
    uint8_t i;

    for (i = 0; i < EEPROM_CFG_SIZE; i++)
        buf[i] = EEPROM.read(i);

    magic = ((uint32_t)buf[3] << 24) | ((uint32_t)buf[2] << 16) | \
            ((uint32_t)buf[1] << 8) | buf[0];
    version = buf[4];
    id = ((uint32_t)buf[8] << 24) | ((uint32_t)buf[7] << 16) | \
         ((uint32_t)buf[6] << 8) | buf[5];

    Serial.print("Chip ID ");
    Serial.println(id);
}

void EEPROM_Config::init() {
    magic = EEPROM_MAGIC;
    version = EEPROM_VERSION;

    if (!id || !(id & 0x00ffffff) || (id & 0xff000000) != CHIP_ID_MSB) {
        id = CHIP_ID_MSB | random(0x01000000);
        Serial.print("New Chip ID ");
        Serial.println(id);
    }

    max_volume = MAX_VOLUME;
    min_volume = MIN_VOLUME;
}

bool EEPROM_Config::check() {
    if (magic != EEPROM_MAGIC)
        return false;

    if (version != EEPROM_VERSION)
        return false;

    if (!id || !(id & 0x00ffffff) || (id & 0xff000000) != 0xad000000)
        return false;

    return true;
}
// vim: ts=4 sw=4 et cindent
