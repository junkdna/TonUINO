/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2018-2019 Thorsten Voß <voss@bummel.de>
 * Copyright 2019-2020 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include "player.h"
#include "tonuino.h"
#include "tstate.h"

#include <DFMiniMp3.h>
#include <SoftwareSerial.h>

static SoftwareSerial g_soft_uart(SOFT_UART_RX_PIN, SOFT_UART_TX_PIN);
static DFMiniMp3<SoftwareSerial, Mp3Notify> g_dfplayer(g_soft_uart);


bool Player::is_playing() {
    return !digitalRead(BUSY_PIN);
}

bool Player::hp_present() {
    return !digitalRead(HPP_PIN);
}

void Player::spk_enable() {
    digitalWrite(SPK_ENABLE_PIN, 0);
}

void Player::spk_disable() {
    digitalWrite(SPK_ENABLE_PIN, 1);
}

void Player::volume_up() {
    volume_set(current_volume + context->get_config().step_volume);
    playAdvertTrack(ADVERT_VOL_UP);
}

void Player::volume_down() {
    uint8_t step = context->get_config().step_volume;
    if (current_volume < step)
        volume_set(0);
    else
        volume_set(current_volume - step);
    playAdvertTrack(ADVERT_VOL_DOWN);
}

void Player::volume_set(uint8_t vol) {
    if (vol > context->get_config().max_volume)
        vol = context->get_config().max_volume;

    if (vol < context->get_config().min_volume)
        vol = context->get_config().min_volume;

    Serial.print(F("Set volume "));
    Serial.println(vol);
    current_volume = vol;
    g_dfplayer.setVolume(current_volume);
    last_command = MP3_CMD_SET_VOL;
    delay(200);
}

void Player::playMP3Track(uint16_t track) {
    g_dfplayer.playMp3FolderTrack(track);
    last_command = MP3_CMD_MP3_TRACK;
    current_folder = 0;
    current_track = track;
    delay(200);
}

void Player::playAdvertTrack(uint16_t track) {
    g_dfplayer.playAdvertisement(track);
    last_command = MP3_CMD_ADVERT_TRACK;
    current_folder = 0;
    current_track = track;
    delay(200);
}

bool Player::playFolderTrack(uint16_t folder, uint16_t track) {
    if (current_folder_track_num < 1)
        current_folder_track_num = g_dfplayer.getFolderTrackCount(folder);

    if (!current_folder_track_num || track > current_folder_track_num)
        return false;

    g_dfplayer.playFolderTrack(folder, track);
    last_command = MP3_CMD_FOLDER_TRACK;
    current_folder = folder;
    current_track = track;
    delay(200);

    return true;
}

bool Player::playRandomTrack(uint16_t folder) {
    uint16_t track;

    if (!current_track || !current_folder || folder != current_folder) {
        g_dfplayer.loop();
        current_folder_track_num = g_dfplayer.getFolderTrackCount(folder);

        /* init list to sorted */
        for (track = 1; track <= current_folder_track_num; ++track)
            random_queue[track] = track;

        /* shuffle */
        for (track = 0; track < 10 * current_folder_track_num; ++track) {
            idx = random(0, current_folder_track_num);
            uint8_t tmp = random_queue[track % current_folder_track_num];
            random_queue[track % current_folder_track_num] = random_queue[idx];
            random_queue[idx] = tmp;
        }

        idx = 0;
    }

    if (idx >= current_folder_track_num) {
        state = new_state_by_name(state, STATE_IDLE);
        return;
    }

    track = random_queue[idx++];

    return playFolderTrack(folder, track);
}

void Player::stop() {
    g_dfplayer.stop();
    last_command = MP3_CMD_STOP;
    current_folder = 0;
    current_track = 0;
    current_folder_track_num = 0;
    delay(200);
}

void Player::pause() {
    g_dfplayer.pause();
    last_command = MP3_CMD_PAUSE;
    delay(200);
}

void Player::start() {
    g_dfplayer.start();
    last_command = MP3_CMD_START;
    delay(200);
}

