#ifndef _TONUINO_H_
#define _TONUINO_H_

#include <DFMiniMp3.h>
#include <EEPROM.h>
#include <MFRC522.h>
#include <SPI.h>
#include <SoftwareSerial.h>


#define EEPROM_MAGIC    0x5a5f5059
#define EEPROM_VERSION  1
#define EEPROM_CFG_SIZE 9
#define EEPROM_CFG_LEN  128

#define MAX_VOLUME       35
#define MIN_VOLUME       0
#define INITIAL_VOLUME   15

#define SOFT_UART_RX_PIN 5
#define SOFT_UART_TX_PIN 6

#define LED_GREEN        A4
#define LED_RED          A5
#define LED_BLUE         A6

#define BUSY_PIN         7

#define PAUSE_BUTTON_PIN A0
#define UP_BUTTON_PIN    A1
#define DOWN_BUTTON_PIN  A2

#define MFRC522_RST_PIN  9
#define MFRC522_SS_PIN   10

#define LONG_PRESS_TIMEOUT 1000

enum mp3_notify_event {
    MP3_NOTIFY_ERROR,
    MP3_PLAY_FINISHED,
    MP3_CARD_ONLINE,
    MP3_CARD_INSERTED,
    MP3_CARD_REMOVED,
    MP3_USB_ONLINE,
    MP3_USB_INSERTED,
    MP3_USB_REMOVED,
};

enum mp3_command {
    MP3_CMD_SET_VOL,
};

class Mp3Notify;
class RFIDCard;
class RFIDReader;
class TState;

class EEPROM_Config {
public:
    uint32_t magic;
    uint8_t  version;
    uint32_t id;

    void write();
    void read();
    void init();
    bool check();
};

class TonUINO {
    protected:
        EEPROM_Config config;
        DFMiniMp3<SoftwareSerial, Mp3Notify> *dfplay;
        TState *state;
        RFIDCard *rfid_card;

        uint8_t buttons_pressed = 0;
        uint8_t buttons_released = 0;
        uint8_t buttons_long_press = 0;

    public:
        DFMiniMp3<SoftwareSerial, Mp3Notify>* get_dfplayer();
        EEPROM_Config &get_config();
        void setup(DFMiniMp3<SoftwareSerial, Mp3Notify> *dfp);
        void loop();

        /* this goes into the state */
        void notify_buttons(uint8_t pressed, uint8_t released, uint8_t long_pressed);

        void notify_rfid(RFIDCard *rfid_card);
        void notify_mp3(mp3_notify_event event, uint16_t code);
};

extern TonUINO tonuino;

class Mp3Notify {
    public:
        static void OnError(uint16_t errorCode) {
            Serial.println();
            Serial.print("Com Error ");
            Serial.println(errorCode);
            tonuino.notify_mp3(MP3_NOTIFY_ERROR, errorCode);
        }
        static void OnPlayFinished(uint16_t track) {
            Serial.print("Track beendet");
            Serial.println(track);
            tonuino.notify_mp3(MP3_PLAY_FINISHED, track);
        }
        static void OnCardOnline(uint16_t code) {
            Serial.println(F("SD Karte online "));
            tonuino.notify_mp3(MP3_CARD_ONLINE, code);
        }
        static void OnCardInserted(uint16_t code) {
            Serial.println(F("SD Karte bereit "));
            tonuino.notify_mp3(MP3_CARD_INSERTED, code);
        }
        static void OnCardRemoved(uint16_t code) {
            Serial.println(F("SD Karte entfernt "));
            tonuino.notify_mp3(MP3_CARD_REMOVED, code);
        }
        static void OnUsbOnline(uint16_t code) {
            Serial.println(F("USB online "));
            tonuino.notify_mp3(MP3_USB_ONLINE, code);
        }
        static void OnUsbInserted(uint16_t code) {
            Serial.println(F("USB bereit "));
            tonuino.notify_mp3(MP3_USB_INSERTED, code);
        }
        static void OnUsbRemoved(uint16_t code) {
            Serial.println(F("USB entfernt "));
            tonuino.notify_mp3(MP3_USB_REMOVED, code);
        }
};

#endif
// vim: ts=4 sw=4 et cindent
