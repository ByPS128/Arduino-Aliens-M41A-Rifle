#pragma once

#include <Arduino.h>

// Use pins 2 and 3 to communicate with DFPlayer Mini
static const uint8_t PIN_MP3_TX = 2; // Connects to module's RX
static const uint8_t PIN_MP3_RX = 3; // Connects to module's TX

static const int MAX_VOLUME = 22; // DFPlayer has a volume range from 0 to 30, but it freezes at 24.
static const int VOLUME_ADDRESS = 10; // EEPROM address for volume storage
static const int INIT_FLAG_ADDRESS = 9;  // Address for depositing the control flag
static const byte INITIALIZED_FLAG = 0xA5;  // Some specific value for checking initialization, typically greater than MAX_VOLUME, to indicate that the volume has not yet been stored.
static const byte DEFAULT_VOLUME_VALUE = 20;

static const int MAX_BULETS_COUNT = 95; // Count of bulets in full magazine

// after how long it takes to substract one bullet from the magazine.
// My sound files for rifle shoot takes 8.5-9 seconds, so tis is the calculation of substraction interval:
// 90ms * 95 bullets = 8550ms = 8.55s
static const int FIRE_COUNTDOWN_INTERVAL = 90; 

static const int RIFLE_FIRE_FLASH_TIMEOUT = 60; // After what time does the LED state (on/off) switch to mimic flashes.

// I have some sounds in multiple variants and I randomly choose which variant to play.
// For this there are FIRST and LAST containers, which define the file numbers of the sounds of a given type.

// Sound file interval for rifle fire
static const int SAMPLE_RIFLE_FIRE_FIRST = 1;
static const int SAMPLE_RIFLE_FIRE_LAST = 6;

// Sound file interval for cock sound
static const int SAMPLE_COCK_FIRST = 10;
static const int SAMPLE_COCK_LAST = 14;

// Sound file interval for empty magazine, you rpes rifle fire and this empty file will play
static const int SAMPLE_EMPTY_MAGAZINE_FIRST = 20;
static const int SAMPLE_EMPTY_MAGAZINE_LAST = 24;

// Sound file interval for ejecting of the magazine
static const int SAMPLE_MAGAZINE_EJECT_FIRST = 30;
static const int SAMPLE_MAGAZINE_EJECT_LAST = 31;

// Sound file interval for loading of the magazine
static const int SAMPLE_MAGAZINE_LOAD_FIRST = 40;
static const int SAMPLE_MAGAZINE_LOAD_LAST = 42;

// Sound file interval for ejecting + loading magazine + cock
// it depends how you make your own sound files and which hardware you use.
static const int SAMPLE_MAGAZINE_RELOAD_FIRST = 53;
static const int SAMPLE_MAGAZINE_RELOAD_LAST = 54;

// This Granade load sound file includes actions:
// open chamber + put granade to chamber + close chamber. 3 actions in one sound file.
// Again it depends on hardware. My 3D printed M41A has no functional granade mechanics.
static const int SAMPLE_GRANADE_LOAD = 65;
static const int SAMPLE_GRANADE_FIRE = 60; // 60-62 - variations
static const int SAMPLE_GRANADE_EXPLOSION = 66; // 66-67 - variations

// NOT USED comment means: DF Player is very simple audio device and playing doferent sounds in short frequency of changhe is tooooo much for it.

static const int SAMPLE_CARD_INSERTED = 90; // NOT USED
static const int SAMPLE_CARD_REMOVED = 91; // NOT USED
static const int SAMPLE_LOW_BATTERY = 92; // NOT USED
static const int SAMPLE_VOLUME_SAVED = 93; // It say: Volume saved
static const int SAMPLE_WEAPON_READY = 94; // NOT USED
static const int SAMPLE_CLICK = 95; // NOT USED

// Volume sounds are organised to two segments.
// I distinguish between VOLUME UP and VOLUME DOWN sound.
// Each of these operations has its own sound.
// I have prepared these in several sets and they are used in pairs. For example: 70-80, 71-81, 72-82
// To make the sounds easier to work around the DOWN is always 10 larger. That's the rule.
static const int SAMPLE_VOLUME_UP = 70; // NOT USED
static const int SAMPLE_VOLUME_DOWN = SAMPLE_VOLUME_UP + 10; // NOT USED

// These are the interval constants for how long it takes to hear the explosion after the grenade is fired. Values are in milliseconds.
// So each shot takes a randomly varying amount of time between MIN and MAX.
static const int GRANADE_BALISTIC_DELAY_MIN = 500;
static const int GRANADE_BALISTIC_DELAY_MAX = 3000;

// FADE OUT is the animation time of the grenade launcher in milliseconds.
// When a grenade is fired, the flare lights up to 100% and goes out for this constant.
static const int GRANADE_FLASH_FADEOUT_TIME = 700;

// The Rotary Encoder used is with push button and RC function.
//I distinguish between short and long press of the encoder button.
//A long press is detected by knowing that the button has been pressed longer than the horn of this constant in milliseconds.
static const unsigned long LONG_PRESS_TIME = 1000;  // 1000 milisekund pro dlouhý stisk
