/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2018-2019 Thorsten Voß <voss@bummel.de>
 * Copyright 2019-2020 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include "buttons.h"
#include "tonuino.h"
#include "tstate.h"
#include "rfid.h"


TState *TState_AlbumRandom::handle_buttons(uint32_t _map) {
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
        player->playRandomTrack(player->get_current_folder());
    } else if (button_prev(_map)) {
        player->playFolderTrack(player->get_current_folder(), player->get_current_track());
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

TState *TState_AlbumRandom::handle_card(RFIDCard *card) {
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
#if 0
        case CARD_MODE_PLAYER:
            this->card = card;
            state = new_state_by_name(this, card->extdata[0]);
            break;
#endif
        case CARD_MODE_MODIFY:
            apply_modificator(card);
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_AlbumRandom::handle_player_event(mp3_notify_event event, uint16_t code) {
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
            state = player->handle_error(code);
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

TState *TState_AlbumRandom::loop() {
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

TState_AlbumRandom::TState_AlbumRandom(TonUINO *context) {
    this->context = context;
}

TState_AlbumRandom::TState_AlbumRandom(TState *last_state) {
    from_last_state(last_state);
    notify_led->update_state(LED_STATE_PLAY);
    Serial.println(F("AlbumRandom(last)"));
    if (restore) {
        player->start();
        restore = false;
    } else {
        player->playRandomTrack(card->extdata[1]);
    }
}

TState_AlbumRandom::~TState_AlbumRandom() {
}
// vim: ts=4 sw=4 et cindent
