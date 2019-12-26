/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright TODO Thorsten Voß
 * Copyright 2019 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include "buttons.h"
#include "rfid.h"
#include "tonuino.h"
#include "tstate.h"

TonUINO tonuino;

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
    id = 0xad | random(0x01000000);

    Serial.print("New Chip ID ");
    Serial.println(id);
}

bool EEPROM_Config::check() {
    if (magic != EEPROM_MAGIC)
        return false;

    if (version != EEPROM_VERSION)
        return false;

    if (!id || !(id & 0x00ffffff))
        return false;

    return true;
}

DFMiniMp3<SoftwareSerial, Mp3Notify>* TonUINO::get_dfplayer()
{
    return this->dfplay;
}

EEPROM_Config &TonUINO::get_config()
{
    return this->config;
}

void TonUINO::notify_mp3(mp3_notify_event event, uint16_t code) {
    (void)code;
    switch(event) {
        case MP3_NOTIFY_ERROR:
            /* Fallthrough */
        case MP3_PLAY_FINISHED:
            /* Fallthrough */
        case MP3_CARD_ONLINE:
            /* Fallthrough */
        case MP3_CARD_INSERTED:
            /* Fallthrough */
        case MP3_CARD_REMOVED:
            state = state->handle_dfplay_event(event, code);
            break;
        case MP3_USB_ONLINE:
            /* ignore for now */
            break;
        case MP3_USB_INSERTED:
            /* ignore for now */
            break;
        case MP3_USB_REMOVED:
            /* ignore for now */
            break;
    }
}

void TonUINO::notify_buttons(uint8_t pressed, uint8_t released, uint8_t long_pressed)
{
    buttons_pressed = pressed;
    buttons_released = released;
    buttons_long_press = long_pressed;
}

void TonUINO::notify_rfid(RFIDCard *card)
{
    if (rfid_card)
        delete rfid_card;

    rfid_card = card;
    if (!rfid_card->check(config.id))
        rfid_card->card_mode = 255;

    state = state->handle_card(rfid_card);
}

void TonUINO::setup(DFMiniMp3<SoftwareSerial, Mp3Notify>* dfp) {
    /* PIN A3 is open, the ADC should produce noise */
    randomSeed(analogRead(A3));

    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    digitalWrite(LED_GREEN, 0);
    digitalWrite(LED_RED, 0);
    digitalWrite(LED_BLUE, 0);

    pinMode(BUSY_PIN, INPUT);
    dfplay = dfp;
    dfplay->begin();
    dfplay->loop();

    dfplay->reset();
    dfplay->loop();

    config.read();
    if (!config.check()) {
        config.init();
        config.write();
    }

    state = new TState_Idle(&tonuino);
    state->volume_set(INITIAL_VOLUME);

    if (buttons_pressed == 0x7) {
        /* reset */
        buttons_pressed = 0;
    }
}

void TonUINO::loop() {
    dfplay->loop();
    state = state->handle_buttons(buttons_pressed, buttons_released, buttons_long_press);
    buttons_pressed = 0;
    buttons_released = 0;
    buttons_long_press = 0;
    state = state->run();
}

static SoftwareSerial g_soft_uart(SOFT_UART_RX_PIN, SOFT_UART_TX_PIN);
static DFMiniMp3<SoftwareSerial, Mp3Notify> g_dfplay(g_soft_uart);

MFRC522 mfrc522(MFRC522_SS_PIN, MFRC522_RST_PIN);
RFIDReader rfid(&mfrc522);

static Button button_down(DOWN_BUTTON_PIN, 25, false, true);
static Button button_pause(PAUSE_BUTTON_PIN, 25, false, true);
static Button button_up(UP_BUTTON_PIN, 25, false, true);
static Buttons buttons(&button_down, &button_pause, &button_up);

void setup() {
    Serial.begin(115200);

    Serial.println(F("TonUINO NG Version 0.1"));
    Serial.println(F("(c) Tilllmann Heidsieck"));
    Serial.println(F("based on work by Thorsten Voß"));

    buttons.setup(&tonuino);
    rfid.setup(&tonuino);
    tonuino.setup(&g_dfplay);
}

void loop() {
    buttons.loop();
    rfid.loop();
    tonuino.loop();
}
// vim: ts=4 sw=4 et cindent
