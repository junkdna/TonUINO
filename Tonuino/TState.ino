/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2018-2019 Thorsten Voß <voss@bummel.de>
 * Copyright 2019-2020 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include "buttons.h"
#include "player.h"
#include "rfid.h"
#include "tonuino.h"
#include "tstate.h"

TState *new_state_by_name(TState *orig, uint8_t state_name, bool restore = false) {
    orig->set_restore(restore);

    switch (state_name) {
    case STATE_IDLE:
        return new TState_Idle(orig);
        break;
    case STATE_NEW_CARD:
        return new TState_NewCard(orig);
        break;
    case STATE_ALBUM:
        return new TState_Album(orig);
        break;
    case STATE_ALBUM_RANDOM:
        return new TState_AlbumRandom(orig);
        break;
    case STATE_AUDIO_BOOK:
        return new TState_AudioBook(orig);
        break;
    case STATE_RADIO_PLAY:
        return new TState_RadioPlay(orig);
        break;
    case STATE_SINGLE:
        return new TState_Single(orig);
        break;
    case STATE_ADMIN:
        return new TState_Menu(orig);
        break;
    default:
        return orig;
    }
}

/* TODO add option to clean some variables */
void TState::from_last_state(TState *last_state) {
    /* TODO do we always want to delete all mods here or should some stay in place
     * maybe add a marker?
     * if (!mods[i] || mods[i]->marker)
     *     continue;
     */
    for (int8_t i = 0; i < MAX_MODIFICATORS; i++) {
        if (!mods[i])
            continue;
        delete mods[i];
        mods[i] = nullptr;
    }

    this->card                     = last_state->card;
    this->context                  = last_state->context;
    this->restore                  = last_state->restore;
    this->player                   = last_state->player;
    this->notify_led               = last_state->notify_led;

    this->player->set_state(this);
}

void TState::set_restore(bool r) {
    restore = r;
}

TState::TState() {
}

TState::TState(TState *last_state) {
    from_last_state(last_state);
}

TState::TState(TonUINO *context) {
    this->context = context;
}

TState::~TState() {
}
// vim: ts=4 sw=4 et cindent
