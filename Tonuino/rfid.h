#ifndef _RFID_H_
#define _RFID_H_

#include <MFRC522.h>

class TonUINO;

#define CARD_VERSION 1

enum Card_Mode {
    CARD_MODE_NONE = 0,
    CARD_MODE_PLAYER = 1,
#if 0
    CARD_MODE_MODIFY,
    CARD_MODE_ADMIN,
#endif
};

class RFIDCard;

class RFIDReader {
    protected:
        TonUINO *tonuino;
        MFRC522 *mfrc522;
        MFRC522::MIFARE_Key keyA;
        MFRC522::MIFARE_Key keyB;
        byte sector = 1;
        byte blockAddr = 4;
        byte trailerBlock = 7;

    public:
        int write(RFIDCard *card);
        int read(RFIDCard *card);
        void stop();

        void setup(TonUINO *tonuino);
        void loop();

        RFIDReader(MFRC522 *mfrc522);
        void dump_byte_array(byte *buffer, byte bufferSize);
};

class RFIDCard {
protected:
    RFIDReader *reader;
public:
    uint8_t chip_id[4];
    uint8_t version;
    uint8_t card_mode;
    uint8_t extdata[10];

    bool write() {return reader->write(this) >= 0;}
    bool read()  {return reader->read(this) >= 0;}
    bool check(uint32_t chip_id) {
        return this->chip_id[0] == ((chip_id >>  0) & 0xff) &&
            this->chip_id[1] == ((chip_id >>  8) & 0xff) &&
            this->chip_id[2] == ((chip_id >> 16) & 0xff) &&
            this->chip_id[3] == ((chip_id >> 24) & 0xff) &&
            this->version == CARD_VERSION;
    }
    RFIDCard(RFIDReader *r) : reader(r) {}
};

#endif
// vim: ts=4 sw=4 et cindent
