/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2018-2019 Thorsten Voß <voss@bummel.de>
 * Copyright 2019-2020 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include <Arduino.h>

#include "buttons.h"
#include "messages.h"
#include "rfid.h"
#include "tonuino.h"
#include "tstate.h"

TonUINO tonuino;

EEPROM_Config &TonUINO::get_config()
{
    return this->config;
}

Player &TonUINO::get_player()
{
    return this->player;
}

void TonUINO::notify_mp3(mp3_notify_event event, uint16_t code) {
    state = state->handle_player_event(event, code);
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
    if (!rfid_card->check())
        rfid_card->card_mode = 255;

    state = state->handle_card(rfid_card);
}

void TonUINO::setup() {
    uint32_t i, seed = 0;

    config.read();
    if (!config.check()) {
        config.init();
        config.write();
    }

    state = new TState_Idle(this, &player);

    player.set_context(this);
    player.set_state(state);

    player.setup();
    player.loop();

    /* PIN A7 is open, the ADC should produce noise */
    for (i = 0; i < 256; i++)
        seed ^= (analogRead(A7) & 0x1) << (i % 32);
    randomSeed(seed);

    if (digitalRead(PAUSE_BUTTON_PIN) == LOW && digitalRead(UP_BUTTON_PIN) == LOW &&
        digitalRead(DOWN_BUTTON_PIN) == LOW) {
        Serial.println(F("reset config"));
        /* reset config*/
        config.init();
        config.write();
        player.volume_set(config.init_volume);
        player.playMP3Track(MESSAGE_RESET_DONE);
        button_map = 0;
        /* TODO reset board ? */
    }
}

void TonUINO::loop() {
    state = state->loop();
}

MFRC522 mfrc522(MFRC522_SS_PIN, MFRC522_RST_PIN);
RFIDReader rfid(&mfrc522);

static Button button_down(DOWN_BUTTON_PIN, 25, !BUTTON_EXTERNAL_PULLUP, true);
static Button button_pause(PAUSE_BUTTON_PIN, 25, !BUTTON_EXTERNAL_PULLUP, true);
static Button button_up(UP_BUTTON_PIN, 25, !BUTTON_EXTERNAL_PULLUP, true);
static Buttons buttons(&button_down, &button_pause, &button_up);

void setup() {
    Serial.begin(115200);

    Serial.println(F("TonUINO NG Version 0.1"));
    Serial.println(F("(c) Tilllmann Heidsieck"));
    Serial.println(F("based on work by Thorsten Voß"));

    buttons.setup(&tonuino);
    rfid.setup(&tonuino);
    tonuino.setup();
}

void loop() {
    buttons.loop();
    rfid.loop();
    tonuino.loop();
}
