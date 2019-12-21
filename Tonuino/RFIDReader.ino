#include <SPI.h>

#include "rfid.h"
#include "tonuino.h"

RFIDReader::RFIDReader(MFRC522 *mfrc522) : mfrc522(mfrc522) {
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

}

#if 0
bool readCard(nfcTagObject *nfcTag) {
    bool returnValue = true;
    // Show some details of the PICC (that is: the tag/card)
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    byte buffer[18];
    byte size = sizeof(buffer);

    // Authenticate using key A
    Serial.println(F("Authenticating using key A..."));
    status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(
            MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        returnValue = false;
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return returnValue;
    }

    // Show the whole sector as it currently is
    Serial.println(F("Current data in sector:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
    Serial.println();

    // Read data from the block
    Serial.print(F("Reading data from block "));
    Serial.print(blockAddr);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        returnValue = false;
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.print(F("Data in block "));
    Serial.print(blockAddr);
    Serial.println(F(":"));
    dump_byte_array(buffer, 16);
    Serial.println();
    Serial.println();

    uint32_t tempCookie;
    tempCookie = (uint32_t)buffer[0] << 24;
    tempCookie += (uint32_t)buffer[1] << 16;
    tempCookie += (uint32_t)buffer[2] << 8;
    tempCookie += (uint32_t)buffer[3];

    nfcTag->cookie = tempCookie;
    nfcTag->version = buffer[4];
    nfcTag->folder = buffer[5];
    nfcTag->mode = buffer[6];
    nfcTag->special = buffer[7];

    return returnValue;
}

void writeCard(nfcTagObject nfcTag) {
    MFRC522::PICC_Type mifareType;
    byte buffer[16] = {0x13, 0x37, 0xb3, 0x47, // 0x1337 0xb347 magic cookie to
        // identify our nfc tags
        0x01,                   // version 1
        nfcTag.folder,          // the folder picked by the user
        nfcTag.mode,    // the playback mode picked by the user
        nfcTag.special, // track or function for admin cards
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    byte size = sizeof(buffer);

    mifareType = mfrc522.PICC_GetType(mfrc522.uid.sak);

    // Authenticate using key B
    Serial.println(F("Authenticating again using key B..."));
    status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(
            MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        mp3.playMp3FolderTrack(401);
        return;
    }

    // Write data to the block
    Serial.print(F("Writing data into block "));
    Serial.print(blockAddr);
    Serial.println(F(" ..."));
    dump_byte_array(buffer, 16);
    Serial.println();
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(blockAddr, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        mp3.playMp3FolderTrack(401);
    }
    else
        mp3.playMp3FolderTrack(400);
    Serial.println();
    delay(100);
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
#endif
// vim: ts=4 sw=4 et cindent
