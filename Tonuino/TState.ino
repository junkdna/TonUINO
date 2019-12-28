/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright TODO Thorsten Voß
 * Copyright 2019 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include "buttons.h"
#include "tonuino.h"
#include "tstate.h"
#include "rfid.h"

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

/*******************************
 * state Idle
 *******************************/
TState *TState_Idle::handle_buttons(uint32_t _map) {
    TState *state = this;

    if (button_vol_up(_map)) {
        volume_up();
    } else if (button_vol_down(_map)) {
        volume_down();
    } else if (button_released(_map, BUTTON_PAUSE)) {
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
    (void)code;

    switch (event) {
        case MP3_NOTIFY_ERROR:
            /* TODO handle */
            break;
        case MP3_PLAY_FINISHED:
            /* TODO should not be playing in idle state right, maybe second finish
               after last track */
            break;
        case MP3_CARD_ONLINE:
            /* TODO make helper */
            break;
        case MP3_CARD_INSERTED:
            break;
        case MP3_CARD_REMOVED:
            current_track = 0;
            current_folder = 0;
            current_folder_track_num = 0;
            break;
        default:
            break;
    }

    return this;
}

TState *TState_Idle::loop() {
    return this;
}

/* this function is called in case we have no previous state */
TState_Idle::TState_Idle(TonUINO *context) {
    this->context = context;
}

TState_Idle::TState_Idle(TState *last_state) {
    from_last_state(last_state);
}

TState_Idle::~TState_Idle() {
}


/*******************************
 * state Menu
 *******************************/
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
                ++menu_item;
                card->extdata[1] = selected_value;
                selected_value = 0;
                stop();
                playMP3Track(MESSAGE_CARD_ASSIGNED);
            }
            break;
        case 1:
            /* select mode */
            if (button_released(_map, BUTTON_UP)) {
                ++selected_value;
                playMP3Track(MESSAGE_CARD_ASSIGNED + selected_value);
            } else if (button_released(_map, BUTTON_DOWN)) {
                --selected_value;
                playMP3Track(MESSAGE_CARD_ASSIGNED + selected_value);
            } else if (button_long_pressed(_map, BUTTON_UP)) {
                selected_value += 10;
                playMP3Track(MESSAGE_CARD_ASSIGNED + selected_value);
            } else if (button_long_pressed(_map, BUTTON_DOWN)) {
                selected_value -= 10;
                playMP3Track(MESSAGE_CARD_ASSIGNED + selected_value);
            } else if (button_released(_map, BUTTON_PAUSE)) {
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
                menu_item = 250;
                if (card)
                    card->extdata[2] = selected_value;
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
        delete this;

    return state;
}


