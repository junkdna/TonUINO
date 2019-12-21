#ifndef _RFID_H_
#define _RFID_H_

#include <MFRC522.h>

class TonUINO;

struct RFIDCard {
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
        void setup(TonUINO *tonuino);
        void loop();

        RFIDReader(MFRC522 *mfrc522);
};

#endif
// vim: ts=4 sw=4 et cindent
