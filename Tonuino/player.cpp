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

extern TonUINO tonuino;

class Mp3Notify {
public:
    static void OnError(uint16_t errorCode) {
#if 0
        Serial.println();
        Serial.print("Com Error ");
        Serial.println(errorCode);
#endif
        tonuino.notify_mp3(MP3_NOTIFY_ERROR, errorCode);
    }
    static void OnPlayFinished(DfMp3_PlaySources src, uint16_t track) {
        static uint16_t last_track;

        if (src ^ DfMp3_PlaySources_Sd)
            return;

#if 1
        Serial.print("Track beendet");
        Serial.println(track);
#endif

        /*
         * we should always get two messages with the same track
         * one for track finished and on for command finished
         */
        if (track != last_track) {
            last_track = track;
            return;
        }

        /*
         * we either get the global track number
         * or for some reason the folder track number
         * allow both.
         * This filter will prevent Advert tracks to trigger e.g. selection
         * of the next track
         */
        if (tonuino.get_player().get_current_global_track() != track &&
            tonuino.get_player().get_current_track() != track)
            return;

        tonuino.notify_mp3(MP3_PLAY_FINISHED, track);
    }

    static void OnPlaySourceOnline(DfMp3_PlaySources src) {
        if (src & DfMp3_PlaySources_Sd)
            tonuino.notify_mp3(MP3_CARD_ONLINE, 0);
    }

    static void OnPlaySourceInserted(DfMp3_PlaySources src) {
        if (src & DfMp3_PlaySources_Sd)
            tonuino.notify_mp3(MP3_CARD_INSERTED, 0);
    }

    static void OnPlaySourceRemoved(DfMp3_PlaySources src) {
        if (src & DfMp3_PlaySources_Sd)
            tonuino.notify_mp3(MP3_CARD_REMOVED, 0);
    }
};

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
}

void Player::volume_down() {
    uint8_t step = context->get_config().step_volume;

    if (current_volume < step)
        volume_set(0);
    else
        volume_set(current_volume - step);
}

void Player::volume_set(uint8_t vol) {
    uint8_t cur_vol;

    if (vol > context->get_config().max_volume)
        vol = context->get_config().max_volume;

    if (vol < context->get_config().min_volume)
        vol = context->get_config().min_volume;

    cur_vol = g_dfplayer.getVolume();
    g_dfplayer.loop();

    Serial.print(F("Set volume "));
    Serial.print(cur_vol);
    Serial.print(F(" -> "));
    Serial.println(vol);

    current_volume = vol;
    g_dfplayer.setVolume(current_volume);
    last_command = MP3_CMD_SET_VOL;

    if (state)
        state->flash_notify_led(2);
}

void Player::playMP3Track(uint16_t track) {
    g_dfplayer.playMp3FolderTrack(track);
    last_command = MP3_CMD_MP3_TRACK;
    current_folder = 0;
    current_track = track;

    for (int8_t i = 0; i < 100 && !is_playing(); i++) {
        delay(100);
        g_dfplayer.loop();
    }

    global_track = g_dfplayer.getCurrentTrack(DfMp3_PlaySource_Sd);
}

void Player::playAdvertTrack(uint16_t track) {
    g_dfplayer.playAdvertisement(track);
    last_command = MP3_CMD_ADVERT_TRACK;

    for (int8_t i = 0; i < 100 && !is_playing(); i++) {
        delay(100);
        g_dfplayer.loop();
    }

    if (state)
        state->flash_notify_led(2);
}

bool Player::playFolderTrack(uint16_t folder, uint16_t track) {
    if (current_folder_track_num < 1)
        current_folder_track_num = g_dfplayer.getFolderTrackCount(folder);

    if (!current_folder_track_num || track > current_folder_track_num)
        return false;

    Serial.print(F("FolderTrack "));
    Serial.print(folder);
    Serial.print(F(" "));
    Serial.println(track);

    last_command = MP3_CMD_FOLDER_TRACK;
    current_folder = folder;
    current_track = track;

    /* request and wait */
    g_dfplayer.playFolderTrack(folder, track);
    for (int8_t i = 0; i < 100 && !is_playing(); i++) {
        delay(100);
        g_dfplayer.loop();
    }

    /* uhoh, for some reason this did not work lets retry */
    if (!is_playing()) {
        g_dfplayer.playFolderTrack(folder, track);
        for (int8_t i = 0; i < 100 && !is_playing(); i++) {
            delay(100);
            g_dfplayer.loop();
        }
    }

    if (is_playing()) {
        global_track = g_dfplayer.getCurrentTrack(DfMp3_PlaySource_Sd);
        if (!global_track)
            global_track = g_dfplayer.getCurrentTrack(DfMp3_PlaySource_Sd);
    }

    return is_playing();
}

