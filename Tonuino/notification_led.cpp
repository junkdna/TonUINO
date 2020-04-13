/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2020 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include <Arduino.h>
#include "notification_led.h"

void NotificationLED_3LEDs::loop() {
    uint32_t delta;

    delta = millis() - ms;
    idx %= 3;

    if (delta > 250 && flash_led >= 0)
        digitalWrite(led_pins[flash_led], 1);

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
            digitalWrite(led_pins[1], idx);
            idx = !idx;
            ms = millis();
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
    ms = 0;

    /* reset LEDs to off */
    digitalWrite(led_pins[0], 1);
    digitalWrite(led_pins[1], 1);
    digitalWrite(led_pins[2], 1);

    switch (state) {
    case LED_STATE_IDLE:
        digitalWrite(led_pins[0], 0);
        digitalWrite(led_pins[1], 0);
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

void NotificationLED_3LEDs::flash(int8_t led)
{
    if (led < 0 || led > 2)
        return;

    flash_led = led;
    digitalWrite(led_pins[flash_led], 0);
    ms = millis();
}

NotificationLED_3LEDs::NotificationLED_3LEDs(uint8_t led0, uint8_t led1, uint8_t led2) {
    led_pins[0] = led0;
    led_pins[1] = led1;
    led_pins[2] = led2;
    ms = 0;
    idx = 0;

    pinMode(led_pins[0], OUTPUT);
    pinMode(led_pins[1], OUTPUT);
    pinMode(led_pins[2], OUTPUT);
}

NotificationLED::NotificationLED() {
}

NotificationLED::~NotificationLED() {
}
