/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2020 Tillmann Heidsieck <theidsieck@leenox.de>
 */
#ifndef _NOTIFICATION_LED_H_
#define _NOTIFICATION_LED_H_

enum notification_led_state {
    LED_STATE_IDLE,
    LED_STATE_PLAY,
    LED_STATE_PAUSE,
    LED_STATE_MENU,
    LED_STATE_ERROR,
};

class NotificationLED {
protected:
    notification_led_state state = LED_STATE_IDLE;

public:
    virtual void loop();
    virtual void update_state(notification_led_state _state);
    NotificationLED();
    ~NotificationLED();
};

class NotificationLED_3LEDs : public NotificationLED {
protected:
    uint8_t led_pins[3];
    uint32_t ms = 0;
    uint8_t idx;
public:
    void loop() override;
    void update_state(notification_led_state _state) override;
    explicit NotificationLED_3LEDs(uint8_t led0, uint8_t led1, uint8_t led2);
};
#endif
// vim: ts=4 sw=4 et cindent