bool Player::playRandomTrack(uint16_t folder) {
    uint16_t track;

    if (!current_track || !current_folder || folder != current_folder) {
        current_folder_track_num = g_dfplayer.getFolderTrackCount(folder);

        /* init list to shuffle */
        for (track = 1; track <= current_folder_track_num; ++track)
            random_queue[track] = track;

        /* shuffle */
        for (track = 0; track < 100 * current_folder_track_num; ++track) {
            idx = random(0, current_folder_track_num);
            uint8_t tmp = random_queue[track % current_folder_track_num];
            random_queue[track % current_folder_track_num] = random_queue[idx];
            random_queue[idx] = tmp;
        }

        idx = 0;
    }

    if (idx >= current_folder_track_num) {
        state = new_state_by_name(state, STATE_IDLE);
        return false;
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

    for (int8_t i = 0; i < 100 && is_playing(); i++) {
        delay(100);
        g_dfplayer.loop();
    }
}

void Player::pause() {
    g_dfplayer.pause();
    last_command = MP3_CMD_PAUSE;

    for (int8_t i = 0; i < 100 && is_playing(); i++) {
        delay(100);
        g_dfplayer.loop();
    }
}

void Player::start() {
    g_dfplayer.start();
    last_command = MP3_CMD_START;

    for (int8_t i = 0; i < 100 && !is_playing(); i++) {
        delay(100);
        g_dfplayer.loop();
    }
}

bool Player::next() {
    /*
     * do not use next() command of the DFPlayer because it has some very
     * strange properties.
     */
    if (current_folder_track_num < 1)
        current_folder_track_num = g_dfplayer.getFolderTrackCount(current_folder);

    ++current_track;
    /* TODO do we want wrap-around or end of playback here */
    if (current_track > current_folder_track_num)
        current_track = 1;

    return playFolderTrack(current_folder, current_track);
}

bool Player::prev() {
    /*
     * do not use prev() command of the DFPlayer because it has some very
     * strange properties.
     */
    --current_track;
    if (current_track < 1)
        current_track = 1;

    return playFolderTrack(current_folder, current_track);
}

#if 0
TState *Player::redo_last_command()
{
    bool r;

    switch (last_command) {
    case MP3_CMD_RESET:
        spk_disable();
        g_dfplayer.reset();
        delay(200);
        state = new_state_by_name(state, STATE_IDLE);
        break;
    case MP3_CMD_SET_VOL:
        volume_set(current_volume);
        break;
    case MP3_CMD_MP3_TRACK:
        /* no redo here */
        break;
    case MP3_CMD_ADVERT_TRACK:
        /* no redo here */
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
    default:
        break;
    }
    return state;
}
#endif

TState *Player::handle_error(uint16_t code)
{
#if 0
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
    case DfMp3_Error_FileMismatch:
        if (last_command != MP3_CMD_ADVERT_TRACK) {
            state->set_error(true);
            state = new_state_by_name(state, STATE_IDLE);
        }
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
    case DfMp3_Error_General:
        /* ignored for now */
        break;
    case DfMp3_Error_Advertise:
        /* ignored for now */
        break;
    default:
        break;
    }

    return state;
#endif

    return new_state_by_name(state, STATE_IDLE);
}

TState *Player::loop() {
    if (!hp_present())
        spk_enable();
    else
        spk_disable();

    g_dfplayer.loop();

    return state;
}

void Player::set_state(TState *_state) {
    state = _state;
}

void Player::set_context(TonUINO *_tonuino) {
    context = _tonuino;
}

uint16_t Player::get_current_track() {
    return current_track;
}

uint16_t Player::get_current_folder() {
    return current_folder;
}

uint16_t Player::get_current_folder_track_num() {
    return current_folder_track_num;
}

uint16_t Player::get_current_global_track() {
    return global_track;
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
    pinMode(BUSY_PIN, INPUT);
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
