#ifndef _RFID_H_
#define _RFID_H_

#include <MFRC522.h>

class TonUINO;

enum Card_Mode {
    CARD_MODE_PLAYER,
#if 0
    CARD_MODE_MODIFY,
    CARD_MODE_ADMIN,
#endif
};

class RFIDCard {
public:
    uint32_t chip_id;
    uint8_t  version;
    uint8_t  card_mode;
    uint8_t  extdata[10];
};

class RFIDReader {
    protected:
	TonUINO *tonuino;
        MFRC522 *mfrc522;
        MFRC522::MIFARE_Key key;
        MFRC522::StatusCode status;
        byte sector = 1;
        byte blockAddr = 4;
        byte trailerBlock = 7;

    public:
        int write(RFIDCard& card);
        int read(RFIDCard& card);

        void setup(TonUINO *tonuino);
        void loop();

        RFIDReader(MFRC522 *mfrc522);
};

#endif
// vim: ts=4 sw=4 et cindent
