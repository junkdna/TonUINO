/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright 2019-2020 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#define ADVERT_VOL_DOWN         100
#define ADVERT_VOL_UP           101

#define MESSAGE_NEW_CARD        300
#define MESSAGE_CARD_ASSIGNED   310
#define MESSAGE_MODE_RADIO_PLAY 311
#define MESSAGE_MODE_ALBUM      312
#define MESSAGE_MODE_PARTY      313
#define MESSAGE_MODE_SINGLE     314
#define MESSAGE_MODE_AUDIOBOOK  315
#define MESSAGE_MODE_ADMIN      316
#define MESSAGE_MODE_MOD        317
#define MESSAGE_SELECT_FILE     320
#define MESSAGE_MENU_WELCOME    330
#define MESSAGE_MENU_VOL_STEP   331
#define MESSAGE_MENU_VOL_MAX    332
#define MESSAGE_MENU_VOL_MIN    333
#define MESSAGE_MENU_VOL_INIT   334
#define MESSAGE_MENU_NEW_CARD   335
#define MESSAGE_MENU_EXIT       336
#define MESSAGE_CARD_CONFIGURED 400
#define MESSAGE_ERROR           401
#define MESSAGE_RESET_TAG       800
#define MESSAGE_RESET_TAG_OK    801
#define MESSAGE_RESET_ABORTED   802
#define MESSAGE_RESET_DONE      999

/* helpers */
#define MENU_VOL_STEP 1
#define MENU_VOL_MAX  2
#define MENU_VOL_MIN  3
#define MENU_VOL_INIT 4
#define MENU_NEW_CARD 5
#define MENU_EXIT     6
#define MENU_ENTRIES  MENU_EXIT

#define MENU_CMOD_RADIO_PLAY 1
#define MENU_CMOD_ALBUM      2
#define MENU_CMOD_PARTY      3
#define MENU_CMOD_SINGLE     4
#define MENU_CMOD_AUDIOBOOK  5
#define MENU_CMOD_ADMIN      6
#define MENU_CMOD_MOD        7
#define MENU_CMOD_ENTRIES    MENU_CMOD_MOD

#endif
// vim: ts=4 sw=4 et cindent
