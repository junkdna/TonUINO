/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2018-2019 Thorsten Voß <voss@bummel.de>
 * Copyright 2019-2020 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#ifndef _TONUINO_H_
#define _TONUINO_H_

#include <stdint.h>

#include <MFRC522.h>
#include <SPI.h>

#include "player.h"
#include "eeprom_config.h"

#define SOFT_UART_RX_PIN        2
#define SOFT_UART_TX_PIN        3

#define LED_GREEN               A5
#define LED_RED                 A4
#define LED_BLUE                A3

#define BUSY_PIN                4
#define SPK_ENABLE_PIN          5
#define HPP_PIN                 6

#define BUTTON_EXTERNAL_PULLUP  true

#define PAUSE_BUTTON_PIN        A0
#define UP_BUTTON_PIN           A1
#define DOWN_BUTTON_PIN         A2

#define MFRC522_RST_PIN         9
#define MFRC522_SS_PIN          10

class RFIDCard;
class RFIDReader;
class TState;

class TonUINO {
    protected:
        EEPROM_Config config;
        RFIDCard *rfid_card;
        Player player;

        TState *state;

        uint32_t button_map = 0;

    public:
        EEPROM_Config &get_config();
        Player &get_player();
        void setup();
        void loop();

        /* this goes into the state */
        void notify_buttons(uint32_t _map);
        void notify_rfid(RFIDCard *rfid_card);
        void notify_mp3(mp3_notify_event event, uint16_t code);
};
#endif
// vim: ts=4 sw=4 et cindent
