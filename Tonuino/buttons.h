#ifndef _BUTTONS_H_
#define _BUTTONS_H_

#include <JC_Button.h>

class TonUINO;

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

    public:
        void setup(TonUINO *tonuino);
        void loop();

        Buttons(Button *down, Button *pause, Button *up) : buttons{down, pause, up} {}
};

#endif
