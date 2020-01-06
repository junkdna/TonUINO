/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2019 Tillmann Heidsieck <theidsieck@leenox.de>
 */
#ifndef _EEPROM_CONFIG_H_
#define _EEPROM_CONFIG_H_

#define CHIP_ID_MSB        0x01000000
#define EEPROM_CFG_LEN     128
#define EEPROM_CFG_SIZE    13
#define EEPROM_MAGIC       0x5a5f5059
#define EEPROM_VERSION     2
#define INITIAL_VOLUME     15
#define LONG_PRESS_TIMEOUT 1000
#define MAX_VOLUME         35
#define MIN_VOLUME         1
#define STEP_VOLUME        1

class EEPROM_Config {
public:
    uint32_t magic;
    uint8_t  version;
    uint32_t id;
    uint8_t  max_volume;
    uint8_t  min_volume;
    uint8_t  step_volume;
    uint8_t  init_volume;

    void write();
    void read();
    void init();
    bool check();
};
#endif
// vim: ts=4 sw=4 et cindent
