/*
 * SPDX-License-Identifier: GPL-3.0-only
 * based on https://github.com/miguelbalboa/rfid examples/ReadAndWrite/ReadAndWrite.ino
 * Copyright 2019 Tillmann Heidsieck <theidsieck@leenox.de>
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
        card->chip_id[0], card->chip_id[1], card->chip_id[2], card->chip_id[3],
        card->version, card->card_mode,
        card->extdata[0], card->extdata[1], card->extdata[2], card->extdata[3],
        card->extdata[4], card->extdata[5], card->extdata[6], card->extdata[7],
        card->extdata[8], card->extdata[9]
    };
    byte dummy[4], dsz = sizeof(dummy);
    MFRC522::StatusCode status;

    status = mfrc522->PICC_RequestA(dummy, &dsz);
    Serial.print(F("PCD_RequestA() failed: "));
    Serial.println(mfrc522->GetStatusCodeName(status));

    status = mfrc522->PICC_WakeupA(dummy, &dsz);
    Serial.print(F("PCD_WakeupA() failed: "));
    Serial.println(mfrc522->GetStatusCodeName(status));

    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("No card present"));
        return -1;
    }

    if (!mfrc522->PICC_ReadCardSerial()) {
        Serial.print(F("Failed to read card serial"));
        return -1;
    }

    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522->uid.uidByte, mfrc522->uid.size);

    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = mfrc522->PICC_GetType(mfrc522->uid.sak);
    Serial.println(mfrc522->PICC_GetTypeName(piccType));

    Serial.println(F("Authenticating again using key B..."));
    status = mfrc522->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailer_block, &keyB, &(mfrc522->uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522->GetStatusCodeName(status));
        return -1;
    }

    status = mfrc522->MIFARE_Write(block_addr, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
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

    Serial.print(F("Card UID: "));
    dump_byte_array(mfrc522->uid.uidByte, mfrc522->uid.size);

    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = mfrc522->PICC_GetType(mfrc522->uid.sak);
    Serial.println(mfrc522->PICC_GetTypeName(piccType));

    Serial.println(F("Authenticating using key A..."));
    status = mfrc522->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailer_block, &keyA, &(mfrc522->uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522->GetStatusCodeName(status));
        return -1;
    }

    Serial.println(F("Current data in sector:"));
    mfrc522->PICC_DumpMifareClassicSectorToSerial(&(mfrc522->uid), &keyA, sector);
    Serial.println();

    status = mfrc522->MIFARE_Read(block_addr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522->GetStatusCodeName(status));
        return -1;
    }

    for (size = 0; size < 4; size++)
        card->chip_id[size] = buffer[size];

    for (size = 0; size < 10; size++)
        card->extdata[size] = buffer[size];

    card->version    = buffer[0x4];
    card->card_mode  = buffer[0x5];

    stop();

    return 0;
}

void RFIDReader::setup(TonUINO *tonuino) {
    this->tonuino = tonuino;
    SPI.begin();
    mfrc522->PCD_Init();
    mfrc522->PCD_DumpVersionToSerial();
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

void RFIDReader::dump_byte_array(byte *buffer, byte buffer_size) {
    for (byte i = 0; i < buffer_size; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
    Serial.println();
}

RFIDReader::RFIDReader(MFRC522 *mfrc522) : mfrc522(mfrc522) {
    block_addr = 4;
    sector = block_addr / 4;
    trailer_block = block_addr + 3;
}
// vim: ts=4 sw=4 et cindent
