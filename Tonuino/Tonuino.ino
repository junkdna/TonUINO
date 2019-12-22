/*
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright TODO Thorsten Voß
 * Copyright 2019 Tillmann Heidsieck <theidsieck@leenox.de>
 */

#include "buttons.h"
#include "rfid.h"
#include "tonuino.h"
#include "tstate.h"


DFMiniMp3<SoftwareSerial, Mp3Notify>* TonUINO::get_dfplayer()
{
    return this->dfplay;
}

void TonUINO::notify_mp3(mp3_notify_event event, uint16_t code) {
    (void)code;
    switch(event) {
        case MP3_NOTIFY_ERROR:
            /* Fallthrough */
        case MP3_PLAY_FINISHED:
            /* Fallthrough */
        case MP3_CARD_ONLINE:
            /* Fallthrough */
        case MP3_CARD_INSERTED:
            /* Fallthrough */
        case MP3_CARD_REMOVED:
            state = state->handle_dfplay_event(event, code);
            break;
        case MP3_USB_ONLINE:
            /* ignore for now */
            break;
        case MP3_USB_INSERTED:
            /* ignore for now */
            break;
        case MP3_USB_REMOVED:
            /* ignore for now */
            break;
    }
}

void TonUINO::notify_buttons(uint8_t pressed, uint8_t released, uint8_t long_pressed)
{
    buttons_pressed = pressed;
    buttons_released = released;
    buttons_long_press = long_pressed;
}

void TonUINO::notify_rfid(RFIDCard& card)
{
    rfid_card = card;
    state->handle_card(&rfid_card);
}

void TonUINO::setup(DFMiniMp3<SoftwareSerial, Mp3Notify>* dfp) {
    /* PIN A3 is open, the ADC should produce noise */
    randomSeed(analogRead(A3));

    pinMode(BUSY_PIN, INPUT);
    dfplay = dfp;
    dfplay->begin();
    dfplay->loop();

    dfplay->reset();
    dfplay->loop();

    state = new TState_Idle(this);
    state->volume_set(INITIAL_VOLUME);

    if (buttons_pressed == 0x7) {
        /* reset */
        buttons_pressed = 0;
    }
}

void TonUINO::loop() {
    dfplay->loop();
    state = state->handle_card(&rfid_card);
    state = state->handle_buttons(buttons_pressed, buttons_released, buttons_long_press);
}

TonUINO tonuino;

static SoftwareSerial g_soft_uart(SOFT_UART_RX_PIN, SOFT_UART_TX_PIN);
static DFMiniMp3<SoftwareSerial, Mp3Notify> g_dfplay(g_soft_uart);

MFRC522 mfrc522(MFRC522_SS_PIN, MFRC522_RST_PIN);
RFIDReader rfid(&mfrc522);

static Button button_down(DOWN_BUTTON_PIN, 25, false, true);
static Button button_pause(PAUSE_BUTTON_PIN, 25, false, true);
static Button button_up(UP_BUTTON_PIN, 25, false, true);
static Buttons buttons(&button_down, &button_pause, &button_up);

void setup() {
    Serial.begin(115200);

    Serial.println(F("TonUINO NG Version 0.1"));
    Serial.println(F("(c) Tilllmann Heidsieck"));
    Serial.println(F("based on work by Thorsten Voß"));

    buttons.setup(&tonuino);
    rfid.setup(&tonuino);
    tonuino.setup(&g_dfplay);
}

void loop() {
    buttons.loop();
    rfid.loop();
    tonuino.loop();
}

