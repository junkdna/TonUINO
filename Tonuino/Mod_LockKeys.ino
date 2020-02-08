/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2020 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include "modificator.h"
#include "messages.h"
#include "tonuino.h"

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

TState *Modificator_LockKeys::handle_player_event(mp3_notify_event &event, uint16_t &code)  {
    return state;
}

Modificator_LockKeys::Modificator_LockKeys(TonUINO *context, TState *state) {
    this->context = context;
    this->state = state;
    context->get_player().playAdvertTrack(ADVERT_MOD_LOCK_ON);
}

Modificator_LockKeys::~Modificator_LockKeys() {
    context->get_player().playAdvertTrack(ADVERT_MOD_LOCK_OFF);
}
// vim: ts=4 sw=4 et cindent
