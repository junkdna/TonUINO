/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2019 Tillmann Heidsieck <theidsieck@leenox.de>
 */
#ifndef _BUTTONS_H_
#define _BUTTONS_H_

#include <JC_Button.h>

#define LONG_PRESS_TIMEOUT      1000
#define LONG_PRESS_IS_VOLUME_KEY

class TonUINO;

/* N.B.: Maximum number of buttons is limited to 10 */
enum button_names {
    BUTTON_DOWN = 0,
    BUTTON_PAUSE,
    BUTTON_UP,
    BUTTON_MAX
};

class Buttons {
    protected:
        Button *buttons[BUTTON_MAX] = {nullptr};
        TonUINO *tonuino = nullptr;
        uint32_t ignore_map;

    public:
        void setup(TonUINO *tonuino);
        void loop();

        Buttons(Button *down, Button *pause, Button *up) :
            buttons{down, pause, up}, ignore_map(0) {}
};

static inline bool button_pressed(uint32_t _map, button_names button) {
    return _map & (1 << button);
}

static inline bool button_released(uint32_t _map, button_names button) {
    return _map & (1 << (BUTTON_MAX + button));
}

static inline bool button_long_pressed(uint32_t _map, button_names button) {
    return _map & (1 << (2 * BUTTON_MAX + button));
}

#ifdef LONG_PRESS_IS_VOLUME_KEY
static inline bool button_vol_up(uint32_t _map) {
    return button_long_pressed(_map, BUTTON_UP);
}

static inline bool button_vol_down(uint32_t _map) {
    return button_long_pressed(_map, BUTTON_DOWN);
}

static inline bool button_next(uint32_t _map) {
    return button_released(_map, BUTTON_UP);
}

static inline bool button_prev(uint32_t _map) {
    return button_released(_map, BUTTON_DOWN);
}
#else
static inline bool button_vol_up(uint32_t _map) {
    return button_released(_map, BUTTON_UP);
}

static inline bool button_vol_down(uint32_t _map) {
    return button_released(_map, BUTTON_DOWN);
}

static inline bool button_next(uint32_t _map) {
    return button_long_pressed(_map, BUTTON_UP);
}

static inline bool button_prev(uint32_t _map) {
    return button_long_pressed(_map, BUTTON_DOWN);
}
#endif


#endif
// vim: ts=4 sw=4 et cindent
