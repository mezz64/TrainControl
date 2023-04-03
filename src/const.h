#pragma once
// Define constants utilized

// Comment out to disable debug serial prints
#define DEBUG

// OLED Items
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  //< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

#define HORN_PIN 36
#define BELL_PIN 4 //39
#define CRAZY_PIN 23 //35

#define DIRECTION_A_PIN 34
#define DIRECTION_B_PIN 27

#define ECLK_A 19 //CLK
#define EDAT_A 18
#define ESW_A 5

#define ECLK_B 13
#define EDAT_B 14
#define ESW_B  15
#define ROTARY_ENCODER_VCC_PIN -1 // 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder
#define ROTARY_ENCODER_STEPS 4  //depending on your encoder - try 1,2 or 4 to get expected behaviour

#define SPEED_RATE 10
#define CHANGE_RATE 1
#define MAXFWD 255
#define MAXREV -255

#define HORN01 1
#define BELL01 2
#define CRAZY 3
#define WHISTLE01 4
#define WHISTLE02 5
#define WHISTLE03 6
#define WHISTLE04 7 //toy
#define MISC01 8 //Crossing
#define MISC02 9 //Chug
#define MISC03 10 //Elephant

#define A 0  
#define B 1

const int FORWARD = 0;
const int REVERSE = 1;
const int STOP = -1;

extern uint8_t playTrack;

extern uint8_t inputSpeedE[2];
extern uint8_t inputSpeedW[2];
extern int inputSpeed[2];
extern int actualSpeed[2];
extern int speedScale[2];
extern int direction[2]; //0 - Forward, 1 - Backward, -1 - Stop
extern bool directionCHANGED[2];
extern bool uiSPEEDCHANGED[2];

extern uint8_t soundVol;
extern int trainPWMHz[2];
extern bool pwmCHANGED[2];
