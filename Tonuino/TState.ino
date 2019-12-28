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
    (void)pressed;

    if (released & (1 << BUTTON_UP)) {
    } else if (released & (1 << BUTTON_DOWN)) {
    } else if (long_pressed & (1 << BUTTON_UP)) {
        volume_up();
    } else if (long_pressed & (1 << BUTTON_DOWN)) {
        volume_down();
    } else if (released & (1 << BUTTON_PAUSE)) {
        if (card && card->card_mode == CARD_MODE_PLAYER) {
            state = new_state_by_name(this, card->extdata[0]);
            start();
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
            state = new_state_by_name(this, STATE_NEW_CARD);
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
}

TState_Idle::~TState_Idle() {
}


/*******************************
 * state Menu
 *******************************/
TState *TState_NewCard::handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_pressed) {
    TState *state = this;
    (void)pressed;

    switch (menu_item) {
        case 0:
            /* select folder */
            if (released & (1 << BUTTON_UP)) {
                ++selected_value;
                playMP3Track(selected_value);
                preview = 1;
            } else if (released & (1 << BUTTON_DOWN)) {
                --selected_value;
                playMP3Track(selected_value);
                preview = 1;
            } else if (long_pressed & (1 << BUTTON_UP)) {
                selected_value += 10;
                playMP3Track(selected_value);
                preview = 1;
            } else if (long_pressed & (1 << BUTTON_DOWN)) {
                selected_value -= 10;
                playMP3Track(selected_value);
                preview = 1;
            } else if (released & (1 << BUTTON_PAUSE)) {
                ++menu_item;
                card->extdata[1] = selected_value;
                selected_value = 0;
                stop();
                playMP3Track(MESSAGE_CARD_ASSIGNED);
            }
            break;
        case 1:
            /* select mode */
            if (released & (1 << BUTTON_UP)) {
                ++selected_value;
                playMP3Track(MESSAGE_CARD_ASSIGNED + selected_value);
            } else if (released & (1 << BUTTON_DOWN)) {
                --selected_value;
                playMP3Track(MESSAGE_CARD_ASSIGNED + selected_value);
            } else if (long_pressed & (1 << BUTTON_UP)) {
                selected_value += 10;
                playMP3Track(MESSAGE_CARD_ASSIGNED + selected_value);
            } else if (long_pressed & (1 << BUTTON_DOWN)) {
                selected_value -= 10;
                playMP3Track(MESSAGE_CARD_ASSIGNED + selected_value);
            } else if (released & (1 << BUTTON_PAUSE)) {
                stop();
                if (selected_value != STATE_SINGLE) {
                    menu_item = 250;
                } else {
                    ++menu_item;
                    delay(200);
                    playMP3Track(MESSAGE_SELECT_FILE);
                }
                if (card)
                    card->extdata[0] = selected_value;
                selected_value = 0;
            }
            break;
        case 2:
            /* select track */
            if (released & (1 << BUTTON_UP)) {
                ++selected_value;
                playMP3Track(selected_value);
                preview = 1;
            } else if (released & (1 << BUTTON_DOWN)) {
                --selected_value;
                playMP3Track(selected_value);
                preview = 1;
            } else if (long_pressed & (1 << BUTTON_UP)) {
                selected_value += 10;
                playMP3Track(selected_value);
                preview = 1;
            } else if (long_pressed & (1 << BUTTON_DOWN)) {
                selected_value -= 10;
                playMP3Track(selected_value);
                preview = 1;
            } else if (released & (1 << BUTTON_PAUSE)) {
                menu_item = 250;
                if (card)
                    card->extdata[3] = selected_value;
                selected_value = 0;
                stop();
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

    /* TODO do sane thing here */
    switch(card->card_mode) {
        case CARD_MODE_PLAYER:
            state = new_state_by_name(this, card->extdata[0]);
            break;
        default:
            state = new_state_by_name(this, STATE_NEW_CARD);
            break;
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_NewCard::handle_dfplay_event(mp3_notify_event event, uint16_t code) {

    return this;
}

TState *TState_NewCard::run() {
    TState *state = this;

    if (!card)
        return;

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
            if (preview) {
                playFolderTrack(card->extdata[1], selected_value);
                preview = 0;
            }
            break;

        case 250:
            ++menu_item;
            playMP3Track(MESSAGE_MODE_RADIO_PLAY + card->extdata[0]);
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
        delete state;

    return state;
}


TState_NewCard::TState_NewCard(TonUINO *context) {
}

TState_NewCard::TState_NewCard(TState *last_state) {
    uint32_t chip_id;

    from_last_state(last_state);
    menu_item = 0;
    selected_value = 0;

    chip_id = context->get_config().id;
    card->chip_id[0] = (chip_id >>  0) & 0xff;
    card->chip_id[1] = (chip_id >>  8) & 0xff;
    card->chip_id[2] = (chip_id >> 16) & 0xff;
    card->chip_id[3] = (chip_id >> 24) & 0xff;
    card->version = CARD_VERSION;
    card->card_mode = CARD_MODE_PLAYER;
    memset(card->extdata, 0, 10);
    playMP3Track(MESSAGE_NEW_CARD);
}

TState_NewCard::~TState_NewCard() {
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

void TState::playMP3Track(uint16_t track) {
    context->get_dfplayer()->playMp3FolderTrack(track);
    /* TODO handle COM Errors etc */
    delay(200);
}

void TState::playFolderTrack(uint16_t folder, uint16_t track) {
    context->get_dfplayer()->playFolderTrack(folder, track);
    /* TODO handle COM Errors etc */
    delay(200);
}

void TState::stop() {
    context->get_dfplayer()->stop();
    /* TODO handle COM Errors etc */
    delay(200);
}

void TState::pause() {
    context->get_dfplayer()->pause();
    /* TODO handle COM Errors etc */
    delay(200);
}

void TState::start() {
    context->get_dfplayer()->pause();
    /* TODO handle COM Errors etc */
    delay(200);
}

TState *new_state_by_name(TState *orig, uint8_t state_name) {
    switch (state_name) {
    case STATE_IDLE:
        return new TState_Idle(orig);
        break;
    case STATE_NEW_CARD:
        return new TState_NewCard(orig);
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