bool Player::next() {
    if (current_folder_track_num < 1)
        current_folder_track_num = g_dfplayer.getFolderTrackCount(current_folder);

    ++current_track;
    /* TODO do we want wrap-around or end of playback here */
    if (current_track > current_folder_track_num)
        current_track = 1;

    return playFolderTrack(current_folder, current_track);
}

bool Player::prev() {
    --current_track;
    if (current_track < 1)
        current_track = 1;

    return playFolderTrack(current_folder, current_track);
}

TState *Player::redo_last_command()
{
    bool r;

    switch (last_command) {
        case MP3_CMD_RESET:
            spk_disable();
            g_dfplayer.reset();
            delay(200);
            /* fallthroug */
        case MP3_CMD_NONE:
            state = new_state_by_name(state, STATE_IDLE);
            break;
        case MP3_CMD_SET_VOL:
            volume_set(current_volume);
            break;
        case MP3_CMD_MP3_TRACK:
            break;
        case MP3_CMD_ADVERT_TRACK:
            break;
        case MP3_CMD_FOLDER_TRACK:
            r = playFolderTrack(current_folder, current_track);
            if (!r)
                state = new_state_by_name(state, STATE_IDLE);
            break;
        case MP3_CMD_PAUSE:
            pause();
            break;
        case MP3_CMD_START:
            start();
            break;
        case MP3_CMD_STOP:
            stop();
            break;
        case MP3_CMD_NEXT:
            r = playFolderTrack(current_folder, current_track);
            if (!r)
                state = new_state_by_name(state, STATE_IDLE);
            break;
        case MP3_CMD_PREV:
            r = playFolderTrack(current_folder, current_track);
            if (!r)
                state = new_state_by_name(state, STATE_IDLE);
            break;
    }
    return state;
}

TState *Player::handle_error(uint16_t code)
{
    switch (code) {
        case DfMp3_Error_Sleeping:
            spk_disable();
            g_dfplayer.reset();
            delay(200);
            /* fallthrough */
        case DfMp3_Error_Busy:
            /* fallthrough */
        case DfMp3_Error_SerialWrongStack:
            /* fallthrough */
        case DfMp3_Error_CheckSumNotMatch:
            /* fallthrough */
        case DfMp3_Error_FileIndexOut:
            /* fallthrough */
        case DfMp3_Error_General:
            /* fallthrough */
        case DfMp3_Error_FileMismatch:
            state->set_error(true);
            state = new_state_by_name(state, STATE_IDLE);
            break;
        case DfMp3_Error_RxTimeout:
            /* fallthrough */
        case DfMp3_Error_PacketSize:
            /* fallthrough */
        case DfMp3_Error_PacketHeader:
            /* fallthrough */
        case DfMp3_Error_PacketChecksum:
            state = redo_last_command();
            break;
        case DfMp3_Error_Advertise:
            /* ignored for now */
            break;
        default:
            break;
    }

    return state;
}

TState *Player::loop() {
    if (!hp_present())
        spk_enable();
    else
        spk_disable();

    return state;
}

void Player::set_state(TState *_state) {
    state = _state;
}

void Player::set_context(TonUINO *_tonuino) {
    context = _tonuino;
}

const uint16_t Player::get_current_track() {
    return current_track;
}

const uint16_t Player::get_current_folder() {
    return current_folder;
}

const uint16_t Player::get_current_folder_track_num() {
    return current_folder_track_num;
}

void Player::set_current_folder(uint16_t folder) {
    if (!folder) {
        current_folder = 0;
        current_track = 0;
        current_folder_track_num = 0;
    } else {
        current_folder = folder;
        current_track = 0;
        current_folder_track_num = g_dfplayer.getFolderTrackCount(current_folder);
    }
}

void Player::setup() {
    pinMode(BUSY_PIN, INPUT_PULLUP);
    pinMode(HPP_PIN, INPUT);
    pinMode(SPK_ENABLE_PIN, OUTPUT);

    spk_disable();

    g_dfplayer.begin();
    g_dfplayer.loop();

    g_dfplayer.reset();
    g_dfplayer.loop();
}

Player::Player() {
}

Player::~Player() {
}
// vim: ts=4 sw=4 et cindent
