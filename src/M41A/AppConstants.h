#pragma once

#include <Arduino.h>

// Use pins 2 and 3 to communicate with DFPlayer Mini
static constexpr uint8_t PIN_MP3_TX = 2; // Connects to module's RX
static constexpr uint8_t PIN_MP3_RX = 3; // Connects to module's TX

static constexpr int MAX_VOLUME = 22; // DFPlayer has a volume range from 0 to 30, but it freezes at 24.
static constexpr int VOLUME_ADDRESS = 10; // EEPROM address for volume storage
static constexpr int INIT_FLAG_ADDRESS = 9;  // Address for depositing the control flag
static constexpr byte INITIALIZED_FLAG = 0xA5;  // Some specific value for checking initialization, typically greater than MAX_VOLUME, to indicate that the volume has not yet been stored.
static constexpr byte DEFAULT_VOLUME_VALUE = 20;

static constexpr int MAX_BULLETS_COUNT = 95; // Count of bullets in full magazine

// Pin assignments
constexpr int RIFLE_TRIGGER_PIN = A2;        // Rifle trigger button
constexpr int MAGAZINE_RELOAD_PIN = A3;      // Magazine reload button
constexpr int RIFLE_COCK_PIN = A4;           // Rifle cocking button
constexpr int GRENADE_TRIGGER_PIN = A5;      // Grenade launcher trigger button
constexpr int RESERVED_BUTTON_PIN = A6;      // Reserved button

constexpr int ROTARY_S1_PIN = 9;             // Volume rotary encoder (S1)
constexpr int ROTARY_S2_PIN = 10;            // Volume rotary encoder (S2)
constexpr int ROTARY_BUTTON_PIN = 8;         // Volume rotary encoder button (KEY)

constexpr int LED_RIFLE_PIN = A0;            // LED for rifle barrel
constexpr int LED_GRENADE_PIN = 5;           // LED for grenade barrel (must be PWM for animation)

// Connections to 74HC595 shift register
constexpr int LATCH_PIN = 12;                // 74HC595 RCLK  - ST_CP (LOAD)
constexpr int CLOCK_PIN = 11;                // 74HC595 SRCLK - SH_CP (SCLK)
constexpr int DATA_PIN = 13;                 // 74HC595 SER   - DS    (SDD)

// Time constants
constexpr int VOLUME_DISPLAY_TIMEOUT = 3000; // Time in milliseconds for how long the volume will be displayed on the screen
// after how long it takes to substract one bullet from the magazine.
// My sound files for rifle shoot takes 8.5-9 seconds, so tis is the calculation of substraction interval:
// 90ms * 95 bullets = 8550ms = 8.55s
constexpr int FIRE_COUNTDOWN_INTERVAL = 90;  // Interval in milliseconds between each bullet subtraction during firing
constexpr int RIFLE_FIRE_FLASH_TIMEOUT = 60; // Duration in milliseconds for the rifle LED flash during firing
constexpr int GRENADE_FLASH_FADEOUT_TIME = 700; // Duration in milliseconds for the grenade LED fadeout animation
constexpr int TRANSITION_DURATION_PER_10_UNITS = 60; // Duration in milliseconds for each 10 units of transition in displayAnimator

// Other constants
constexpr unsigned long LONG_PRESS_TIME = 1000; // Duration in milliseconds to detect a long button press

// I have some sounds in multiple variants and I randomly choose which variant to play.
// For this there are FIRST and LAST containers, which define the file numbers of the sounds of a given type.

// Sound file interval for rifle fire
static constexpr int SAMPLE_RIFLE_FIRE_FIRST = 1;
static constexpr int SAMPLE_RIFLE_FIRE_LAST = 6;

// Sound file interval for cock sound
static constexpr int SAMPLE_COCK_FIRST = 10;
static constexpr int SAMPLE_COCK_LAST = 14;

// Sound file interval for empty magazine, you press rifle fire and this empty file will play
static constexpr int SAMPLE_EMPTY_MAGAZINE_FIRST = 20;
static constexpr int SAMPLE_EMPTY_MAGAZINE_LAST = 24;

// Sound file interval for ejecting of the magazine
static constexpr int SAMPLE_MAGAZINE_EJECT_FIRST = 30;
static constexpr int SAMPLE_MAGAZINE_EJECT_LAST = 31;

// Sound file interval for loading of the magazine
static constexpr int SAMPLE_MAGAZINE_LOAD_FIRST = 40;
static constexpr int SAMPLE_MAGAZINE_LOAD_LAST = 42;

// Sound file interval for ejecting + loading magazine + cock
// it depends how you make your own sound files and which hardware you use.
static constexpr int SAMPLE_MAGAZINE_RELOAD_FIRST = 53;
static constexpr int SAMPLE_MAGAZINE_RELOAD_LAST = 54;

// This Grenade load sound file includes actions:
// open chamber + put grenade to chamber + close chamber. 3 actions in one sound file.
// Again it depends on hardware. My 3D printed M41A has no functional grenade mechanics.
static constexpr int SAMPLE_GRENADE_LOAD = 65;
static constexpr int SAMPLE_GRENADE_FIRE = 60; // 60-62 - variations
static constexpr int SAMPLE_GRENADE_EXPLOSION = 66; // 66-67 - variations

// NOT USED comment means: DF Player is very simple audio device and playing different sounds in short frequency of change is too much for it.

static constexpr int SAMPLE_CARD_INSERTED = 90; // NOT USED
static constexpr int SAMPLE_CARD_REMOVED = 91; // NOT USED
static constexpr int SAMPLE_LOW_BATTERY = 92; // NOT USED
static constexpr int SAMPLE_VOLUME_SAVED = 93; // It say: Volume saved
static constexpr int SAMPLE_WEAPON_READY = 94; // NOT USED
static constexpr int SAMPLE_CLICK = 95; // NOT USED

// Volume sounds are organised to two segments.
// I distinguish between VOLUME UP and VOLUME DOWN sound.
// Each of these operations has its own sound.
// I have prepared these in several sets and they are used in pairs. For example: 70-80, 71-81, 72-82
// To make the sounds easier to work around the DOWN is always 10 larger. That's the rule.
static constexpr int SAMPLE_VOLUME_UP = 70; // NOT USED
static constexpr int SAMPLE_VOLUME_DOWN = SAMPLE_VOLUME_UP + 10; // NOT USED

// These are the interval constants for how long it takes to hear the explosion after the grenade is fired. Values are in milliseconds.
// So each shot takes a randomly varying amount of time between MIN and MAX.
static constexpr int GRENADE_BALLISTIC_DELAY_MIN = 500;
static constexpr int GRENADE_BALLISTIC_DELAY_MAX = 3000;
