/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2019 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include "buttons.h"

void Buttons::setup(TonUINO *tonuino) {
    this->tonuino = tonuino;
    for (int8_t i = 0; i < BUTTON_MAX; i++)
        buttons[i]->begin();
    loop();
}

void Buttons::loop() {
    uint32_t button_map = 0;

    for (int8_t i = 0; i < BUTTON_MAX; i++) {
        buttons[i]->read();

        if (buttons[i]->isPressed())
            button_map |= 1 << i;

        if (buttons[i]->wasReleased())
            button_map |= 1 << (i + BUTTON_MAX);

        if (buttons[i]->pressedFor(LONG_PRESS_TIMEOUT))
            button_map |= 1 << (i + 2 * BUTTON_MAX);
    }

    if (button_map)
        tonuino->notify_buttons(button_map);
}
// vim: ts=4 sw=4 et cindent
