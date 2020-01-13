/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2020 Tillmann Heidsieck <theidsieck@leenox.de>
 */
#include "notification_led.h"

void NotificationLED_3LEDs::loop() {
    uint32_t delta;

    delta = millis() - ms;

    switch (state) {
        case LED_STATE_IDLE:
            if (delta >= 500) {
                digitalWrite(led_pins[idx], 1);
                idx = (idx + 1) % 3;
                digitalWrite(led_pins[idx], 0);
                ms = millis();
            }
            break;
        case LED_STATE_PLAY:
            break;
        case LED_STATE_PAUSE:
            if (delta >= 500) {
                digitalWrite(led_pins[idx], idx);
                idx = !idx;
            }
            break;
        case LED_STATE_MENU:
            break;
        case LED_STATE_ERROR:
            break;
    }
}

void NotificationLED_3LEDs::update_state(notification_led_state _state) {
    state = _state;
    idx = 0;
    ms = millis();

    /* reset LEDs to off */
    digitalWrite(led_pins[0], 1);
    digitalWrite(led_pins[1], 1);
    digitalWrite(led_pins[2], 1);

    switch (state) {
        case LED_STATE_IDLE:
            digitalWrite(led_pins[2], 0);
            break;
        case LED_STATE_PLAY:
            digitalWrite(led_pins[1], 0);
            break;
        case LED_STATE_PAUSE:
            break;
        case LED_STATE_MENU:
            digitalWrite(led_pins[2], 0);
            break;
        case LED_STATE_ERROR:
            digitalWrite(led_pins[0], 0);
            break;
    }
}

NotificationLED_3LEDs::NotificationLED_3LEDs(uint8_t led0, uint8_t led1, uint8_t led2) {
    led_pins[0] = led0;
    led_pins[1] = led1;
    led_pins[2] = led2;
    ms = 0;
    idx = 0;
}

NotificationLED::NotificationLED() {
}

NotificationLED::~NotificationLED() {
}
// vim: ts=4 sw=4 et cindent
