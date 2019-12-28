/*
 * SPDX-License-Identifier: GPL-3.0-only
 * based on https://github.com/miguelbalboa/rfid examples/ReadAndWrite/ReadAndWrite.ino
 * Copyright 2019 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include <SPI.h>

#include "rfid.h"
#include "tonuino.h"
#include "messages.h"

const uint8_t keyA_default[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
const uint8_t keyB_default[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

RFIDReader::RFIDReader(MFRC522 *mfrc522) : mfrc522(mfrc522) {
}

int RFIDReader::write(RFIDCard *card) {
    MFRC522::StatusCode status;

    byte buffer[16] = {
        card->chip_id[0], card->chip_id[1], card->chip_id[2], card->chip_id[3],
        card->version, card->card_mode,
        card->extdata[0], card->extdata[1], card->extdata[2], card->extdata[3],
        card->extdata[4], card->extdata[5], card->extdata[6], card->extdata[7],
        card->extdata[8], card->extdata[9]
    };

    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522->uid.uidByte, mfrc522->uid.size);

    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = mfrc522->PICC_GetType(mfrc522->uid.sak);
    Serial.println(mfrc522->PICC_GetTypeName(piccType));

    Serial.println(F("Authenticating again using key B..."));
    status = mfrc522->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &keyB, &(mfrc522->uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522->GetStatusCodeName(status));
        return -1;
    }

    status = mfrc522->MIFARE_Write(blockAddr, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        return -1;
    }
    Serial.println();

    return 0;
}

int RFIDReader::read(RFIDCard *card) {
    MFRC522::StatusCode status;
    byte buffer[18], size = 18;

    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522->uid.uidByte, mfrc522->uid.size);

    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = mfrc522->PICC_GetType(mfrc522->uid.sak);
    Serial.println(mfrc522->PICC_GetTypeName(piccType));

    Serial.println(F("Authenticating using key A..."));
    status = mfrc522->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &keyA, &(mfrc522->uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522->GetStatusCodeName(status));
        return -1;
    }

    Serial.println(F("Current data in sector:"));
    mfrc522->PICC_DumpMifareClassicSectorToSerial(&(mfrc522->uid), &keyA, sector);
    Serial.println();

    status = mfrc522->MIFARE_Read(blockAddr, buffer, &size);
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
    if (!mfrc522->PICC_IsNewCardPresent())
        return;

    if (!mfrc522->PICC_ReadCardSerial())
        return;

    RFIDCard *card = new RFIDCard(this);
    if (!card)
        return;

    if (!card->read()) {
        delete card;
        return;
    }

    tonuino->notify_rfid(card);
}

void RFIDReader::stop() {
    mfrc522->PICC_HaltA();
    mfrc522->PCD_StopCrypto1();
}

void RFIDReader::dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
    Serial.println();
}
// vim: ts=4 sw=4 et cindent
