/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2019 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include "buttons.h"
#include "tonuino.h"

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

        if (buttons[i]->isPressed()) {
            if (!(ignore_map & (1 << (i + 2 * BUTTON_MAX))))
                button_map |= 1 << i;
        }

        if (buttons[i]->wasReleased()) {
            if (!(ignore_map & (1 << (i + 2 * BUTTON_MAX))))
                button_map |= 1 << (i + BUTTON_MAX);
            ignore_map &= ~(1 << (i + 2 * BUTTON_MAX));
        }

        if (buttons[i]->pressedFor(LONG_PRESS_TIMEOUT)) {
            if (!(ignore_map & (1 << (i + 2 * BUTTON_MAX))))
                button_map |= 1 << (i + 2 * BUTTON_MAX);
            ignore_map |= 1 << (i + 2 * BUTTON_MAX);
        }
    }

    if (button_map)
        tonuino->notify_buttons(button_map);
}