#if 0
// Leider kann das Modul keine Queue abspielen.
static uint16_t _lastTrackFinished;
static void nextTrack(uint16_t track) {
    led_mode = MODE_PLAYING;

    if (track == _lastTrackFinished) {
        led_mode = MODE_NO_MODE;
        return;
    }
    _lastTrackFinished = track;

    if (knownCard == false) {
        led_mode = MODE_NO_CARD;
        // Wenn eine neue Karte angelernt wird soll das Ende eines Tracks nicht
        // verarbeitet werden
        return;
    }

    if (myCard.mode == 1) {
        Serial.println(F("Hörspielmodus ist aktiv -> keinen neuen Track spielen"));
        //    mp3.sleep(); // Je nach Modul kommt es nicht mehr zurück aus dem Sleep!
    }
    if (myCard.mode == 2) {
        if (currentTrack != numTracksInFolder) {
            currentTrack = currentTrack + 1;
            mp3.playFolderTrack(myCard.folder, currentTrack);
            Serial.print(F("Albummodus ist aktiv -> nächster Track: "));
            Serial.print(currentTrack);
        } else 
            //      mp3.sleep();   // Je nach Modul kommt es nicht mehr zurück aus dem Sleep!
        { }
    }
    if (myCard.mode == 3) {
        uint16_t oldTrack = currentTrack;
        currentTrack = random(1, numTracksInFolder + 1);
        if (currentTrack == oldTrack)
            currentTrack = currentTrack == numTracksInFolder ? 1 : currentTrack+1;
        Serial.print(F("Party Modus ist aktiv -> zufälligen Track spielen: "));
        Serial.println(currentTrack);
        mp3.playFolderTrack(myCard.folder, currentTrack);
    }
    if (myCard.mode == 4) {
        Serial.println(F("Einzel Modus aktiv -> Strom sparen"));
        //    mp3.sleep();      // Je nach Modul kommt es nicht mehr zurück aus dem Sleep!
    }
    if (myCard.mode == 5) {
        if (currentTrack != numTracksInFolder) {
            currentTrack = currentTrack + 1;
            Serial.print(F("Hörbuch Modus ist aktiv -> nächster Track und "
                        "Fortschritt speichern"));
            Serial.println(currentTrack);
            mp3.playFolderTrack(myCard.folder, currentTrack);
            // Fortschritt im EEPROM abspeichern
            EEPROM.write(myCard.folder, currentTrack);
        } else {
            //      mp3.sleep();  // Je nach Modul kommt es nicht mehr zurück aus dem Sleep!
            // Fortschritt zurück setzen
            EEPROM.write(myCard.folder, 1);
        }
    }
}

static void previousTrack() {
    if (myCard.mode == 1) {
        Serial.println(F("Hörspielmodus ist aktiv -> Track von vorne spielen"));
        mp3.playFolderTrack(myCard.folder, currentTrack);
    }
    if (myCard.mode == 2) {
        Serial.println(F("Albummodus ist aktiv -> vorheriger Track"));
        if (currentTrack != 1) {
            currentTrack = currentTrack - 1;
        }
        mp3.playFolderTrack(myCard.folder, currentTrack);
    }
    if (myCard.mode == 3) {
        Serial.println(F("Party Modus ist aktiv -> Track von vorne spielen"));
        mp3.playFolderTrack(myCard.folder, currentTrack);
    }
    if (myCard.mode == 4) {
        Serial.println(F("Einzel Modus aktiv -> Track von vorne spielen"));
        mp3.playFolderTrack(myCard.folder, currentTrack);
    }
    if (myCard.mode == 5) {
        Serial.println(F("Hörbuch Modus ist aktiv -> vorheriger Track und "
                    "Fortschritt speichern"));
        if (currentTrack != 1) {
            currentTrack = currentTrack - 1;
        }
        mp3.playFolderTrack(myCard.folder, currentTrack);
        // Fortschritt im EEPROM abspeichern
        EEPROM.write(myCard.folder, currentTrack);
    }
}

// MFRC522
#define RST_PIN 9                 // Configurable, see typical pin layout above
#define SS_PIN 10                 // Configurable, see typical pin layout above
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522
MFRC522::MIFARE_Key key;
bool successRead;
byte sector = 1;
byte blockAddr = 4;
byte trailerBlock = 7;
MFRC522::StatusCode status;

#define buttonPause A0
#define buttonUp A1
#define buttonDown A2
#define busyPin 4

#define LONG_PRESS 1000

Button pauseButton(buttonPause);
Button upButton(buttonUp);
Button downButton(buttonDown);
bool ignorePauseButton = false;
bool ignoreUpButton = false;
bool ignoreDownButton = false;

