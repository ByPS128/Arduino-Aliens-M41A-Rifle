#pragma once

#include <Arduino.h>

// Use pins 2 and 3 to communicate with DFPlayer Mini
static const uint8_t PIN_MP3_TX = 2; // Connects to module's RX
static const uint8_t PIN_MP3_RX = 3; // Connects to module's TX

static const int MAX_VOLUME = 22; // DFPlayer má rozsah hlasitosti od 0 do 30, ale me už u hodnoty 24 zamrzá.
static const int VOLUME_ADDRESS = 10; // Adresa v EEPROM pro uložení hlasitosti
static const int INIT_FLAG_ADDRESS = 9;  // Adresa pro uložení kontrolního flagu
static const byte INITIALIZED_FLAG = 0xA5;  // Nějaká specifická hodnota pro kontrolu inicializace
static const byte DEFAULT_VOLUME_VALUE = 20;

static const int FIRE_COUNTDOWN_INTERVAL = 90; // po jaké době se odečte jeden náboj ze zásobníku, 90ms * 95 patron = 8550ms = 8.55s
static const int LED_BLINK_INTERVAL = 45; // po jaké době se překlopí stav ledky (on/off) k napodobení záblesků

static const int SAMPLE_FIRE_FIRST = 1;
static const int SAMPLE_FIRE_LAST = 6;

static const int SAMPLE_COCK_FIRST = 10;
static const int SAMPLE_COCK_LAST = 14;

static const int SAMPLE_EMPTY_MAGAZINE_FIRST = 20;
static const int SAMPLE_EMPTY_MAGAZINE_LAST = 24;

static const int SAMPLE_MAGAZINE_EJECT = 30; // 30-31
static const int SAMPLE_MAGAZINE_LOAD = 40; // 40-42

static const int SAMPLE_GRTANADE_FIRE = 50; // 50-52

static const int SAMPLE_CARD_INSERTED = 90;
static const int SAMPLE_CARD_REMOVED = 91;
static const int SAMPLE_LOW_BATTERY = 92;
static const int SAMPLE_VOLUME_SAVED = 93;
static const int SAMPLE_WEAPON_READY = 94;
static const int SAMPLE_CLICK = 95;

static const int SAMPLE_VOLUME_UP = 70;
static const int SAMPLE_VOLUME_DOWN = SAMPLE_VOLUME_UP + 10;
