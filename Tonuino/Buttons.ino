#include "buttons.h"

void Buttons::setup(TonUINO *tonuino) {
    this->tonuino = tonuino;
    for (int8_t i = 0; i < BUTTON_MAX; i++)
        buttons[i]->begin();
    loop();
}

void Buttons::loop() {
    uint8_t pressed = 0;
    uint8_t released = 0;
    uint8_t long_press = 0;

    for (int8_t i = 0; i < BUTTON_MAX; i++) {
        buttons[i]->read();

        if (buttons[i]->isPressed())
            pressed |= 1 << i;

        if (buttons[i]->wasReleased())
            released |= 1 << i;

        if (buttons[i]->pressedFor(LONG_PRESS_TIMEOUT))
            long_press |= 1 << i;
    }

    if (released || long_press)
        tonuino->notify_buttons(pressed, released, long_press);
}
// vim: ts=4 sw=4 et cindent