uint8_t numberOfCards = 0;


void setup() {

    Serial.begin(115200); // Es gibt ein paar Debug Ausgaben über die serielle
    // Schnittstelle
    randomSeed(analogRead(A0)); // Zufallsgenerator initialisieren

    Serial.println(F("TonUINO Version 2.0"));
    Serial.println(F("(c) Thorsten Voß"));
    Serial.println(F("modified by Tilllmann Heidsieck"));

    // Knöpfe mit PullUp
    pinMode(buttonPause, INPUT);
    pinMode(buttonUp, INPUT);
    pinMode(buttonDown, INPUT);

    // Busy Pin
    pinMode(busyPin, INPUT);

    // DFPlayer Mini initialisieren
    mp3.begin();
    uint16_t volume = mp3.getVolume();
    Serial.print("volume was ");
    Serial.println(volume);
    mp3.setVolume(15);

    // NFC Leser initialisieren
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522
    mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    // RESET --- ALLE DREI KNÖPFE BEIM STARTEN GEDRÜCKT HALTEN -> alle bekannten
    // Karten werden gelöscht
    if (digitalRead(buttonPause) == LOW && digitalRead(buttonUp) == LOW &&
            digitalRead(buttonDown) == LOW) {
        Serial.println(F("Reset -> EEPROM wird gelöscht"));
        for (int i = 0; i < EEPROM.length(); i++) {
            EEPROM.write(i, 0);
        }
    }

    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
}

void update_led(uint8_t mode)
{
    static int8_t cur_led = 0;
    int8_t i;

    switch (mode) {
        case MODE_CARD_FAIL:
            cur_led = 0;
            for (i = 0; i < NUM_LEDS; i++)
                leds[i] = CRGB::Red;
            FastLED.show();
            for (i = 0; i < NUM_LEDS; i++)
                leds[i] = CRGB::Black;
            break;
        case MODE_CARD_NEW:
            for (cur_led = 0; cur_led < NUM_LEDS; cur_led++) {
                cur_led = (cur_led + 1) % NUM_LEDS;
                leds[cur_led] = CRGB::Blue;
                FastLED.show();
                delay(72);
                leds[cur_led] = CRGB::Black;
            }
            break;
        case MODE_CARD_PROG:
            for (i = 0; i < NUM_LEDS; i++)
                leds[i] = CRGB::Yellow;
            FastLED.show();
            for (i = 0; i < NUM_LEDS; i++)
                leds[i] = CRGB::Black;
            break;
        case MODE_PAUSE:
            cur_led = (cur_led + 1) % NUM_LEDS;
            leds[cur_led] = CRGB::Green;
            FastLED.show();
            leds[cur_led] = CRGB::Black;
            break;
        case MODE_PLAYING:
            leds[0]  = CRGB::Green;
            leds[3]  = CRGB::Green;
            //leds[4]  = CRGB::Green;
            //leds[6]  = CRGB::Green;
            leds[7]  = CRGB::Green;
            leds[10] = CRGB::Green;
            //leds[11] = CRGB::Green;
            //leds[13] = CRGB::Green;
            FastLED.show();
            for (i = 0; i < NUM_LEDS; i++)
                leds[i] = CRGB::Black;
            break;
        case MODE_NO_MODE:
            /* Fallthrough */
        case MODE_NO_CARD:
            /* Fallthrough */
        default:
            cur_led = 0;
            for (i = 0; i < NUM_LEDS; i++)
                leds[i] = CRGB::Blue;
            FastLED.show();
            for (i = 0; i < NUM_LEDS; i++)
                leds[i] = CRGB::Black;
            break;
    }
}

