/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2018-2019 Thorsten Voß <voss@bummel.de>
 * Copyright 2019-2020 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include "buttons.h"
#include "tonuino.h"
#include "tstate.h"
#include "rfid.h"


TState *TState_Idle::handle_buttons(uint32_t _map) {
    TState *state = this;

    for (int8_t i = 0; i < MAX_MODIFICATORS; i++) {
        if (!mods[i])
            continue;
        state = mods[i]->handle_buttons(_map);
        if (state != this) {
            delete this;
            return state;
        }
    }

    if (button_vol_up(_map)) {
        player->volume_up();
    } else if (button_vol_down(_map)) {
        player->volume_down();
    } else if (button_released(_map, BUTTON_PAUSE)) {
        if (card && card->card_mode == CARD_MODE_PLAYER) {
            state = new_state_by_name(this, card->extdata[0], true);
            player->start();
        }
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_Idle::handle_card(RFIDCard *card) {
    TState *state = this;

    for (int8_t i = 0; i < MAX_MODIFICATORS; i++) {
        if (!mods[i])
            continue;
        state = mods[i]->handle_card(card);
        if (state != this) {
            delete this;
            return state;
        }
    }

    this->card = card;

    switch(card->card_mode) {
        case CARD_MODE_PLAYER:
            state = new_state_by_name(this, card->extdata[0]);
            break;
        case CARD_MODE_ADMIN:
            state = new_state_by_name(this, STATE_ADMIN);
            break;
        default:
            state = new_state_by_name(this, STATE_NEW_CARD);
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_Idle::handle_player_event(mp3_notify_event event, uint16_t code) {
    TState *state = this;

    for (int8_t i = 0; i < MAX_MODIFICATORS; i++) {
        if (!mods[i])
            continue;
        state = mods[i]->handle_player_event(event, code);
        if (state != this) {
            delete this;
            return state;
        }
    }

    switch (event) {
        case MP3_NOTIFY_ERROR:
            /* TODO handle */
            break;
        case MP3_PLAY_FINISHED:
            /* TODO should not be playing in idle state right, maybe second finish
               after last track */
            break;
        case MP3_CARD_ONLINE:
            /* TODO make helper */
            break;
        case MP3_CARD_INSERTED:
            break;
        case MP3_CARD_REMOVED:
            break;
        default:
            break;
    }

    return this;
}

TState *TState_Idle::loop() {
    TState *state = this;

    notify_led->loop();

    state = player->loop();
    if (state != this) {
        delete this;
        return state;
    }

    for (int8_t i = 0; i < MAX_MODIFICATORS; i++) {
        if (!mods[i])
            continue;
        state = mods[i]->loop();
        if (state != this) {
            delete this;
            return state;
        }
    }

    return this;
}

/* this function is called in case we have no player->previous state */
TState_Idle::TState_Idle(TonUINO *context, Player *player) {
    this->context = context;
    this->player = player;
    this->notify_led = new NotificationLED_3LEDs(LED_RED, LED_GREEN, LED_BLUE);
    this->notify_led->update_state(LED_STATE_IDLE);
}

TState_Idle::TState_Idle(TState *last_state) {
    from_last_state(last_state);

    if (restore)
        notify_led->update_state(LED_STATE_PAUSE);
    else if (!restore && !error)
        notify_led->update_state(LED_STATE_IDLE);
    else
        notify_led->update_state(LED_STATE_ERROR);

    error = false;
    Serial.println(F("idle(last)"));
}

TState_Idle::~TState_Idle() {
}
// vim: ts=4 sw=4 et cindent
