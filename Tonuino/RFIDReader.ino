#include <SPI.h>

#include "rfid.h"
#include "tonuino.h"
#include "messages.h"

static void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

RFIDReader::RFIDReader(MFRC522 *mfrc522) : mfrc522(mfrc522) {
}

int RFIDReader::write(RFIDCard& card) {
    MFRC522::PICC_Type mifareType;
    byte buffer[16] = {};

    mifareType = mfrc522->PICC_GetType(mfrc522->uid.sak);

    // Authenticate using key B
    Serial.println(F("Authenticating again using key B..."));
    status = (MFRC522::StatusCode)mfrc522->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522->uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522->GetStatusCodeName(status));
        tonuino->get_dfplayer()->playMp3FolderTrack(MESSAGE_ERROR);
        return -1;
    }

    // Write data to the block
    Serial.print(F("Writing data into block "));
    Serial.print(blockAddr);
    Serial.println(F(" ..."));
    dump_byte_array(buffer, 16);
    Serial.println();
    status = (MFRC522::StatusCode)mfrc522->MIFARE_Write(blockAddr, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522->GetStatusCodeName(status));
        tonuino->get_dfplayer()->playMp3FolderTrack(MESSAGE_ERROR);
    } else {
        tonuino->get_dfplayer()->playMp3FolderTrack(MESSAGE_CARD_CONFIGURED);
    }
    Serial.println();
}

int RFIDReader::read(RFIDCard& card) {
    bool returnValue = true;
    byte buffer[16], size = 16 ;

    // Show some details of the PICC (that is: the tag/card)
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522->uid.uidByte, mfrc522->uid.size);

    Serial.println();
    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = mfrc522->PICC_GetType(mfrc522->uid.sak);
    Serial.println(mfrc522->PICC_GetTypeName(piccType));


    // Authenticate using key A
    Serial.println(F("Authenticating using key A..."));
    status = (MFRC522::StatusCode)mfrc522->PCD_Authenticate(
            MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522->uid));
    if (status != MFRC522::STATUS_OK) {
        returnValue = false;
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522->GetStatusCodeName(status));
        return returnValue;
    }

    // Show the whole sector as it currently is
    Serial.println(F("Current data in sector:"));
    mfrc522->PICC_DumpMifareClassicSectorToSerial(&(mfrc522->uid), &key, sector);
    Serial.println();

    // Read data from the block
    Serial.print(F("Reading data from block "));
    Serial.print(blockAddr);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode)mfrc522->MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        returnValue = false;
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522->GetStatusCodeName(status));
    }
    Serial.print(F("Data in block "));
    Serial.print(blockAddr);
    Serial.println(F(":"));
    dump_byte_array(buffer, 16);
    Serial.println();
    Serial.println();

    return returnValue;
}

void RFIDReader::setup(TonUINO *tonuino) {
    this->tonuino = tonuino;
    SPI.begin();
    mfrc522->PCD_Init();
    mfrc522->PCD_DumpVersionToSerial();
    for (byte i = 0; i < 6; i++)
        key.keyByte[i] = 0xFF;
}

void RFIDReader::loop() {
    class RFIDCard card;
    if (mfrc522->PICC_IsNewCardPresent() && mfrc522->PICC_ReadCardSerial()) {
        read(card);
        tonuino->notify_rfid(card);
    }
}
// vim: ts=4 sw=4 et cindent
