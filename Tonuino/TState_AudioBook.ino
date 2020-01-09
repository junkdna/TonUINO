/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2018-2019 Thorsten Voß <voss@bummel.de>
 * Copyright 2019-2020 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include "buttons.h"
#include "tonuino.h"
#include "tstate.h"
#include "rfid.h"


void TState_AudioBook::next() {
    TState::next();
    /* TODO add option to save progress on RFID card */
    EEPROM.write(EEPROM_CFG_LEN + current_folder, current_track);
}

void TState_AudioBook::prev() {
    TState::prev();
    /* TODO add option to save progress on RFID card */
    EEPROM.write(EEPROM_CFG_LEN + current_folder, current_track);
}

TState *TState_AudioBook::handle_buttons(uint32_t _map) {
    TState *state = this;

    for (int8_t i; i < MAX_MODIFICATORS; i++) {
        if (!mods[i])
            continue;
        state = mods[i]->handle_buttons(_map);
        if (state != this) {
            delete this;
            return state;
        }
    }

    if (button_next(_map)) {
        next();
    } else if (button_prev(_map)) {
        prev();
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

TState *TState_AudioBook::handle_card(RFIDCard *card) {
    TState *state = this;

    for (int8_t i; i < MAX_MODIFICATORS; i++) {
        if (!mods[i])
            continue;
        state = mods[i]->handle_card(card);
        if (state != this) {
            delete this;
            return state;
        }
    }

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

TState *TState_AudioBook::handle_dfplay_event(mp3_notify_event event, uint16_t code) {
    TState *state = this;

    for (int8_t i; i < MAX_MODIFICATORS; i++) {
        if (!mods[i])
            continue;
        state = mods[i]->handle_dfplay_event(event, code);
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
            if (!is_playing())
                next();
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

TState *TState_AudioBook::loop() {
    TState *state = this;

    for (int8_t i; i < MAX_MODIFICATORS; i++) {
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

TState_AudioBook::TState_AudioBook(TonUINO *context) {
    this->context = context;
}

TState_AudioBook::TState_AudioBook(TState *last_state) {
    from_last_state(last_state);
    Serial.println(F("AudioBook(last)"));
    current_folder = card->extdata[1];
    current_track = EEPROM.read(EEPROM_CFG_LEN + current_folder);
    current_folder_track_num = context->get_dfplayer()->getFolderTrackCount(current_folder);
    if (current_track > current_folder_track_num || current_track < 1) {
        current_track = 1;
        EEPROM.write(EEPROM_CFG_LEN + current_folder, current_track);
    }
    if (restore) {
        start();
        restore = false;
    } else {
        playFolderTrack(current_folder, current_track);
    }
}

TState_AudioBook::~TState_AudioBook() {
}
// vim: ts=4 sw=4 et cindent
