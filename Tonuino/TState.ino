/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2019 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include "tonuino.h"
#include "tstate.h"
#include "rfid.h"

/*******************************
 * state Idle
 *******************************/
TState* TState_Idle::handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed) {
    return this;
}

TState* TState_Idle::handle_card(RFIDCard* card) {
    TState* state = this;

    switch(card->card_mode) {
        case CARD_MODE_PLAYER:
            state = new_state_by_name(this, card->extdata[0]);
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState* TState_Idle::handle_dfplay_event(mp3_notify_event event, uint16_t code) {
    return this;
}

TState_Idle::TState_Idle(TonUINO *context) {
}

TState_Idle::TState_Idle(TState *last_state) {
}

TState_Idle::~TState_Idle() {
}


/*******************************
 * state Menu
 *******************************/
TState* TState_Menu::handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed) {
    return this;
}

TState* TState_Menu::handle_card(RFIDCard* card) {
    TState* state = this;

    switch(card->card_mode) {
        case CARD_MODE_PLAYER:
            state = new_state_by_name(this, card->extdata[0]);
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState* TState_Menu::handle_dfplay_event(mp3_notify_event event, uint16_t code) {
    return this;
}

TState_Menu::TState_Menu(TonUINO *context) {
}

TState_Menu::TState_Menu(TState *last_state) {
}

TState_Menu::~TState_Menu() {
}


/*******************************
 * state Album
 *******************************/
TState* TState_Album::handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed) {
    return this;
}

TState* TState_Album::handle_card(RFIDCard* card) {
    TState* state = this;

    switch(card->card_mode) {
        case CARD_MODE_PLAYER:
            state = new_state_by_name(this, card->extdata[0]);
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState* TState_Album::handle_dfplay_event(mp3_notify_event event, uint16_t code) {
    return this;
}

TState_Album::TState_Album(TonUINO *context) {
}

TState_Album::TState_Album(TState *last_state) {
}

TState_Album::~TState_Album() {
}



/*******************************
 * state AudioBook
 *******************************/
TState* TState_AudioBook::handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed) {
    return this;
}

TState* TState_AudioBook::handle_card(RFIDCard* card) {
    TState* state = this;

    switch(card->card_mode) {
        case CARD_MODE_PLAYER:
            state = new_state_by_name(this, card->extdata[0]);
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState* TState_AudioBook::handle_dfplay_event(mp3_notify_event event, uint16_t code) {
    return this;
}

TState_AudioBook::TState_AudioBook(TonUINO *context) {
}

TState_AudioBook::TState_AudioBook(TState *last_state) {
}

TState_AudioBook::~TState_AudioBook() {
}


/*******************************
 * state RadioPlay
 *******************************/
TState* TState_RadioPlay::handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed) {
    return this;
}

TState* TState_RadioPlay::handle_card(RFIDCard* card) {
    TState* state = this;

    switch(card->card_mode) {
        case CARD_MODE_PLAYER:
            state = new_state_by_name(this, card->extdata[0]);
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState* TState_RadioPlay::handle_dfplay_event(mp3_notify_event event, uint16_t code) {
    return this;
}

TState_RadioPlay::TState_RadioPlay(TonUINO *context) {
}

TState_RadioPlay::TState_RadioPlay(TState *last_state) {
}

TState_RadioPlay::~TState_RadioPlay() {
}


/*******************************
 * state Album_Random
 *******************************/
TState* TState_Album_Random::handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed) {
    return this;
}

TState* TState_Album_Random::handle_card(RFIDCard* card) {
    TState* state = this;

    switch(card->card_mode) {
        case CARD_MODE_PLAYER:
            state = new_state_by_name(this, card->extdata[0]);
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState* TState_Album_Random::handle_dfplay_event(mp3_notify_event event, uint16_t code) {
    return this;
}

TState_Album_Random::TState_Album_Random(TonUINO *context) {
}

TState_Album_Random::TState_Album_Random(TState *last_state) {
}

TState_Album_Random::~TState_Album_Random() {
}



/*******************************
 * state Global
 *******************************/
TState* TState_Global::handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed) {
    return this;
}

TState* TState_Global::handle_card(RFIDCard* card) {
    TState* state = this;

    switch(card->card_mode) {
        case CARD_MODE_PLAYER:
            state = new_state_by_name(this, card->extdata[0]);
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState* TState_Global::handle_dfplay_event(mp3_notify_event event, uint16_t code) {
    return this;
}

TState_Global::TState_Global(TonUINO *context) {
}

TState_Global::TState_Global(TState *last_state) {
}

TState_Global::~TState_Global() {
}



/*******************************
 * state Global_Random
 *******************************/
TState* TState_Global_Random::handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed) {
    return this;
}

TState* TState_Global_Random::handle_card(RFIDCard* card) {
    TState* state = this;

    switch(card->card_mode) {
        case CARD_MODE_PLAYER:
            state = new_state_by_name(this, card->extdata[0]);
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState* TState_Global_Random::handle_dfplay_event(mp3_notify_event event, uint16_t code) {
    return this;
}

TState_Global_Random::TState_Global_Random(TonUINO *context) {
}

TState_Global_Random::TState_Global_Random(TState *last_state) {
}

TState_Global_Random::~TState_Global_Random() {
}


/*******************************
 * base class
 *******************************/

void TState::volume_up() {
    volume_set(current_volume + 1);
}

void TState::volume_down() {
    volume_set(current_volume - 1);
}

void TState::volume_set(uint8_t vol) {
    if (vol > MAX_VOLUME && vol < MIN_VOLUME)
        return;

    current_volume = vol;
    context->get_dfplayer()->setVolume(current_volume);
    last_command = MP3_CMD_SET_VOL;
}

TState* new_state_by_name(TState* orig, uint8_t state_name) {
    switch (state_name) {
    case STATE_MENU:
        return new TState_Menu(orig);
        break;
    case STATE_ALBUM:
        return new TState_Album(orig);
        break;
    case STATE_ALBUM_RANDOM:
        return new TState_Album_Random(orig);
        break;
    case STATE_AUDIO_BOOK:
        return new TState_AudioBook(orig);
        break;
    case STATE_RADIO_PLAY:
        return new TState_RadioPlay(orig);
        break;
    case STATE_GLOBAL:
        return new TState_Global(orig);
        break;
    case STATE_GLOBAL_RANDOM:
        return new TState_Global_Random(orig);
        break;
    default:
        return orig;
    }
}

TState::TState() {
}

TState::TState(TState *last_state) : context(last_state->get_context()) {
}

TState::TState(TonUINO *context) : context(context) {
}

TState::~TState() {
}

// vim: ts=4 sw=4 et cindent