void loop() {
    do {
        update_led(led_mode);
        mp3.loop();
        // Buttons werden nun über JS_Button gehandelt, dadurch kann jede Taste
        // doppelt belegt werden
        pauseButton.read();
        upButton.read();
        downButton.read();

        if (pauseButton.wasReleased()) {
            if (ignorePauseButton == false) {
                if (isPlaying()) {
                    led_mode = MODE_PAUSE;
                    mp3.pause();
                } else {
                    led_mode = MODE_PLAYING;
                    mp3.start();
                }
            }
            ignorePauseButton = false;
        } else if (pauseButton.pressedFor(LONG_PRESS) &&
                ignorePauseButton == false) {
            if (isPlaying())
                mp3.playAdvertisement(currentTrack);
            else {
                led_mode = MODE_NO_CARD;
                knownCard = false;
                mp3.playMp3FolderTrack(800);
                Serial.println(F("Karte resetten..."));
                resetCard();
                mfrc522.PICC_HaltA();
                mfrc522.PCD_StopCrypto1();
            }
            ignorePauseButton = true;
        }

        if (upButton.pressedFor(LONG_PRESS)) {
            Serial.println(F("Volume Up"));
            mp3.increaseVolume();
            ignoreUpButton = true;
        } else if (upButton.wasReleased()) {
            if (!ignoreUpButton)
                nextTrack(random(65536));
            else
                ignoreUpButton = false;
        }

        if (downButton.pressedFor(LONG_PRESS)) {
            Serial.println(F("Volume Down"));
            mp3.decreaseVolume();
            ignoreDownButton = true;
        } else if (downButton.wasReleased()) {
            if (!ignoreDownButton)
                previousTrack();
            else
                ignoreDownButton = false;
        }
        // Ende der Buttons
    } while (!mfrc522.PICC_IsNewCardPresent());

    // RFID Karte wurde aufgelegt

    if (!mfrc522.PICC_ReadCardSerial())
        return;

    if (readCard(&myCard) == true) {
        update_led(MODE_CARD_NEW);
        if (myCard.cookie == 322417479 && myCard.folder != 0 && myCard.mode != 0) {

            knownCard = true;
            _lastTrackFinished = 0;
            numTracksInFolder = mp3.getFolderTrackCount(myCard.folder);
            Serial.print(numTracksInFolder);
            Serial.print(F(" Dateien in Ordner "));
            Serial.println(myCard.folder);

            // Hörspielmodus: eine zufällige Datei aus dem Ordner
            if (myCard.mode == 1) {
                Serial.println(F("Hörspielmodus -> zufälligen Track wiedergeben"));
                currentTrack = random(1, numTracksInFolder + 1);
                Serial.println(currentTrack);
                mp3.playFolderTrack(myCard.folder, currentTrack);
            }
            // Album Modus: kompletten Ordner spielen
            if (myCard.mode == 2) {
                Serial.println(F("Album Modus -> kompletten Ordner wiedergeben"));
                currentTrack = 1;
                mp3.playFolderTrack(myCard.folder, currentTrack);
            }
            // Party Modus: Ordner in zufälliger Reihenfolge
            if (myCard.mode == 3) {
                Serial.println(
                        F("Party Modus -> Ordner in zufälliger Reihenfolge wiedergeben"));
                currentTrack = random(1, numTracksInFolder + 1);
                mp3.playFolderTrack(myCard.folder, currentTrack);
            }
            // Einzel Modus: eine Datei aus dem Ordner abspielen
            if (myCard.mode == 4) {
                Serial.println(
                        F("Einzel Modus -> eine Datei aus dem Odrdner abspielen"));
                currentTrack = myCard.special;
                mp3.playFolderTrack(myCard.folder, currentTrack);
            }
            // Hörbuch Modus: kompletten Ordner spielen und Fortschritt merken
            if (myCard.mode == 5) {
                Serial.println(F("Hörbuch Modus -> kompletten Ordner spielen und "
                            "Fortschritt merken"));
                currentTrack = EEPROM.read(myCard.folder);
                mp3.playFolderTrack(myCard.folder, currentTrack);
            }

            led_mode = MODE_PLAYING;
        }

        // Neue Karte konfigurieren
        else {
            led_mode = MODE_CARD_PROG;
            knownCard = false;
            setupCard();
        }

    }
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}

