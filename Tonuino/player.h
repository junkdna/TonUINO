/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2018-2019 Thorsten Voß <voss@bummel.de>
 * Copyright 2019-2020 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#ifndef _PLAYER_H_
#define _PLAYER_H_

class TonUINO;
class TState;

enum mp3_command {
    MP3_CMD_NONE,
    MP3_CMD_RESET,
    MP3_CMD_SET_VOL,
    MP3_CMD_MP3_TRACK,
    MP3_CMD_ADVERT_TRACK,
    MP3_CMD_FOLDER_TRACK,
    MP3_CMD_PAUSE,
    MP3_CMD_START,
    MP3_CMD_STOP,
    MP3_CMD_NEXT,
    MP3_CMD_PREV,
};

enum mp3_notify_event {
    MP3_NOTIFY_ERROR,
    MP3_PLAY_FINISHED,
    MP3_CARD_ONLINE,
    MP3_CARD_INSERTED,
    MP3_CARD_REMOVED,
    MP3_USB_ONLINE,
    MP3_USB_INSERTED,
    MP3_USB_REMOVED,
};

class Player {
    protected:
        TonUINO *context = nullptr;
        TState  *state = nullptr;

        mp3_command last_command = MP3_CMD_NONE;

        uint16_t current_folder = 0;
        uint16_t current_folder_track_num = 0;
        uint16_t current_track = 0;
        uint16_t current_volume = 0;

        uint8_t idx = 0;
        uint8_t random_queue[255] = {0};

        void spk_enable();
        void spk_disable();
        bool hp_present();

    public:
        const uint16_t get_current_track();
        const uint16_t get_current_folder();
        const uint16_t get_current_folder_track_num();
        void set_current_folder(uint16_t folder);

        void playAdvertTrack(uint16_t track);
        void playMP3Track(uint16_t track);
        bool playFolderTrack(uint16_t folder, uint16_t track);
        bool playRandomTrack(uint16_t folder);
        void pause();
        void start();
        void stop();
        void next();
        void prev();
        void volume_up();
        void volume_down();
        void volume_set(uint8_t vol);

        bool is_playing();

        TState *loop();
        void setup();
        void set_state(TState *_state);
        void set_context(TonUINO *_tonuino);

        Player();
        ~Player();
};

#endif
// vim: ts=4 sw=4 et cindent
