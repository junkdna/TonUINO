/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright TODO Thorsten Voß
 * Copyright 2019 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include "buttons.h"
#include "tonuino.h"
#include "tstate.h"
#include "rfid.h"


TState *TState_Album_Random::handle_buttons(uint32_t _map) {
    TState *state = this;

    if (button_next(_map)) {
        playRandomTrack(current_folder);
    } else if (button_prev(_map)) {
        playFolderTrack(current_folder, current_track);
    } else if (button_vol_up(_map)) {
        volume_up();
    } else if (button_vol_down(_map)) {
        volume_down();
    } else if (button_released(_map, BUTTON_PAUSE)) {
        pause();
        state = new_state_by_name(this, STATE_IDLE);
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_Album_Random::handle_card(RFIDCard *card) {
    TState *state = this;
    (void)card;

#if 0
    /* do not handle empty cards */
    switch(card->card_mode) {
        case CARD_MODE_PLAYER:
            this->card = card;
            state = new_state_by_name(this, card->extdata[0]);
            break;
    }
#endif

    if (state != this)
        delete this;

    return state;
}

TState *TState_Album_Random::handle_dfplay_event(mp3_notify_event event, uint16_t code) {
    TState *state = this;

    (void)code;

    switch (event) {
        case MP3_NOTIFY_ERROR:
            /* TODO handle */
            break;
        case MP3_PLAY_FINISHED:
            if (!is_playing())
                playRandomTrack(current_folder);
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

TState *TState_Album_Random::loop() {
    return this;
}

TState_Album_Random::TState_Album_Random(TonUINO *context) {
    this->context = context;
}

TState_Album_Random::TState_Album_Random(TState *last_state) {
    from_last_state(last_state);
    Serial.println(F("Album_Random(last)"));
    if (restore) {
        start();
        restore = false;
    } else {
        current_folder = 0;
        current_folder_track_num = 0;
        current_track = 0;
        playRandomTrack(card->extdata[1]);
    }
}

TState_Album_Random::~TState_Album_Random() {
}
// vim: ts=4 sw=4 et cindent
