/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2020 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#ifndef _MODIFICATOR_H_
#define _MODIFICATOR_H_

#include "tonuino.h"
#include "tstate.h"

class Modificator {
protected:
	TonUINO *context = nullptr;
    TState *state = nullptr;

public:
	virtual TState *loop() = 0;
	virtual TState *handle_buttons(uint32_t &_map) = 0;
	virtual TState *handle_card(RFIDCard *card) = 0;
	virtual TState *handle_player_event(mp3_notify_event &event, uint16_t &code) = 0;

	explicit Modificator(TonUINO *context, TState *state);
    Modificator();
	~Modificator();
};

class Modificator_LockKeys : public Modificator {
	TState *loop() override;
	TState *handle_buttons(uint32_t &_map) override;
	TState *handle_card(RFIDCard *card) override;
	TState *handle_player_event(mp3_notify_event &event, uint16_t &code) override;

	explicit Modificator_LockKeys(TonUINO *context, TState *state);
	~Modificator_LockKeys();
};

#endif
// vim: ts=4 sw=4 et cindent
