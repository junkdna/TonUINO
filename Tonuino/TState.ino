/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright TODO Thorsten Voß
 * Copyright 2019 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include "tonuino.h"
#include "tstate.h"
#include "rfid.h"

/*******************************
 * state Idle
 *******************************/
TState *TState_Idle::handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_pressed) {
    TState *state = this;

    if (released & (1 << BUTTON_UP)) {
    } else if (released & (1 << BUTTON_DOWN)) {
    } else if (long_pressed & (1 << BUTTON_UP)) {
        volume_up();
    } else if (long_pressed & (1 << BUTTON_DOWN)) {
        volume_down();
    } else if (released & (1 << BUTTON_PAUSE)) {
        if (card && card->card_mode == CARD_MODE_PLAYER) {
            state = new_state_by_name(this, card->extdata[0]);
            context->get_dfplayer()->start();
        }
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_Idle::handle_card(RFIDCard *card) {
    TState *state = this;
    this->card = card;

    switch(card->card_mode) {
        case CARD_MODE_PLAYER:
            state = new_state_by_name(this, card->extdata[0]);
            break;
        default:
            state = new_state_by_name(this, STATE_MENU);
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_Idle::handle_dfplay_event(mp3_notify_event event, uint16_t code) {
    return this;
}

TState *TState_Idle::run() {
    return this;
}

TState_Idle::TState_Idle(TonUINO *context) {
    this->context = context;
    current_volume = INITIAL_VOLUME;
}

TState_Idle::TState_Idle(TState *last_state) {
    from_last_state(last_state);
    Serial.println(F("idle(last_state)"));
}

TState_Idle::~TState_Idle() {
}


/*******************************
 * state Menu
 *******************************/
TState *TState_Menu::handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_pressed) {
    TState *state = this;

    switch (sub_menu) {
        case NEW_CARD:
            switch (sub_menu_item) {
                case 0:
                    /* select folder */
                    if (released & (1 << BUTTON_UP)) {
                        ++selected_value;
                        play(selected_value);
                        preview = 1;
                    } else if (released & (1 << BUTTON_DOWN)) {
                        --selected_value;
                        play(selected_value);
                        preview = 1;
                    } else if (long_pressed & (1 << BUTTON_UP)) {
                        selected_value += 10;
                        play(selected_value);
                        preview = 1;
                    } else if (long_pressed & (1 << BUTTON_DOWN)) {
                        selected_value -= 10;
                        play(selected_value);
                        preview = 1;
                    } else if (released & (1 << BUTTON_PAUSE)) {
                        ++sub_menu_item;
                        card->extdata[1] = selected_value;
                        selected_value = 0;
                        context->get_dfplayer()->stop();
                        delay(200);
                        play(MESSAGE_CARD_ASSIGNED);
                    }
                    break;
                case 1:
                    /* select mode */
                    if (released & (1 << BUTTON_UP)) {
                        ++selected_value;
                        play(MESSAGE_MODE_RADIO_PLAY + selected_value);
                    } else if (released & (1 << BUTTON_DOWN)) {
                        --selected_value;
                        play(MESSAGE_MODE_RADIO_PLAY + selected_value);
                    } else if (long_pressed & (1 << BUTTON_UP)) {
                        selected_value += 10;
                        play(MESSAGE_MODE_RADIO_PLAY + selected_value);
                    } else if (long_pressed & (1 << BUTTON_DOWN)) {
                        selected_value -= 10;
                        play(MESSAGE_MODE_RADIO_PLAY + selected_value);
                    } else if (released & (1 << BUTTON_PAUSE)) {
                        context->get_dfplayer()->stop();
                        if (selected_value != STATE_SINGLE) {
                            sub_menu_item = 254;
                        } else {
                            ++sub_menu_item;
                            delay(200);
                            play(MESSAGE_SELECT_FILE);
                        }
                        card->extdata[0] = selected_value;
                        selected_value = 0;
                    }
                    break;
                case 2:
                    /* select track */
                    if (released & (1 << BUTTON_UP)) {
                        ++selected_value;
                        play(MESSAGE_MODE_RADIO_PLAY + selected_value);
                        preview = 1;
                    } else if (released & (1 << BUTTON_DOWN)) {
                        --selected_value;
                        play(MESSAGE_MODE_RADIO_PLAY + selected_value);
                        preview = 1;
                    } else if (long_pressed & (1 << BUTTON_UP)) {
                        selected_value += 10;
                        play(MESSAGE_MODE_RADIO_PLAY + selected_value);
                        preview = 1;
                    } else if (long_pressed & (1 << BUTTON_DOWN)) {
                        selected_value -= 10;
                        play(MESSAGE_MODE_RADIO_PLAY + selected_value);
                        preview = 1;
                    } else if (released & (1 << BUTTON_PAUSE)) {
                        sub_menu_item = 254;
                        card->extdata[3] = selected_value;
                        selected_value = 0;
                        context->get_dfplayer()->stop();
                    }
                    break;
                case 254:
                    play(MESSAGE_MODE_RADIO_PLAY + selected_value);
                    sub_menu_item = 255;
                    card->write();
                    delay(200);
                    /* Fallthrough */
                default:
                    state = new_state_by_name(this, STATE_IDLE);
                    break;
            }
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_Menu::handle_card(RFIDCard *card) {
    TState *state = this;

    switch (sub_menu) {
        case NEW_CARD:
            break;
    }

    this->card = card;
    switch(card->card_mode) {
        case CARD_MODE_PLAYER:
            state = new_state_by_name(this, card->extdata[0]);
            break;
        default:
            state = new_state_by_name(this, STATE_MENU);
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_Menu::handle_dfplay_event(mp3_notify_event event, uint16_t code) {
    switch (sub_menu) {
        case NEW_CARD:
            break;
    }

    return this;
}

TState *TState_Menu::run() {
    TState *state = this;

    switch (sub_menu) {
        case NEW_CARD:
            if (!is_playing()) {
                if (preview) {
                    if (sub_menu == 2)
                        context->get_dfplayer()->playFolderTrack(card->extdata[1], selected_value);
                    else if (sub_menu == 0)
                        context->get_dfplayer()->playFolderTrack(selected_value, 1);
                    preview = 0;
                }
            }
            if (sub_menu_item == 255)
                state = new_state_by_name(this, STATE_IDLE);
            break;
    }

    if (state != this)
        delete state;

    return state;
}


TState_Menu::TState_Menu(TonUINO *context) {
}

TState_Menu::TState_Menu(TState *last_state) {
    uint32_t chip_id;
    Serial.println(F("menu(last_state)"));

    from_last_state(last_state);
    sub_menu = NEW_CARD;
    sub_menu_item = 0;
    selected_value = 0;

    switch (sub_menu) {
        case NEW_CARD:
            chip_id = context->get_config().id;
            card->chip_id[0] = (chip_id >>  0) & 0xff;
            card->chip_id[1] = (chip_id >>  8) & 0xff;
            card->chip_id[2] = (chip_id >> 16) & 0xff;
            card->chip_id[3] = (chip_id >> 24) & 0xff;
            card->version = CARD_VERSION;
            card->card_mode = CARD_MODE_PLAYER;
            memset(card->extdata, 0, 10);
            play(MESSAGE_NEW_CARD);
            break;
    }
}

TState_Menu::~TState_Menu() {
}


/*******************************
 * state Album
 *******************************/
TState *TState_Album::handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_pressed) {
    TState *state = this;

    if (released & (1 << BUTTON_UP)) {
        context->get_dfplayer()->nextTrack();
    } else if (released & (1 << BUTTON_DOWN)) {
        context->get_dfplayer()->prevTrack();
    } else if (long_pressed & (1 << BUTTON_UP)) {
        volume_up();
    } else if (long_pressed & (1 << BUTTON_DOWN)) {
        volume_down();
    } else if (released & (1 << BUTTON_PAUSE)) {
        state = new_state_by_name(this, STATE_IDLE);
        context->get_dfplayer()->pause();
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_Album::handle_card(RFIDCard *card) {
    TState *state = this;
    this->card = card;

    switch(card->card_mode) {
        case CARD_MODE_PLAYER:
            state = new_state_by_name(this, card->extdata[0]);
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_Album::handle_dfplay_event(mp3_notify_event event, uint16_t code) {
    return this;
}

TState *TState_Album::run() {
    return this;
}

TState_Album::TState_Album(TonUINO *context) {
}

TState_Album::TState_Album(TState *last_state) {
    from_last_state(last_state);
    Serial.println(F("album(last_state)"));
}

TState_Album::~TState_Album() {
}


/*******************************
 * state AudioBook
 *******************************/
TState *TState_AudioBook::handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_pressed) {
    return this;
}

TState *TState_AudioBook::handle_card(RFIDCard *card) {
    TState *state = this;
    this->card = card;

    switch(card->card_mode) {
        case CARD_MODE_PLAYER:
            state = new_state_by_name(this, card->extdata[0]);
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_AudioBook::handle_dfplay_event(mp3_notify_event event, uint16_t code) {
    return this;
}

TState *TState_AudioBook::run() {
    return this;
}

TState_AudioBook::TState_AudioBook(TonUINO *context) {
}

TState_AudioBook::TState_AudioBook(TState *last_state) {
    from_last_state(last_state);
}

TState_AudioBook::~TState_AudioBook() {
}


/*******************************
 * state RadioPlay
 *******************************/
TState *TState_RadioPlay::handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_pressed) {
    return this;
}

TState *TState_RadioPlay::handle_card(RFIDCard *card) {
    TState *state = this;
    this->card = card;

    switch(card->card_mode) {
        case CARD_MODE_PLAYER:
            state = new_state_by_name(this, card->extdata[0]);
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_RadioPlay::handle_dfplay_event(mp3_notify_event event, uint16_t code) {
    return this;
}

TState *TState_RadioPlay::run() {
    return this;
}

TState_RadioPlay::TState_RadioPlay(TonUINO *context) {
}

TState_RadioPlay::TState_RadioPlay(TState *last_state) {
    from_last_state(last_state);
}

TState_RadioPlay::~TState_RadioPlay() {
}


/*******************************
 * state Single
 *******************************/
TState *TState_Single::handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_pressed) {
    return this;
}

TState *TState_Single::handle_card(RFIDCard *card) {
    TState *state = this;
    this->card = card;

    switch(card->card_mode) {
        case CARD_MODE_PLAYER:
            state = new_state_by_name(this, card->extdata[0]);
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_Single::handle_dfplay_event(mp3_notify_event event, uint16_t code) {
    return this;
}

TState *TState_Single::run() {
    return this;
}

TState_Single::TState_Single(TonUINO *context) {
}

TState_Single::TState_Single(TState *last_state) {
    from_last_state(last_state);
}

TState_Single::~TState_Single() {
}


/*******************************
 * state Album_Random
 *******************************/
TState *TState_Album_Random::handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_pressed) {
    return this;
}

TState *TState_Album_Random::handle_card(RFIDCard *card) {
    TState *state = this;
    this->card = card;

    switch(card->card_mode) {
        case CARD_MODE_PLAYER:
            state = new_state_by_name(this, card->extdata[0]);
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_Album_Random::handle_dfplay_event(mp3_notify_event event, uint16_t code) {
    return this;
}

TState *TState_Album_Random::run() {
    return this;
}

TState_Album_Random::TState_Album_Random(TonUINO *context) {
}

TState_Album_Random::TState_Album_Random(TState *last_state) {
    from_last_state(last_state);
}

TState_Album_Random::~TState_Album_Random() {
}


#if 0
/*******************************
 * state Global
 *******************************/
TState *TState_Global::handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_pressed) {
    return this;
}

TState *TState_Global::handle_card(RFIDCard *card) {
    TState *state = this;
    this->card = card;

    switch(card->card_mode) {
        case CARD_MODE_PLAYER:
            state = new_state_by_name(this, card->extdata[0]);
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_Global::handle_dfplay_event(mp3_notify_event event, uint16_t code) {
    return this;
}

TState *TState_Global::run() {
    return this;
}

TState_Global::TState_Global(TonUINO *context) {
}

TState_Global::TState_Global(TState *last_state) {
    from_last_state(last_state);
}

TState_Global::~TState_Global() {
}


/*******************************
 * state Global_Random
 *******************************/
TState *TState_Global_Random::handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_pressed) {
    return this;
}

TState *TState_Global_Random::handle_card(RFIDCard *card) {
    TState *state = this;
    this->card = card;

    switch(card->card_mode) {
        case CARD_MODE_PLAYER:
            state = new_state_by_name(this, card->extdata[0]);
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_Global_Random::handle_dfplay_event(mp3_notify_event event, uint16_t code) {
    return this;
}

TState *TState_Global_Random::run() {
    return this;
}

TState_Global_Random::TState_Global_Random(TonUINO *context) {
}

TState_Global_Random::TState_Global_Random(TState *last_state) {
    from_last_state(last_state);
}

TState_Global_Random::~TState_Global_Random() {
}
#endif

/*******************************
 * base class
 *******************************/
void TState::from_last_state(TState *last_state) {
        this->card                     = last_state->card;
        this->context                  = last_state->context;
        this->current_folder           = last_state->current_folder;
        this->current_folder_track_num = last_state->current_folder_track_num;
        this->current_track            = last_state->current_track;
        this->current_volume           = last_state->current_volume;
        this->global_track             = last_state->global_track;
        this->global_track_num         = last_state->global_track_num;
        this->last_command             = last_state->last_command;
}

void TState::volume_up() {
    volume_set(current_volume + 1);
}

void TState::volume_down() {
    volume_set(current_volume - 1);
}

void TState::volume_set(uint8_t vol) {
    if (vol > MAX_VOLUME && vol < MIN_VOLUME)
        return;

    current_volume = vol;
    context->get_dfplayer()->setVolume(current_volume);
    last_command = MP3_CMD_SET_VOL;
}

void TState::play(uint16_t track) {
    context->get_dfplayer()->playMp3FolderTrack(track);
    delay(200);
}

TState *new_state_by_name(TState *orig, uint8_t state_name) {
    switch (state_name) {
    case STATE_IDLE:
        return new TState_Idle(orig);
        break;
    case STATE_MENU:
        return new TState_Menu(orig);
        break;
    case STATE_ALBUM:
        return new TState_Album(orig);
        break;
    case STATE_ALBUM_RANDOM:
        return new TState_Album_Random(orig);
        break;
    case STATE_AUDIO_BOOK:
        return new TState_AudioBook(orig);
        break;
    case STATE_RADIO_PLAY:
        return new TState_RadioPlay(orig);
        break;
    case STATE_SINGLE:
        return new TState_Single(orig);
        break;
#if 0
    case STATE_GLOBAL:
        return new TState_Global(orig);
        break;
    case STATE_GLOBAL_RANDOM:
        return new TState_Global_Random(orig);
        break;
#endif
    default:
        return orig;
    }
}

TState::TState() {
}

TState::TState(TState *last_state) {
    from_last_state(last_state);
}

TState::TState(TonUINO *context) {
    this->context = context;
}

TState::~TState() {
}

// vim: ts=4 sw=4 et cindent
