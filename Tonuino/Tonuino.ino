/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright TODO Thorsten Voß
 * Copyright 2019 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include "buttons.h"
#include "messages.h"
#include "rfid.h"
#include "tonuino.h"
#include "tstate.h"

TonUINO tonuino;

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

void TonUINO::notify_buttons(uint32_t _map)
{
    button_map = _map;

    if (state)
        state = state->handle_buttons(button_map);

    /* TODO let the state consume what it needs ? */
    button_map = 0;
}

void TonUINO::notify_rfid(RFIDCard *card)
{
    /* TODO this might be stupid for mod cards we need to keep the current
     * player card
     */
    if (rfid_card)
        delete rfid_card;

    rfid_card = card;
    if (!rfid_card->check(config.id))
        rfid_card->card_mode = 255;

    state = state->handle_card(rfid_card);
}

void TonUINO::setup(DFMiniMp3<SoftwareSerial, Mp3Notify>* dfp) {
    uint32_t i, seed = 0;

    /* PIN A3 is open, the ADC should produce noise */
    for (i = 0; i < 128; i++)
        seed ^= (analogRead(A3) & 0x1) << (i % 32);
    randomSeed(seed);

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
    state->volume_set(config.init_volume);

    //if ((button_map & 0x7) == 0x7) {
    if (digitalRead(PAUSE_BUTTON_PIN) == LOW && digitalRead(UP_BUTTON_PIN) == LOW &&
            digitalRead(DOWN_BUTTON_PIN) == LOW) {
        /* reset */
        config.init();
        config.write();
        state->volume_set(config.init_volume);
        dfplay->playMp3FolderTrack(MESSAGE_RESET_DONE);
        delay(200);
        button_map = 0;
    }

}

void TonUINO::loop() {
    dfplay->loop();
    state = state->loop();
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
