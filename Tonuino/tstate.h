#ifndef _TSTATE_H_
#define _TSTATE_H_

#include "tonuino.h"

class RFIDCard;

#define STATE_IDLE          255
#define STATE_NEW_CARD      128
#define STATE_RADIO_PLAY    1
#define STATE_ALBUM         2
#define STATE_ALBUM_RANDOM  3
#define STATE_SINGLE        4
//#define STATE_GLOBAL
//#define STATE_GLOBAL_RANDOM
#define STATE_AUDIO_BOOK    5
#define STATE_ADMIN         6

enum mp3_command {
    MP3_CMD_NONE,
    MP3_CMD_RESET,
    MP3_CMD_SET_VOL,
    MP3_CMD_MP3_TRACK,
    MP3_CMD_FOLDER_TRACK,
    MP3_CMD_PAUSE,
    MP3_CMD_START,
    MP3_CMD_STOP,
    MP3_CMD_NEXT,
    MP3_CMD_PREV,
};


class TState {
    protected:
        TonUINO *context;
        RFIDCard *card;
        mp3_command last_command;

        uint16_t global_track_num;
        uint16_t global_track;
        uint16_t current_folder;
        uint16_t current_folder_track_num;
        uint16_t current_track;
        uint16_t current_volume;
        void from_last_state(TState *last_state);
        void playMP3Track(uint16_t track);
        void playFolderTrack(uint16_t folder, uint16_t track);
        void pause();
        void start();
        void stop();
        void next();
        void prev();

    public:
        void volume_up();
        void volume_down();
        void volume_set(uint8_t vol);

        bool is_playing() {return !digitalRead(BUSY_PIN);}

        virtual TState *handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed) = 0;
        virtual TState *handle_card(RFIDCard *card) = 0;
        virtual TState *handle_dfplay_event(mp3_notify_event event, uint16_t code) = 0;
        virtual TState *run() = 0;

        TState(TonUINO *context);
        TState(TState *last_state);
        TState();
        virtual ~TState();
};

TState *new_state_by_name(uint8_t state_name);

class TState_Idle : public TState {
    protected:

    public:
        TState *handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed);
        TState *handle_card(RFIDCard *card);
        TState *handle_dfplay_event(mp3_notify_event event, uint16_t code);
        TState *run();

        TState_Idle(TonUINO *context);
        TState_Idle(TState *last_state);
        ~TState_Idle();
};

class TState_NewCard : public TState {
    protected:
        uint8_t menu_item;
        uint8_t selected_value;
        uint8_t preview;

    public:
        TState *handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed);
        TState *handle_card(RFIDCard *card);
        TState *handle_dfplay_event(mp3_notify_event event, uint16_t code);
        TState *run();

        TState_NewCard(TonUINO *context);
        TState_NewCard(TState *last_state);
        ~TState_NewCard();
};

class TState_Album : public TState {
    protected:

    public:
        TState *handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed);
        TState *handle_card(RFIDCard *card);
        TState *handle_dfplay_event(mp3_notify_event event, uint16_t code);
        TState *run();

        TState_Album(TonUINO *context);
        TState_Album(TState *last_state);
        ~TState_Album();
};

class TState_Album_Random : public TState {
    protected:

    public:
        TState *handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed);
        TState *handle_card(RFIDCard *card);
        TState *handle_dfplay_event(mp3_notify_event event, uint16_t code);
        TState *run();

        TState_Album_Random(TonUINO *context);
        TState_Album_Random(TState *last_state);
        ~TState_Album_Random();
};

class TState_AudioBook : public TState {
    protected:

    public:
        TState *handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed);
        TState *handle_card(RFIDCard *card);
        TState *handle_dfplay_event(mp3_notify_event event, uint16_t code);
        TState *run();

        TState_AudioBook(TonUINO *context);
        TState_AudioBook(TState *last_state);
        ~TState_AudioBook();
};

class TState_RadioPlay : public TState {
    protected:

    public:
        TState *handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed);
        TState *handle_card(RFIDCard *card);
        TState *handle_dfplay_event(mp3_notify_event event, uint16_t code);
        TState *run();

        TState_RadioPlay(TonUINO *context);
        TState_RadioPlay(TState *last_state);
        ~TState_RadioPlay();
};

class TState_Single : public TState {
    protected:

    public:
        TState *handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed);
        TState *handle_card(RFIDCard *card);
        TState *handle_dfplay_event(mp3_notify_event event, uint16_t code);
        TState *run();

        TState_Single(TonUINO *context);
        TState_Single(TState *last_state);
        ~TState_Single();
};

#if 0
class TState_Global : public TState {
    protected:

    public:
        TState *handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed);
        TState *handle_card(RFIDCard *card);
        TState *handle_dfplay_event(mp3_notify_event event, uint16_t code);
        TState *run();

        TState_Global(TonUINO *context);
        TState_Global(TState *last_state);
        ~TState_Global();
};

class TState_Global_Random : public TState {
    protected:

    public:
        TState *handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed);
        TState *handle_card(RFIDCard *card);
        TState *handle_dfplay_event(mp3_notify_event event, uint16_t code);
        TState *run();

        TState_Global_Random(TonUINO *context);
        TState_Global_Random(TState *last_state);
        ~TState_Global_Random();
};
#endif

#endif
// vim: ts=4 sw=4 et cindent