TState_NewCard::TState_NewCard(TonUINO *context) {
    this->context = context;
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
TState *TState_Album::handle_buttons(uint32_t _map) {
    TState *state = this;

    if (button_next(_map)) {
        next();
    } else if (button_prev(_map)) {
        prev();
    } else if (button_vol_up(_map)) {
        volume_up();
    } else if (button_vol_down(_map)) {
        volume_down();
    } else if (button_released(_map, BUTTON_PAUSE)) {
        pause();
        state = new_state_by_name(this, STATE_IDLE);
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_Album::handle_card(RFIDCard *card) {
    TState *state = this;
    this->card = card;

    /* do not handle empty cards */
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
    TState *state = this;

    (void)code;

    switch (event) {
        case MP3_NOTIFY_ERROR:
            /* TODO handle */
            break;
        case MP3_PLAY_FINISHED:
            next();
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

TState *TState_Album::loop() {
    return this;
}

TState_Album::TState_Album(TonUINO *context) {
    this->context = context;
}

TState_Album::TState_Album(TState *last_state) {
    from_last_state(last_state);
}

TState_Album::~TState_Album() {
}


/*******************************
 * state AudioBook
 *******************************/
void TState_AudioBook::next() {
    TState::next();
    /* TODO add option to save progress on RFID card */
    EEPROM.write(EEPROM_CFG_LEN + current_folder, current_track);
}

void TState_AudioBook::prev() {
    TState::prev();
    /* TODO add option to save progress on RFID card */
    EEPROM.write(EEPROM_CFG_LEN + current_folder, current_track);
}

TState *TState_AudioBook::handle_buttons(uint32_t _map) {
    TState *state = this;

    if (button_next(_map)) {
        next();
    } else if (button_prev(_map)) {
        prev();
    } else if (button_vol_up(_map)) {
        volume_up();
    } else if (button_vol_down(_map)) {
        volume_down();
    } else if (button_released(_map, BUTTON_PAUSE)) {
        pause();
        state = new_state_by_name(this, STATE_IDLE);
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_AudioBook::handle_card(RFIDCard *card) {
    TState *state = this;
    this->card = card;

    /* do not handle empty cards */
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
    TState *state = this;

    (void)code;

    switch (event) {
        case MP3_NOTIFY_ERROR:
            /* TODO handle */
            break;
        case MP3_PLAY_FINISHED:
            next();
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

TState *TState_AudioBook::loop() {
    return this;
}

TState_AudioBook::TState_AudioBook(TonUINO *context) {
    this->context = context;
}

TState_AudioBook::TState_AudioBook(TState *last_state) {
    from_last_state(last_state);
}

TState_AudioBook::~TState_AudioBook() {
}


/*******************************
 * state RadioPlay
 *******************************/
TState *TState_RadioPlay::handle_buttons(uint32_t _map) {
    TState *state = this;

    if (button_next(_map)) {
        /* ignore next button */
    } else if (button_prev(_map)) {
        playFolderTrack(current_folder, current_track);
    } else if (button_vol_up(_map)) {
        volume_up();
    } else if (button_vol_down(_map)) {
        volume_down();
    } else if (button_released(_map, BUTTON_PAUSE)) {
        pause();
        state = new_state_by_name(this, STATE_IDLE);
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_RadioPlay::handle_card(RFIDCard *card) {
    TState *state = this;
    this->card = card;

    /* do not handle empty cards */
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
    TState *state = this;

    (void)code;

    switch (event) {
        case MP3_NOTIFY_ERROR:
            /* TODO handle */
            break;
        case MP3_PLAY_FINISHED:
            /* do not play another track, goto idle state instead */
            state = new_state_by_name(this, STATE_IDLE);
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

TState *TState_RadioPlay::loop() {
    return this;
}

TState_RadioPlay::TState_RadioPlay(TonUINO *context) {
    this->context = context;
}

TState_RadioPlay::TState_RadioPlay(TState *last_state) {
    from_last_state(last_state);
}

TState_RadioPlay::~TState_RadioPlay() {
}


/*******************************
 * state Single
 *******************************/
TState *TState_Single::handle_buttons(uint32_t _map) {
    TState *state = this;

    if (button_next(_map)) {
        /* ignore next button */
    } else if (button_prev(_map)) {
        playFolderTrack(current_folder, current_track);
    } else if (button_vol_up(_map)) {
        volume_up();
    } else if (button_vol_down(_map)) {
        volume_down();
    } else if (button_released(_map, BUTTON_PAUSE)) {
        pause();
        state = new_state_by_name(this, STATE_IDLE);
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_Single::handle_card(RFIDCard *card) {
    TState *state = this;
    this->card = card;

    /* do not handle empty cards */
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
    TState *state = this;

    (void)code;

    switch (event) {
        case MP3_NOTIFY_ERROR:
            /* TODO handle */
            break;
        case MP3_PLAY_FINISHED:
            /* do not play another track goto idle state instead */
            state = new_state_by_name(this, STATE_IDLE);
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

TState *TState_Single::loop() {
    return this;
}

TState_Single::TState_Single(TonUINO *context) {
    this->context = context;
}

TState_Single::TState_Single(TState *last_state) {
    from_last_state(last_state);
}

TState_Single::~TState_Single() {
}


/*******************************
 * state Album_Random
 *******************************/
TState *TState_Album_Random::handle_buttons(uint32_t _map) {
    TState *state = this;

    if (button_next(_map)) {
        playRandomTrack(current_folder);
    } else if (button_prev(_map)) {
        playFolderTrack(current_folder, current_track);
    } else if (button_vol_up(_map)) {
        volume_up();
    } else if (button_vol_down(_map)) {
        volume_down();
    } else if (button_released(_map, BUTTON_PAUSE)) {
        pause();
        state = new_state_by_name(this, STATE_IDLE);
    }

    if (state != this)
        delete this;

    return state;
}

TState *TState_Album_Random::handle_card(RFIDCard *card) {
    TState *state = this;
    this->card = card;

    /* do not handle empty cards */
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
    TState *state = this;

    (void)code;

    switch (event) {
        case MP3_NOTIFY_ERROR:
            /* TODO handle */
            break;
        case MP3_PLAY_FINISHED:
            playRandomTrack(current_folder);
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

TState *TState_Album_Random::loop() {
    return this;
}

TState_Album_Random::TState_Album_Random(TonUINO *context) {
    this->context = context;
}

TState_Album_Random::TState_Album_Random(TState *last_state) {
    from_last_state(last_state);
}

TState_Album_Random::~TState_Album_Random() {
}

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
        this->last_command             = last_state->last_command;
}

void TState::volume_up() {
    volume_set(current_volume + 1);
}

void TState::volume_down() {
    volume_set(current_volume - 1);
}

void TState::volume_set(uint8_t vol) {
    if (vol > context->get_config().max_volume && vol < context->get_config().min_volume) {
        Serial.print(vol);
        Serial.println(F(" is out of configured range"));
        return;
    }

    Serial.print(F("Setting new volume "));
    Serial.print(vol);

    current_volume = vol;
    context->get_dfplayer()->setVolume(current_volume);
    delay(200);
    last_command = MP3_CMD_SET_VOL;

    Serial.println(F(" ... done"));
}

void TState::playMP3Track(uint16_t track) {
    context->get_dfplayer()->playMp3FolderTrack(track);
    last_command = MP3_CMD_MP3_TRACK;
    current_folder = 0;
    current_track = track;
    /* TODO handle COM Errors etc */
    delay(200);
}

void TState::playAdvertTrack(uint16_t track) {
    context->get_dfplayer()->playAdvertisement(track);
    last_command = MP3_CMD_ADVERT_TRACK;
    current_folder = 0;
    current_track = track;
    /* TODO handle COM Errors etc */
    delay(200);
}

void TState::playFolderTrack(uint16_t folder, uint16_t track) {
    context->get_dfplayer()->playFolderTrack(folder, track);
    last_command = MP3_CMD_FOLDER_TRACK;
    current_folder = folder;
    current_track = track;
    /* TODO handle COM Errors etc */
    delay(200);
}

void TState::playRandomTrack(uint16_t folder) {
    uint16_t track;

    if (!current_folder_track_num)
        current_folder_track_num = context->get_dfplayer()->getFolderTrackCount(folder);

    track = random(1, current_folder_track_num + 1);
    if (track == current_track)
        ++track;

    context->get_dfplayer()->playFolderTrack(folder, track);
    last_command = MP3_CMD_FOLDER_TRACK;
    current_folder = folder;
    current_track = track;
    /* TODO handle COM Errors etc */
    delay(200);
}

void TState::stop() {
    context->get_dfplayer()->stop();
    last_command = MP3_CMD_STOP;
    current_folder = 0;
    current_track = 0;
    current_folder_track_num = 0;
    /* TODO handle COM Errors etc */
    delay(200);
}

void TState::pause() {
    context->get_dfplayer()->pause();
    last_command = MP3_CMD_PAUSE;
    /* TODO handle COM Errors etc */
    delay(200);
}

void TState::start() {
    context->get_dfplayer()->start();
    last_command = MP3_CMD_START;
    /* TODO handle COM Errors etc */
    delay(200);
}

void TState::next() {
    ++current_track;
    /* TODO do we want wrap-around or end of playback here */
#if 1
    current_track %= current_folder_track_num;
#else
    if (current_track > current_folder_track_num)
        return;
#endif

    context->get_dfplayer()->nextTrack();
    last_command = MP3_CMD_NEXT;
    /* TODO handle COM Errors etc */
    delay(200);
}

void TState::prev() {
    --current_track;
    current_track %= current_folder_track_num;
    context->get_dfplayer()->prevTrack();
    last_command = MP3_CMD_PREV;
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
