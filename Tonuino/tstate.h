/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2018-2019 Thorsten Voß <voss@bummel.de>
 * Copyright 2019-2020 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#ifndef _TSTATE_H_
#define _TSTATE_H_

#include "player.h"
#include "notification_led.h"
#include "tonuino.h"

class RFIDCard;
class Modificator;

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

#define MAX_MODIFICATORS    4

class TState {
    protected:
        TonUINO *context = nullptr;
        RFIDCard *card = nullptr;
        Player *player = nullptr;
        NotificationLED *notify_led = nullptr;
        Modificator *mods[MAX_MODIFICATORS] = {nullptr};

        bool restore = false;

    protected:
        void from_last_state(TState *last_state);

    public:
        virtual TState *handle_buttons(uint32_t _map) = 0;
        virtual TState *handle_card(RFIDCard *card) = 0;
        virtual TState *handle_player_event(mp3_notify_event event, uint16_t code) = 0;
        virtual TState *loop() = 0;

        void set_restore(bool r);
        explicit TState(TonUINO *context);
        explicit TState(TState *last_state);
        TState();
        virtual ~TState();
};

TState *new_state_by_name(TState *state, uint8_t state_name, bool restore = false);

class TState_Idle : public TState {
    protected:

    public:
        TState *handle_buttons(uint32_t _map) override;
        TState *handle_card(RFIDCard *card) override;
        TState *handle_player_event(mp3_notify_event event, uint16_t code) override;
        TState *loop() override;

        explicit TState_Idle(TonUINO *context);
        explicit TState_Idle(TState *last_state);
        ~TState_Idle();
};

class TState_NewCard : public TState {
    protected:
        uint8_t menu_item = 0;
        uint8_t selected_value = 0;
        uint8_t preview = 0;

    public:
        TState *handle_buttons(uint32_t _map) override;
        TState *handle_card(RFIDCard *card) override;
        TState *handle_player_event(mp3_notify_event event, uint16_t code) override;
        TState *loop() override;

        explicit TState_NewCard(TonUINO *context);
        explicit TState_NewCard(TState *last_state);
        ~TState_NewCard();
};

class TState_Menu : public TState {
    protected:
        uint8_t menu_item = 0;
        uint8_t selected_value = 0;

    public:
        TState *handle_buttons(uint32_t _map) override;
        TState *handle_card(RFIDCard *card) override;
        TState *handle_player_event(mp3_notify_event event, uint16_t code) override;
        TState *loop() override;

        explicit TState_Menu(TonUINO *context);
        explicit TState_Menu(TState *last_state);
        ~TState_Menu();
};

class TState_Album : public TState {
    protected:

    public:
        TState *handle_buttons(uint32_t _map) override;
        TState *handle_card(RFIDCard *card) override;
        TState *handle_player_event(mp3_notify_event event, uint16_t code) override;
        TState *loop() override;

        explicit TState_Album(TonUINO *context);
        explicit TState_Album(TState *last_state);
        ~TState_Album();
};

class TState_AlbumRandom : public TState {
    protected:

    public:
        TState *handle_buttons(uint32_t _map) override;
        TState *handle_card(RFIDCard *card) override;
        TState *handle_player_event(mp3_notify_event event, uint16_t code) override;
        TState *loop() override;

        explicit TState_AlbumRandom(TonUINO *context);
        explicit TState_AlbumRandom(TState *last_state);
        ~TState_AlbumRandom();
};

class TState_AudioBook : public TState {
    protected:
        void next();
        void prev();

    public:
        TState *handle_buttons(uint32_t _map) override;
        TState *handle_card(RFIDCard *card) override;
        TState *handle_player_event(mp3_notify_event event, uint16_t code) override;
        TState *loop() override;

        explicit TState_AudioBook(TonUINO *context);
        explicit TState_AudioBook(TState *last_state);
        ~TState_AudioBook();
};

class TState_RadioPlay : public TState {
    protected:

    public:
        TState *handle_buttons(uint32_t _map) override;
        TState *handle_card(RFIDCard *card) override;
        TState *handle_player_event(mp3_notify_event event, uint16_t code) override;
        TState *loop() override;

        explicit TState_RadioPlay(TonUINO *context);
        explicit TState_RadioPlay(TState *last_state);
        ~TState_RadioPlay();
};

class TState_Single : public TState {
    protected:

    public:
        TState *handle_buttons(uint32_t _map) override;
        TState *handle_card(RFIDCard *card) override;
        TState *handle_player_event(mp3_notify_event event, uint16_t code) override;
        TState *loop() override;

        explicit TState_Single(TonUINO *context);
        explicit TState_Single(TState *last_state);
        ~TState_Single();
};

#endif
// vim: ts=4 sw=4 et cindent
