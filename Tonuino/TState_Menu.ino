/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2018-2019 Thorsten Voß <voss@bummel.de>
 * Copyright 2019-2020 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include "buttons.h"
#include "tonuino.h"
#include "tstate.h"
#include "rfid.h"


TState *TState_Menu::handle_buttons(uint32_t _map) {
    TState *state = this;

    player->stop();
    if (menu_item == 0) {
        if (button_released(_map, BUTTON_UP) || button_long_pressed(_map, BUTTON_UP)) {
            ++selected_value;
            if (selected_value > MENU_ENTRIES)
                selected_value = 1;
            player->playMP3Track(MESSAGE_MENU_WELCOME + selected_value);
        } else if (button_released(_map, BUTTON_DOWN) || button_long_pressed(_map, BUTTON_DOWN)) {
            --selected_value;
            if (selected_value < 1)
                selected_value = MENU_ENTRIES;
            player->playMP3Track(MESSAGE_MENU_WELCOME + selected_value);
        } else if (button_released(_map, BUTTON_PAUSE)) {
            menu_item = selected_value;
            if (selected_value == MENU_VOL_STEP) {
                selected_value = context->get_config().step_volume;
                player->playMP3Track(selected_value);
            } else if (selected_value == MENU_VOL_MAX) {
                selected_value = context->get_config().max_volume;
                player->playMP3Track(selected_value);
            } else if (selected_value == MENU_VOL_MIN) {
                selected_value = context->get_config().min_volume;
                player->playMP3Track(selected_value);
            } else if (selected_value == MENU_VOL_INIT) {
                selected_value = context->get_config().init_volume;
                player->playMP3Track(selected_value);
            } else if (selected_value == MENU_NEW_CARD) {
                state = new_state_by_name(this, STATE_NEW_CARD);
            } else if (selected_value == MENU_EXIT) {
                state = new_state_by_name(this, STATE_IDLE);
            }
        } else if (button_long_pressed(_map, BUTTON_PAUSE)) {
            player->playMP3Track(MESSAGE_MENU_EXIT);
            state = new_state_by_name(this, STATE_IDLE);
        }
    } else if (menu_item == 1) {
        /* Volume steps */
        if (button_released(_map, BUTTON_UP) || button_long_pressed(_map, BUTTON_UP)) {
            ++selected_value;
            player->playMP3Track(selected_value);
            if (selected_value > 10)
                selected_value = 1;
        } else if (button_released(_map, BUTTON_DOWN) || button_long_pressed(_map, BUTTON_DOWN)) {
            --selected_value;
            if (selected_value < 1)
                selected_value = 1;
            player->playMP3Track(selected_value);
        } else if (button_released(_map, BUTTON_PAUSE)) {
            context->get_config().step_volume = selected_value;
            context->get_config().write();
            state = new_state_by_name(this, STATE_ADMIN);
        }
    } else if (menu_item == 2) {
        /* Volume max */
        if (button_released(_map, BUTTON_UP) || button_long_pressed(_map, BUTTON_UP)) {
            ++selected_value;
            player->playMP3Track(selected_value);
            if (selected_value > 35)
                selected_value = 1;
        } else if (button_released(_map, BUTTON_DOWN) || button_long_pressed(_map, BUTTON_DOWN)) {
            --selected_value;
            if (selected_value < 1)
                selected_value = 1;
            player->playMP3Track(selected_value);
        } else if (button_released(_map, BUTTON_PAUSE)) {
            context->get_config().step_volume = selected_value;
            context->get_config().write();
            state = new_state_by_name(this, STATE_ADMIN);
        }
    } else if (menu_item == 3) {
        /* Volume min */
        if (button_released(_map, BUTTON_UP)) {
            ++selected_value;
            player->playMP3Track(selected_value);
            if (selected_value > 35)
                selected_value = 1;
        } else if (button_released(_map, BUTTON_DOWN)) {
            --selected_value;
            if (selected_value < 1)
                selected_value = 1;
            player->playMP3Track(selected_value);
        } else if (button_released(_map, BUTTON_PAUSE)) {
            context->get_config().step_volume = selected_value;
            context->get_config().write();
            state = new_state_by_name(this, STATE_ADMIN);
        }
    } else if (menu_item == 4) {
        /* initial volume */
        if (button_released(_map, BUTTON_UP)) {
            ++selected_value;
            player->playMP3Track(selected_value);
            if (selected_value > 35)
                selected_value = 1;
        } else if (button_released(_map, BUTTON_DOWN)) {
            --selected_value;
            if (selected_value < 1)
                selected_value = 1;
            player->playMP3Track(selected_value);
        } else if (button_released(_map, BUTTON_PAUSE)) {
            context->get_config().init_volume = selected_value;
            context->get_config().write();
            state = new_state_by_name(this, STATE_ADMIN);
        }
    } else {
        menu_item = 0;
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_Menu::handle_card(RFIDCard *new_card) {
    TState *state = this;
    (void)new_card;

#if 0
    switch(card->card_mode) {
        case CARD_MODE_PLAYER:
            state = new_state_by_name(this, STATE_NEW_CARD);
            player->playMP3Track(MESSAGE_RESET_TAG);
            delay(1000); /* TODO argh don't do this */
            break;
        default:
            /* TODO restart new card at this point? */
            state = new_state_by_name(this, STATE_NEW_CARD);
            break;
    }
#endif

    if (state != this)
        delete this;

    return state;
}

TState *TState_Menu::handle_player_event(mp3_notify_event event, uint16_t code) {
    TState *state = this;

    (void)code;

    switch (event) {
        case MP3_NOTIFY_ERROR:
            /* TODO handle */
            break;
        case MP3_PLAY_FINISHED:
            /* TODO handle */
            break;
        case MP3_CARD_ONLINE:
            /* TODO should not happen */
            break;
        case MP3_CARD_INSERTED:
            /* TODO should not happen */
            break;
        case MP3_CARD_REMOVED:
            state = new_state_by_name(this, STATE_IDLE);
            break;
        default:
            break;
    }

    if (this != state)
        delete this;

    return state;
}

TState *TState_Menu::loop() {
    TState *state = this;

    notify_led->loop();

    if (!card)
        return state;

    switch (menu_item) {
    }

    if (state != this)
        delete this;

    return state;
}


TState_Menu::TState_Menu(TonUINO *context) {
    this->context = context;
}

TState_Menu::TState_Menu(TState *last_state) {
    Serial.println(F("Menu(last)"));

    from_last_state(last_state);
    notify_led->update_state(LED_STATE_MENU);

    menu_item = 0;
    selected_value = 0;

    player->playMP3Track(MESSAGE_MENU_WELCOME);
}

TState_Menu::~TState_Menu() {
}
// vim: ts=4 sw=4 et cindent
