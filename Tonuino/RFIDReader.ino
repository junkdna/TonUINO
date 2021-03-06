/*
 * SPDX-License-Identifier: GPL-3.0-only
 * based on https://github.com/miguelbalboa/rfid examples/ReadAndWrite/ReadAndWrite.ino
 * Copyright 2019-2020 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include <SPI.h>
#include <MFRC522.h>

#include "rfid.h"
#include "tonuino.h"
#include "messages.h"

const uint8_t keyA_default[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
const uint8_t keyB_default[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

int RFIDReader::write(RFIDCard *card) {
    byte buffer[16] = {
        card->magic[0], card->magic[1], card->magic[2], card->magic[3],
        card->version, card->card_mode,
        card->extdata[0], card->extdata[1], card->extdata[2], card->extdata[3],
        card->extdata[4], card->extdata[5], card->extdata[6], card->extdata[7],
        card->extdata[8], card->extdata[9]
    };
    byte dummy[4], dsz = sizeof(dummy);
    MFRC522::StatusCode status;

    status = mfrc522->PICC_RequestA(dummy, &dsz);
    (void)status;

    status = mfrc522->PICC_WakeupA(dummy, &dsz);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("No card present"));
        return -1;
    }

    if (!mfrc522->PICC_ReadCardSerial()) {
        Serial.print(F("Failed to read card serial"));
        return -1;
    }

    MFRC522::PICC_Type piccType = mfrc522->PICC_GetType(mfrc522->uid.sak);
    (void)piccType;

    status = mfrc522->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailer_block, &keyB, &(mfrc522->uid));
    if (status != MFRC522::STATUS_OK) {
        stop();
        Serial.print(F("PCD_Authenticate() failed: "));
        return -1;
    }

    status = mfrc522->MIFARE_Write(block_addr, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
        stop();
        Serial.print(F("MIFARE_Write() failed: "));
        return -1;
    }
    Serial.println();

    stop();

    return 0;
}

int RFIDReader::read(RFIDCard *card) {
    MFRC522::StatusCode status;
    byte buffer[18], size = 18;

    /* TODO maybe add wakeup if requested by caller */
    if (!mfrc522->PICC_IsNewCardPresent()) {
        return -1;
    }

    if (!mfrc522->PICC_ReadCardSerial()) {
        Serial.print(F("Failed to read card serial"));
        return -1;
    }

    MFRC522::PICC_Type piccType = mfrc522->PICC_GetType(mfrc522->uid.sak);
    (void)piccType;

    status = mfrc522->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailer_block, &keyA, &(mfrc522->uid));
    if (status != MFRC522::STATUS_OK) {
        stop();
        Serial.print(F("PCD_Authenticate() failed: "));
        return -1;
    }

    status = mfrc522->MIFARE_Read(block_addr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        stop();
        Serial.print(F("MIFARE_Read() failed: "));
        return -1;
    }

    for (size = 0; size < 4; size++)
        card->magic[size] = buffer[size];

    card->version    = buffer[0x4];
    card->card_mode  = buffer[0x5];

    for (size = 0; size < 10; size++)
        card->extdata[size] = buffer[0x6 + size];

    stop();

    return 0;
}

void RFIDReader::setup(TonUINO *tonuino) {
    this->tonuino = tonuino;
    SPI.begin();
    mfrc522->PCD_Init();
    for (byte i = 0; i < 6; i++) {
        keyA.keyByte[i] = keyA_default[i];
        keyB.keyByte[i] = keyB_default[i];
    }
}

void RFIDReader::loop() {
    if (!current_card) {
        current_card = new RFIDCard(this);
        if (!current_card)
            return;
    }

    if (!current_card->read())
        return;

    tonuino->notify_rfid(current_card);
    current_card = nullptr;
}

void RFIDReader::stop() {
    mfrc522->PICC_HaltA();
    mfrc522->PCD_StopCrypto1();
}

RFIDReader::RFIDReader(MFRC522 *mfrc522) : mfrc522(mfrc522), block_addr(4), current_card(nullptr), tonuino(nullptr) {
    sector = block_addr / 4;
    trailer_block = block_addr + 3;
}

bool RFIDCard::check() {
    uint32_t _magic = CARD_MAGIC;
    if (magic[0] != ((_magic >>  0) & 0xff))
        return false;

    if (magic[1] != ((_magic >>  8) & 0xff))
        return false;

    if (magic[2] != ((_magic >> 16) & 0xff))
        return false;

    if (magic[3] != ((_magic >> 24) & 0xff))
        return false;

    if (version != CARD_VERSION)
        return false;

    return true;
}

RFIDCard::RFIDCard(RFIDReader *r) : reader(r) {
    memset(magic, 0, 4);
    memset(extdata, 0, 10);
    version = CARD_VERSION;
    card_mode = CARD_MODE_NONE;
}
// vim: ts=4 sw=4 et cindent
