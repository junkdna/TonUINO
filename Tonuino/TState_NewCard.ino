/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright TODO Thorsten Voß
 * Copyright 2019 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include "buttons.h"
#include "tonuino.h"
#include "tstate.h"
#include "rfid.h"

TState *TState_NewCard::handle_buttons(uint32_t _map) {
    TState *state = this;

    switch (menu_item) {
        case 0:
            /* select folder */
            if (button_released(_map, BUTTON_UP)) {
                ++selected_value;
                playMP3Track(selected_value);
                preview = 1;
            } else if (button_released(_map, BUTTON_DOWN)) {
                --selected_value;
                playMP3Track(selected_value);
                preview = 1;
            } else if (button_long_pressed(_map, BUTTON_UP)) {
                selected_value += 10;
                playMP3Track(selected_value);
                preview = 1;
            } else if (button_long_pressed(_map, BUTTON_DOWN)) {
                selected_value -= 10;
                playMP3Track(selected_value);
                preview = 1;
            } else if (button_released(_map, BUTTON_PAUSE)) {
                menu_item = 1;
                card->extdata[1] = selected_value;
                selected_value = 0;
                stop();
                playMP3Track(MESSAGE_CARD_ASSIGNED);
            } else if (button_long_pressed(_map, BUTTON_PAUSE)) {
                state = new_state_by_name(this, STATE_IDLE);
                stop();
                playMP3Track(MESSAGE_MENU_EXIT);
            }
            break;

        case 1:
            /* select mode */
            if (button_released(_map, BUTTON_UP) || button_long_pressed(_map, BUTTON_UP)) {
                ++selected_value;
                if (selected_value > MENU_CMOD_ENTRIES)
                    selected_value = 1;
                playMP3Track(MESSAGE_CARD_ASSIGNED + selected_value);
            } else if (button_released(_map, BUTTON_DOWN) || button_long_pressed(_map, BUTTON_DOWN)) {
                --selected_value;
                if (selected_value < 1)
                    selected_value = MENU_CMOD_ENTRIES;
                playMP3Track(MESSAGE_CARD_ASSIGNED + selected_value);
            } else if (button_released(_map, BUTTON_PAUSE)) {
                stop();
                if (card)
                    card->extdata[0] = selected_value;

                if (selected_value == STATE_SINGLE) {
                    playMP3Track(MESSAGE_SELECT_FILE);
                    menu_item = 2;
                    current_folder_track_num =
                        context->get_dfplayer()->getFolderTrackCount(card->extdata[1]);
                } else if (selected_value == STATE_ADMIN) {
                    card->card_mode = CARD_MODE_ADMIN;
                    menu_item = 250;
                } else {
                    menu_item = 250;
                }

                selected_value = 0;
            } else if (button_long_pressed(_map, BUTTON_PAUSE)) {
                state = new_state_by_name(this, STATE_IDLE);
                stop();
                playMP3Track(MESSAGE_MENU_EXIT);
            }

            break;

        case 2:
            /* select track */
            if (button_released(_map, BUTTON_UP)) {
                ++selected_value;
                if (selected_value > current_folder_track_num)
                    selected_value = 1;
                playMP3Track(selected_value);
                preview = 1;
            } else if (button_released(_map, BUTTON_DOWN)) {
                --selected_value;
                if (selected_value < 1)
                    selected_value = current_folder_track_num;
                playMP3Track(selected_value);
                preview = 1;
            } else if (button_long_pressed(_map, BUTTON_UP)) {
                selected_value += 10;
                if (selected_value > current_folder_track_num)
                    selected_value = 1;
                playMP3Track(selected_value);
                preview = 1;
            } else if (button_long_pressed(_map, BUTTON_DOWN)) {
                selected_value -= 10;
                if (selected_value < 1)
                    selected_value = current_folder_track_num;
                playMP3Track(selected_value);
                preview = 1;
            } else if (button_released(_map, BUTTON_PAUSE)) {
                menu_item = 250;
                if (card)
                    card->extdata[2] = selected_value;
                selected_value = 0;
                stop();
            } else if (button_long_pressed(_map, BUTTON_PAUSE)) {
                state = new_state_by_name(this, STATE_IDLE);
                stop();
                playMP3Track(MESSAGE_MENU_EXIT);
            }

            break;

        default:
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_NewCard::handle_card(RFIDCard *new_card) {
    TState *state = this;
    this->card = new_card;

    switch(card->card_mode) {
        case CARD_MODE_PLAYER:
            state = new_state_by_name(this, STATE_NEW_CARD);
            playMP3Track(MESSAGE_RESET_TAG);
            delay(1000); /* TODO argh don't do this */
            break;
        default:
            /* TODO restart new card at this point? */
            state = new_state_by_name(this, STATE_NEW_CARD);
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_NewCard::handle_dfplay_event(mp3_notify_event event, uint16_t code) {
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

TState *TState_NewCard::loop() {
    TState *state = this;

    if (!card)
        return state;

    switch (menu_item) {
        case 0:
            if (preview && !is_playing()) {
                playFolderTrack(selected_value, 1);
                preview = 0;
            }
            break;

        case 1:
            break;

        case 2:
            if (preview && !is_playing()) {
                playFolderTrack(card->extdata[1], selected_value);
                preview = 0;
            }
            break;

        case 250:
            playMP3Track(MESSAGE_MODE_RADIO_PLAY + card->extdata[0]);
            if (!is_playing())
                ++menu_item;
            break;

        case 251:
            menu_item = 255;
            if (card->write())
                playMP3Track(MESSAGE_CARD_CONFIGURED);
            else
                playMP3Track(MESSAGE_ERROR);
            break;

        case 255:
            state = new_state_by_name(this, STATE_IDLE);
            break;
    }

    if (state != this)
        delete this;

    return state;
}


TState_NewCard::TState_NewCard(TonUINO *context) {
    this->context = context;
}

TState_NewCard::TState_NewCard(TState *last_state) {
    uint32_t magic;
    Serial.println(F("NewCard(last)"));

    from_last_state(last_state);
    menu_item = 0;
    selected_value = 0;

    magic = CARD_MAGIC;
    card->magic[0] = (magic >>  0) & 0xff;
    card->magic[1] = (magic >>  8) & 0xff;
    card->magic[2] = (magic >> 16) & 0xff;
    card->magic[3] = (magic >> 24) & 0xff;
    card->version = CARD_VERSION;
    card->card_mode = CARD_MODE_PLAYER;
    memset(card->extdata, 0, 10);
    playMP3Track(MESSAGE_NEW_CARD);
}

TState_NewCard::~TState_NewCard() {
}
// vim: ts=4 sw=4 et cindent
