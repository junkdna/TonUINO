/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2020 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include "modificator.h"

TState *Modificator_LockKeys::loop()  {
    return state;
}

TState *Modificator_LockKeys::handle_buttons(uint32_t &_map)  {
	_map = 0;
    return state;
}

TState *Modificator_LockKeys::handle_card(RFIDCard *card)  {
    return state;
}

TState *Modificator_LockKeys::handle_dfplay_event(mp3_notify_event &event, uint16_t &code)  {
    return state;
}

Modificator_LockKeys::Modificator_LockKeys(TonUINO *context, TState *state) {
    this->context = context;
    this->state = state;
    /* TODO playAdvert(MESSAGE_MOD_LOCK_KEYS); */
}

Modificator_LockKeys::~Modificator_LockKeys() {
}
// vim: ts=4 sw=4 et cindent
