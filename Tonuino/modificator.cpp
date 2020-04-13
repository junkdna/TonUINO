/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2018-2019 Thorsten Voß <voss@bummel.de>
 * Copyright 2019-2020 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include "modificator.h"

Modificator::Modificator(TonUINO *context, TState *state) {
    this->context = context;
    this->state = state;
}

Modificator::Modificator() {
}
