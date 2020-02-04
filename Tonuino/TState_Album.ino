/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2018-2019 Thorsten Voß <voss@bummel.de>
 * Copyright 2019-2020 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include "buttons.h"
#include "tonuino.h"
#include "tstate.h"
#include "rfid.h"


TState *TState_Album::handle_buttons(uint32_t _map) {
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

    if (button_next(_map)) {
        player->next();
    } else if (button_prev(_map)) {
        player->prev();
    } else if (button_vol_up(_map)) {
        player->volume_up();
    } else if (button_vol_down(_map)) {
        player->volume_down();
    } else if (button_released(_map, BUTTON_PAUSE)) {
        player->pause();
        state = new_state_by_name(this, STATE_IDLE, true);
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_Album::handle_card(RFIDCard *card) {
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

    /* do not handle empty cards */
    switch(card->card_mode) {
        case CARD_MODE_PLAYER:
#if 0
            this->card = card;
            state = new_state_by_name(this, card->extdata[0]);
#endif
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_Album::handle_player_event(mp3_notify_event event, uint16_t code) {
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
            /* TODO in case we want to stop playing here we need something else */
            if (!player->is_playing() && !player->next()) {
                set_error(true);
                state = new_state_by_name(this, STATE_IDLE);
            }
            break;
        case MP3_CARD_ONLINE:
            /* TODO should not happen */
            break;
        case MP3_CARD_INSERTED:
            /* TODO should not happen */
            break;
        case MP3_CARD_REMOVED:
            state = new_state_by_name(this, STATE_IDLE);
            break;
        default:
            break;
    }

    if (this != state)
        delete this;

    return state;
}

TState *TState_Album::loop() {
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

    if (!player->is_playing())
        Mp3Notify::OnPlayFinished(player->get_current_track());

    return this;
}

TState_Album::TState_Album(TonUINO *context) {
    this->context = context;
}

TState_Album::TState_Album(TState *last_state) {
    from_last_state(last_state);
    notify_led->update_state(LED_STATE_PLAY);
    Serial.println(F("Album(last)"));
    if (restore) {
        player->start();
        restore = false;
    } else {
        player->playFolderTrack(card->extdata[1], 1);
    }
}

TState_Album::~TState_Album() {
}
// vim: ts=4 sw=4 et cindent
