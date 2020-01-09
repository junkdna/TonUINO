/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2018-2019 Thorsten Voß <voss@bummel.de>
 * Copyright 2019-2020 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include "buttons.h"
#include "tonuino.h"
#include "tstate.h"
#include "rfid.h"


/* TODO add option to clean some variables */
void TState::from_last_state(TState *last_state) {
    /* TODO do we always want to delete all mods here or should some stay in place
     * maybe add a marker?
     * if (!mods[i] || mods[i]->marker)
     *     continue;
     */
    for (int8_t i = 0; i < MAX_MODIFICATORS; i++) {
        if (!mods[i])
            continue;
        delete mods[i];
        mods[i] = nullptr;
    }

    this->card                     = last_state->card;
    this->context                  = last_state->context;
    this->current_folder           = last_state->current_folder;
    this->current_folder_track_num = last_state->current_folder_track_num;
    this->current_track            = last_state->current_track;
    this->current_volume           = last_state->current_volume;
    this->last_command             = last_state->last_command;
    this->restore                  = last_state->restore;
}

void TState::volume_up() {
    volume_set(current_volume + context->get_config().step_volume);
}

void TState::volume_down() {
    uint8_t step = context->get_config().step_volume;
    if (current_volume < step)
        volume_set(0);
    else
        volume_set(current_volume - step);
}

void TState::volume_set(uint8_t vol) {
    if (vol > context->get_config().max_volume)
        vol = context->get_config().max_volume;

    if (vol < context->get_config().min_volume)
        vol = context->get_config().min_volume;

    Serial.print(F("Set volume "));
    Serial.println(vol);
    current_volume = vol;
    context->get_dfplayer()->setVolume(current_volume);
    last_command = MP3_CMD_SET_VOL;
    delay(200);
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
    if (current_folder_track_num < 1)
        current_folder_track_num = context->get_dfplayer()->getFolderTrackCount(folder);

    context->get_dfplayer()->playFolderTrack(folder, track);
    last_command = MP3_CMD_FOLDER_TRACK;
    current_folder = folder;
    current_track = track;
    /* TODO handle COM Errors etc */
    delay(200);
}

void TState::playRandomTrack(uint16_t folder) {
    uint16_t track;

    context->get_dfplayer()->loop();
    current_folder_track_num = context->get_dfplayer()->getFolderTrackCount(folder);

    if (!current_folder_track_num)
        track = 1;
    else
        track = random(1, current_folder_track_num + 1);

    if (track == current_track)
        ++track;

    playFolderTrack(folder, track);
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
    if (current_folder_track_num < 1)
        current_folder_track_num =
            context->get_dfplayer()->getFolderTrackCount(current_folder);

    ++current_track;
    /* TODO do we want wrap-around or end of playback here */
    if (current_track > current_folder_track_num)
        current_track = 1;

    playFolderTrack(current_folder, current_track);
    last_command = MP3_CMD_NEXT;
    /* TODO handle COM Errors etc */
    delay(200);
}

void TState::prev() {
    --current_track;
    if (current_track < 1)
        current_track = 1;

    playFolderTrack(current_folder, current_track);
    last_command = MP3_CMD_PREV;
    /* TODO handle COM Errors etc */
    delay(200);
}

TState *new_state_by_name(TState *orig, uint8_t state_name, bool restore = false) {
    orig->set_restore(restore);

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
        return new TState_AlbumRandom(orig);
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
    case STATE_ADMIN:
        return new TState_Menu(orig);
        break;
    default:
        return orig;
    }
}

void TState::set_restore(bool r) {
    restore = r;
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
