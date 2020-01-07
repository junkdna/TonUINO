/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright TODO Thorsten Voß
 * Copyright 2019 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#ifndef _TONUINO_H_
#define _TONUINO_H_

#include <DFMiniMp3.h>
#include <MFRC522.h>
#include <SPI.h>
#include <SoftwareSerial.h>

#include "eeprom_config.h"

#define LONG_PRESS_IS_VOLUME_KEY

#if 0
#define SOFT_UART_RX_PIN 5
#define SOFT_UART_TX_PIN 6
#else
#define SOFT_UART_RX_PIN 2
#define SOFT_UART_TX_PIN 3
#endif

#define LED_GREEN        A4
#define LED_RED          A5
#define LED_BLUE         A6

#if 0
#define BUSY_PIN         7
#else
#define BUSY_PIN         4
#endif

#define PAUSE_BUTTON_PIN A0
#define UP_BUTTON_PIN    A1
#define DOWN_BUTTON_PIN  A2

#define MFRC522_RST_PIN  9
#define MFRC522_SS_PIN   10

enum mp3_notify_event {
    MP3_NOTIFY_ERROR,
    MP3_PLAY_FINISHED,
    MP3_CARD_ONLINE,
    MP3_CARD_INSERTED,
    MP3_CARD_REMOVED,
    MP3_USB_ONLINE,
    MP3_USB_INSERTED,
    MP3_USB_REMOVED,
};

class Mp3Notify;
class RFIDCard;
class RFIDReader;
class TState;

class TonUINO {
    protected:
        EEPROM_Config config;
        DFMiniMp3<SoftwareSerial, Mp3Notify> *dfplay;
        TState *state;
        RFIDCard *rfid_card;

        uint32_t button_map = 0;

    public:
        DFMiniMp3<SoftwareSerial, Mp3Notify>* get_dfplayer();
        EEPROM_Config &get_config();
        void setup(DFMiniMp3<SoftwareSerial, Mp3Notify> *dfp);
        void loop();

        /* this goes into the state */
        void notify_buttons(uint32_t _map);

        void notify_rfid(RFIDCard *rfid_card);
        void notify_mp3(mp3_notify_event event, uint16_t code);
};

extern TonUINO tonuino;

class Mp3Notify {
    public:
        static void OnError(uint16_t errorCode) {
#if 0
            Serial.println();
            Serial.print("Com Error ");
            Serial.println(errorCode);
#endif
            tonuino.notify_mp3(MP3_NOTIFY_ERROR, errorCode);
        }
        static void OnPlayFinished(uint16_t track) {
#if 0
            Serial.print("Track beendet");
            Serial.println(track);
#endif
            tonuino.notify_mp3(MP3_PLAY_FINISHED, track);
        }
        static void OnCardOnline(uint16_t code) {
#if 0
            Serial.println(F("SD Karte online "));
#endif
            tonuino.notify_mp3(MP3_CARD_ONLINE, code);
        }
        static void OnCardInserted(uint16_t code) {
#if 0
            Serial.println(F("SD Karte bereit "));
#endif
            tonuino.notify_mp3(MP3_CARD_INSERTED, code);
        }
        static void OnCardRemoved(uint16_t code) {
#if 0
            Serial.println(F("SD Karte entfernt "));
#endif
            tonuino.notify_mp3(MP3_CARD_REMOVED, code);
        }
        static void OnUsbOnline(uint16_t code) {
#if 0
            Serial.println(F("USB online "));
#endif
            tonuino.notify_mp3(MP3_USB_ONLINE, code);
        }
        static void OnUsbInserted(uint16_t code) {
#if 0
            Serial.println(F("USB bereit "));
#endif
            tonuino.notify_mp3(MP3_USB_INSERTED, code);
        }
        static void OnUsbRemoved(uint16_t code) {
#if 0
            Serial.println(F("USB entfernt "));
#endif
            tonuino.notify_mp3(MP3_USB_REMOVED, code);
        }
};

#endif
// vim: ts=4 sw=4 et cindent