int voiceMenu(int numberOfOptions, int startMessage, int messageOffset,
        bool preview = false, int previewFromFolder = 0) {
    int returnValue = 0;
    led_mode = MODE_CARD_PROG;
    if (startMessage != 0)
        mp3.playMp3FolderTrack(startMessage);
    do {
        update_led(led_mode);
        pauseButton.read();
        upButton.read();
        downButton.read();
        mp3.loop();
        if (pauseButton.wasPressed()) {
            if (returnValue != 0)
                return returnValue;
            delay(1000);
        }

        if (upButton.pressedFor(LONG_PRESS)) {
            returnValue = min(returnValue + 10, numberOfOptions);
            mp3.playMp3FolderTrack(messageOffset + returnValue);
            delay(1000);
            if (preview) {
                do {
                    delay(10);
                    update_led(led_mode);
                } while (isPlaying());
                if (previewFromFolder == 0)
                    mp3.playFolderTrack(returnValue, 1);
                else
                    mp3.playFolderTrack(previewFromFolder, returnValue);
            }
            ignoreUpButton = true;
        } else if (upButton.wasReleased()) {
            if (!ignoreUpButton) {
                returnValue = min(returnValue + 1, numberOfOptions);
                mp3.playMp3FolderTrack(messageOffset + returnValue);
                delay(1000);
                if (preview) {
                    do {
                        delay(10);
                        update_led(led_mode);
                    } while (isPlaying());
                    if (previewFromFolder == 0)
                        mp3.playFolderTrack(returnValue, 1);
                    else
                        mp3.playFolderTrack(previewFromFolder, returnValue);
                }
            } else
                ignoreUpButton = false;
        }

        if (downButton.pressedFor(LONG_PRESS)) {
            returnValue = max(returnValue - 10, 1);
            mp3.playMp3FolderTrack(messageOffset + returnValue);
            delay(1000);
            if (preview) {
                do {
                    delay(10);
                    update_led(led_mode);
                } while (isPlaying());
                if (previewFromFolder == 0)
                    mp3.playFolderTrack(returnValue, 1);
                else
                    mp3.playFolderTrack(previewFromFolder, returnValue);
            }
            ignoreDownButton = true;
        } else if (downButton.wasReleased()) {
            if (!ignoreDownButton) {
                returnValue = max(returnValue - 1, 1);
                mp3.playMp3FolderTrack(messageOffset + returnValue);
                delay(1000);
                if (preview) {
                    do {
                        delay(10);
                        update_led(led_mode);
                    } while (isPlaying());
                    if (previewFromFolder == 0)
                        mp3.playFolderTrack(returnValue, 1);
                    else
                        mp3.playFolderTrack(previewFromFolder, returnValue);
                }
            } else
                ignoreDownButton = false;
        }
    } while (true);
}

void resetCard() {
    do {
        pauseButton.read();
        upButton.read();
        downButton.read();

        if (upButton.wasReleased() || downButton.wasReleased()) {
            Serial.print(F("Abgebrochen!"));
            mp3.playMp3FolderTrack(802);
            return;
        }
    } while (!mfrc522.PICC_IsNewCardPresent());

    if (!mfrc522.PICC_ReadCardSerial())
        return;

    Serial.print(F("Karte wird neu Konfiguriert!"));
    setupCard();
}

void setupCard() {
    led_mode = MODE_CARD_PROG;
    update_led(led_mode);
    mp3.pause();
    Serial.print(F("Neue Karte konfigurieren"));

    // Ordner abfragen
    myCard.folder = voiceMenu(99, 300, 0, true);

    // Wiedergabemodus abfragen
    myCard.mode = voiceMenu(6, 310, 310);

    // Hörbuchmodus -> Fortschritt im EEPROM auf 1 setzen
    EEPROM.write(myCard.folder,1);

    // Einzelmodus -> Datei abfragen
    if (myCard.mode == 4)
        myCard.special = voiceMenu(mp3.getFolderTrackCount(myCard.folder), 320, 0,
                true, myCard.folder);

    // Admin Funktionen
    if (myCard.mode == 6)
        myCard.special = voiceMenu(3, 320, 320);

    // Karte ist konfiguriert -> speichern
    mp3.pause();
    writeCard(myCard);
}

#endif
// vim: ts=4 sw=4 et cindent
