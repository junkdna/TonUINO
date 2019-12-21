#ifndef _TSTATE_H_
#define _TSTATE_H_

#include "tonuino.h"

struct RFIDCard card;

class TState {
    protected:
        TonUINO *context;
        mp3_command last_command;

        uint16_t global_track_num;
        uint16_t global_track;
        uint16_t current_folder;
        uint16_t current_folder_track_num;
        uint16_t current_track;
        uint16_t current_volume;

    public:
        void volume_up();
        void volume_down();
        void volume_set(uint8_t vol);

        TonUINO* get_context() {return context;}
        bool is_playing() {return !digitalRead(BUSY_PIN);}

        virtual TState* handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed) = 0;
        virtual TState* handle_card(RFIDCard* card) = 0;
        virtual TState* handle_dfplay_event(mp3_notify_event event, uint16_t code) = 0;

        TState(TonUINO *context);
        TState(TState *last_state);
        TState();
        virtual ~TState();
};

class TState_Idle : public TState {
    protected:

    public:
        TState* handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed);
        TState* handle_card(RFIDCard* card);
        TState* handle_dfplay_event(mp3_notify_event event, uint16_t code);

        TState_Idle(TonUINO *context);
        TState_Idle(TState *last_state);
        ~TState_Idle();
};

class TState_Menu : public TState {
    protected:

    public:
        TState* handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed);
        TState* handle_card(RFIDCard& card);
        TState* handle_dfplay_event(mp3_notify_event event, uint16_t code);

        TState_Menu(TonUINO *context) {this->context = context;}
        TState_Menu(TState *last_state) {this->context = last_state->get_context();}
};

class TState_Album : public TState {
    protected:

    public:
        TState* handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed);
        TState* handle_card(RFIDCard& card);
        TState* handle_dfplay_event(mp3_notify_event event, uint16_t code);

};

class TState_AudioBook : public TState_Album {
    protected:

    public:
        TState* handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed);
        TState* handle_card(RFIDCard& card);
        TState* handle_dfplay_event(mp3_notify_event event, uint16_t code);

};

class TState_Album_Random : public TState {
    protected:

    public:
        TState* handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed);
        TState* handle_card(RFIDCard& card);
        TState* handle_dfplay_event(mp3_notify_event event, uint16_t code);

};

class TState_Global : public TState {
    protected:

    public:
        TState* handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed);
        TState* handle_card(RFIDCard& card);
        TState* handle_dfplay_event(mp3_notify_event event, uint16_t code);

};

class TState_Global_Random : public TState {
    protected:

    public:
        TState* handle_buttons(uint8_t pressed, uint8_t released, uint8_t long_ressed);
        TState* handle_card(RFIDCard& card);
        TState* handle_dfplay_event(mp3_notify_event event, uint16_t code);

};

#endif
// vim: ts=4 sw=4 et cindent
