/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2019 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include "tonuino.h"
#include "tstate.h"
#include "rfid.h"

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

TState::TState() {
}

TState::TState(TState *last_state) : context(last_state->get_context()) {
}

TState::TState(TonUINO *context) : context(context) {
}

TState::~TState() {
}


/*******************************
 * idle state
 *******************************/
TState* TState_Idle::handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed) {
    return this;
}

TState* TState_Idle::handle_card(RFIDCard* card) {
    return this;
}

TState* TState_Idle::handle_dfplay_event(mp3_notify_event event, uint16_t code) {
    return this;
}

TState_Idle::TState_Idle(TonUINO *context) {
    this->context = context;
}

TState_Idle::TState_Idle(TState *last_state) {
    context = last_state->get_context();
}

TState_Idle::~TState_Idle() {
}


/*******************************
 * state
 *******************************/

// vim: ts=4 sw=4 et cindent
