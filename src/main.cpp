#include <Arduino.h>
#include <Preferences.h>
#include "ui.h"
#include "const.h"
#include "bitmaps.h"

// Motor Control
#include <L298NX2.h>

// Encoders
#include "AiEsp32RotaryEncoder.h"

// Audio Board
#include "DYPlayerArduino.h"

//Button
#include "EasyButton.h"

// OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Adafruit_SSD1306.h>

// Wifi Items
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>

// OTA Updates
#include <ArduinoOTA.h>

// Web Server Items
// #include <AsyncTCP.h>
// #include <ESPAsyncWebServer.h>

// Setup preference to allow setting changes
Preferences saveData;

// Create an object for each OLED display
Adafruit_SSD1306 dA(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SSD1306 dB(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SSD1306 *display[2] = { &dA, &dB };

// Duplicate display buffers
static uint8_t displayBuffer[2][(SCREEN_WIDTH * SCREEN_HEIGHT + 7) / 8];

// Pin definition
const unsigned int IN1_A = 26; //IN1
const unsigned int IN2_A = 25; //IN2
const unsigned int IN1_B = 33; //IN3
const unsigned int IN2_B = 32; //IN4

const char* ssid = "SSID";
const char* password = "PASS";
IPAddress local_IP(192, 168, 25, 100);
IPAddress gateway(192, 168, 25, 1);
IPAddress subnet(255, 255, 255, 0);

int32_t prevRssi, RSSI;
unsigned long current_time;
unsigned long rssi_update_time;
const unsigned long rssi_period = 10000; //update period

uint8_t playTrack = 0;

unsigned long ui_update_time;
const unsigned long ui_period = 5000; //update period


int speedScale[2] = {MAXFWD, MAXFWD};
int inputSpeed[2] = {0,0};
int actualSpeed[2] = {0,0};

int direction[2] = {0,0};
int uiDIRECTION[2] = {0,0};
bool directionCHANGED[2] = {false,false};
bool dCHANGEFLAG[2] = {false,false};
bool uiSPEEDCHANGED[2] = {false, false};

int trainPWMHz[2];
bool pwmCHANGED[2] = {false,false};

uint8_t soundVol, psoundVol;

AiEsp32RotaryEncoder ERotary[2] = { 
  AiEsp32RotaryEncoder(ECLK_A, EDAT_A, ESW_A, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS), 
  AiEsp32RotaryEncoder(ECLK_B, EDAT_B, ESW_B, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS)};

//Initialize both motors
// L298NX2 trains(IN1_A, IN2_A, 1000, IN1_B, IN2_B, 1000);
L298N train[2] = {
  L298N(IN1_A, IN2_A, 1000),
  L298N(IN1_B, IN2_B, 1000)};


// Initialise the player, it defaults to using Serial.
DY::Player player(&Serial2);

EasyButton hornButton(HORN_PIN, 100, true, false);
EasyButton bellButton(BELL_PIN, 100, true, false);
EasyButton crazyButton(CRAZY_PIN, 100, true, false);
EasyButton directionAButton(DIRECTION_A_PIN, 500, true, false);
EasyButton directionBButton(DIRECTION_B_PIN, 500, true, false);

// Declare function prototypes
void disp_update_speed(uint8_t id, long value);
void disp_update_direction(uint8_t id);
void ui_update_direction(uint8_t id);
void disp_overlay(uint8_t id, uint8_t type);
bool speedControl(uint8_t id);
void commandMotor(uint8_t id);

// void train_update_speed(uint8_t id, uint8_t speed, int direction);

bool first_run = true;

// Select I2C BUS
void TCA9548A(uint8_t bus){
  Wire.beginTransmission(0x70);  // TCA9548A address
  Wire.write(1 << bus);          // send byte to select bus
  Wire.endTransmission();
}

void rotary_onButtonClick_A()
{
	static unsigned long lastTimePressed = 0;
	//ignore multiple press in that time milliseconds
	if (millis() - lastTimePressed < 500)
	{
		return;
	}
	lastTimePressed = millis();
	// Serial.print("A button pressed ");
	// Serial.print(millis());
	// Serial.println(" milliseconds after restart");
  playTrack = WHISTLE04;
}

void rotary_onButtonClick_B()
{
	static unsigned long lastTimePressed = 0;
	//ignore multiple press in that time milliseconds
	if (millis() - lastTimePressed < 500)
	{
		return;
	}
	lastTimePressed = millis();
	// Serial.print("B button pressed ");
	// Serial.print(millis());
	// Serial.println(" milliseconds after restart");
  playTrack = WHISTLE02;
}

void rotary_loop_A()
{
	if (ERotary[A].encoderChanged() || first_run)
	{
    inputSpeed[A] = map((int)ERotary[A].readEncoder(), 0, 100, 0, speedScale[A]);
		// Serial.print("A Value: ");
		// Serial.println(inputSpeed[A]);
	}
	if (ERotary[A].isEncoderButtonClicked())
	{
		rotary_onButtonClick_A();
	}
}

void rotary_loop_B()
{
	if (ERotary[B].encoderChanged() || first_run)
	{
    inputSpeed[B] = map((int)ERotary[B].readEncoder(), 0, 100, 0, speedScale[B]);
		// Serial.print("B Value: ");
		// Serial.println(inputSpeed[B]);
	}
	if (ERotary[B].isEncoderButtonClicked())
	{
		rotary_onButtonClick_B();
	}
}

void disp_update_speed(uint8_t id, long value)
{
  // value = map(value, 0, 255, 0, 100);
  TCA9548A(id);
  display[id]->setFont(&FreeSansBold24pt7b);
  display[id]->fillRect(2,18,102,45,BLACK); // Clear speed window
  display[id]->setTextSize(1);
  display[id]->setTextColor(WHITE);
  display[id]->setCursor(3, 56);
  // Display static text
  display[id]->print(value);
  display[id]->setFont(&FreeSansBold12pt7b);
  display[id]->print("%");
  display[id]->display();
}

void ui_update_direction(uint8_t id)
{
  String dname = "";
  if (direction[id] == 0){
    dname = "Forward"; 
  } else if (direction[id] == 1) {
    dname = "Reverse"; 
  } else {
    dname = "Stopped"; 
  }
  ESPUI.updateLabel(traindirectionlabel[id], dname);
}

void ui_update_sound(uint8_t id)
{
  String pname = "";
  switch (id)
  {
    case HORN01:
      pname = "Horn 1";
      break;
    case BELL01:
      pname = "Bell 1";
      break;
    case WHISTLE01:
      pname = "Whistle 1";
      break;
    case WHISTLE02:
      pname = "Whistle 2";
      break;
    case WHISTLE03:
      pname = "Whistle 3";
      break;
    case WHISTLE04:
      pname = "Whistle 4";
      break;
    case MISC01:
      pname = "Crossing 1";
      break;
    case MISC02:
      pname = "Chugging 1";
      break;
    case MISC03:
      pname = "Elephant!";
      break;
  }
  ESPUI.updateLabel(soundlabel, pname);
}

void change_volume(uint8_t vol)
{
  if (vol > 30) { vol = 30; }
  if (vol < 0) { vol = 0; }
  saveData.putUChar("volume", vol);
  player.setVolume(vol);
  psoundVol = vol;
}

void disp_update_direction(uint8_t id)
{
  TCA9548A(id);
  // display[id]->fillRect(1,1,28,14,BLACK); // Clear TOP direction box
  display[id]->fillRect(106,18,21,45,BLACK); // Clear arrow box
  display[id]->fillRect(114, 33, 6, 15, 1); // Arrow middle
  display[id]->setFont(NULL);
  display[id]->setTextColor(WHITE);
  // display[id]->setCursor(3,4); // Top Position
  if (direction[id] == 0 || direction[id] == -1){
    // display[id]->print("FWD");
    //Forward Arrow
    display[id]->fillTriangle(110, 33, 123, 33, 116, 21, 1);
    display[id]->setCursor(108, 53);
    display[id]->print("FWD");
    display[id]->display();
  } else {
    // display[id]->println("REV");
    //Reverse Arrow
    display[id]->fillTriangle(110, 48, 123, 48, 116, 60, 1);
    display[id]->setCursor(108, 22);
    display[id]->print("REV");
    display[id]->display();
  }
}

void disp_update_pwm(uint8_t id)
{
  TCA9548A(id);
  display[id]->fillRect(1,1,45,14,BLACK); // Clear TOP PWM box
  display[id]->setFont(NULL);
  display[id]->setTextColor(WHITE);
  display[id]->setCursor(3,4); // Top Position
  display[id]->print(trainPWMHz[id]);
  display[id]->print(" Hz");
  display[id]->display();
}

void disp_overlay(uint8_t id, uint8_t type)
{
  memcpy(displayBuffer[id], display[id]->getBuffer(), sizeof(displayBuffer[id]));
  // Show the thing
  memcpy(display[id]->getBuffer(), displayBuffer[id], sizeof(displayBuffer[id]));
  display[id]->display();
}

void playSound(uint8_t id)
{
  /* Sound List
  00001.mp3 - Horn 01
  00002.mp3 - Bell 01
  00003.mp3 - Whistle 01
  00004.mp3 - Whistle 02
  00005.mp3 - Whistle 03
  00006.mp3 - Whistle 04
  00007.mp3 - Chugg 01
  00008.mp3 - Crossing 01
  00009.mp3 - Elephant 01
  00010.mp3 - OPEN
  00011.mp3 - OPEN 
  */
  Serial.print("Playsound called!...");
  playTrack = 0;
  // char path[11];
  // strncpy(path, "", 11);
  int tnum = 0;
  switch (id)
  {
    case HORN01:
      // strncpy(path, "/horn01.mp3", 11);
      tnum = 1;
      break;
    case BELL01:
      // strncpy(path, "/bell01.mp3", 11);
      tnum = 2;
      break;
    case WHISTLE01:
      // strncpy(path, "/whsl01.mp3", 11);
      tnum = 3;
      break;
    case WHISTLE02:
      // strncpy(path, "/whsl02.mp3", 11);
      tnum = 4;
      break;
    case WHISTLE03:
      // strncpy(path, "/whsl03.mp3", 11);
      tnum = 5;
      break;
    case WHISTLE04:
      // strncpy(path, "/whsl04.mp3", 11);
      tnum = 6;
      break;
    case MISC01:
      // strncpy(path, "/cros01.mp3", 11);
      tnum = 8;
      break;
    case MISC02:
      // strncpy(path, "/chug01.mp3", 11);
      tnum = 7;
      break;
    case MISC03:
      tnum = 9;
      break;
  }
  // Serial.println(path);
  Serial.println(tnum);
  // player.playSpecifiedDevicePath(DY::Device::Flash, path);
  player.playSpecified(tnum);
}

void IRAM_ATTR readEncoderISR_A()
{
	ERotary[A].readEncoder_ISR();
}

void IRAM_ATTR readEncoderISR_B()
{
	ERotary[B].readEncoder_ISR();
}

void hornPressed()
{
  playTrack = HORN01;
}
void bellPressed()
{
  playTrack = BELL01;
}
void crazyPressed()
{
  playTrack = MISC03;
}
void directionAPressed()
{
  directionCHANGED[A] = true;
}
void directionBPressed()
{
  directionCHANGED[B] = true;
}

// void IRAM_ATTR horn_call() 
// {
//   static unsigned long lastTimePressed = 0;
// 	//ignore multiple press in that time milliseconds
// 	if (millis() - lastTimePressed < 500)
// 	{
// 		return;
// 	}
// 	lastTimePressed = millis();
//   playTrack = HORN01;

//   // static unsigned long last_interrupt_time = 0;
//   // unsigned long interrupt_time = millis();
//   // if (interrupt_time - last_interrupt_time > 100) // If interrupts come faster than 200ms, assume it's a bounce and ignore
//   // {
//   //   playTrack = HORN01;
//   // }
//   // last_interrupt_time = interrupt_time;
// }

// void IRAM_ATTR bell_call() 
// {
//   static unsigned long lastTimePressed = 0;
// 	//ignore multiple press in that time milliseconds
// 	if (millis() - lastTimePressed < 500)
// 	{
// 		return;
// 	}
// 	lastTimePressed = millis();
//   playTrack = BELL01;

//   // static unsigned long last_interrupt_time = 0;
//   // unsigned long interrupt_time = millis();
//   // if (interrupt_time - last_interrupt_time > 100) // If interrupts come faster than 200ms, assume it's a bounce and ignore
//   // {
//   //   playTrack = BELL01;
//   // }
//   // last_interrupt_time = interrupt_time;
// }

// void IRAM_ATTR crazy_call() 
// {
//   static unsigned long lastTimePressed = 0;
// 	//ignore multiple press in that time milliseconds
// 	if (millis() - lastTimePressed < 500)
// 	{
// 		return;
// 	}
// 	lastTimePressed = millis();
//   playTrack = MISC03;

//   // static unsigned long last_interrupt_time = 0;
//   // unsigned long interrupt_time = millis();
//   // if (interrupt_time - last_interrupt_time > 200) // If interrupts come faster than 200ms, assume it's a bounce and ignore
//   // {
//   //   playTrack = MISC03;
//   // }
//   // last_interrupt_time = interrupt_time;
// }

// void IRAM_ATTR directionA_call() 
// {
//   static unsigned long lastTimePressed = 0;
// 	//ignore multiple press in that time milliseconds
// 	if (millis() - lastTimePressed < 500)
// 	{
// 		return;
// 	}
// 	lastTimePressed = millis();
//   directionCHANGED[A] = true;

//   // static unsigned long last_interrupt_time = 0;
//   // unsigned long interrupt_time = millis();
//   // if (interrupt_time - last_interrupt_time > 300) // If interrupts come faster than 200ms, assume it's a bounce and ignore
//   // {
//   //   directionCHANGED[A] = true;
//   // }
//   // last_interrupt_time = interrupt_time;
// }

// void IRAM_ATTR directionB_call() 
// {
//   static unsigned long lastTimePressed = 0;
// 	//ignore multiple press in that time milliseconds
// 	if (millis() - lastTimePressed < 500)
// 	{
// 		return;
// 	}
// 	lastTimePressed = millis();
//   directionCHANGED[B] = true;

//   // static unsigned long last_interrupt_time = 0;
//   // unsigned long interrupt_time = millis();
//   // if (interrupt_time - last_interrupt_time > 300) // If interrupts come faster than 200ms, assume it's a bounce and ignore
//   // {
//   //   directionCHANGED[B] = true;
//   // }
//   // last_interrupt_time = interrupt_time;
// }

void initWiFi() {
  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting up....");

  Wire.begin();

  saveData.begin("trains", false);
  psoundVol = saveData.getUChar("volume", 25);
  soundVol = psoundVol;

  // pinMode(HORN_PIN, INPUT_PULLUP);
  // pinMode(BELL_PIN, INPUT_PULLUP);
  // pinMode(CRAZY_PIN, INPUT_PULLUP);
  // pinMode(DIRECTION_A_PIN, INPUT_PULLUP);
  // pinMode(DIRECTION_B_PIN, INPUT_PULLUP);

  // attachInterrupt(HORN_PIN, horn_call, RISING);
  // attachInterrupt(BELL_PIN, bell_call, RISING);
  // attachInterrupt(CRAZY_PIN, crazy_call, RISING);
  // attachInterrupt(DIRECTION_A_PIN, directionA_call, RISING);
  // attachInterrupt(DIRECTION_B_PIN, directionB_call, RISING);

  // Setup buttons
  hornButton.begin();
  bellButton.begin();
  crazyButton.begin();
  directionAButton.begin();
  directionBButton.begin();

  hornButton.onPressed(hornPressed);
  bellButton.onPressed(bellPressed);
  crazyButton.onPressed(crazyPressed);
  directionAButton.onPressed(directionAPressed);
  directionBButton.onPressed(directionBPressed);
  // directionAButton.onPressedFor(1000, directionAPressed);
  // directionBButton.onPressedFor(1000, directionBPressed);

  current_time = millis();
  rssi_update_time = 0;
  ui_update_time = 0;

  // Establish starting PWM Hz
  trainPWMHz[A] = train[A].getPWM();
  trainPWMHz[B] = train[B].getPWM();

  initWiFi();

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  TCA9548A(A);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display[A]->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Clear the buffer
  display[A]->clearDisplay();
  Serial.println("Display A Initialized.");

  TCA9548A(B);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display[B]->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Clear the buffer
  display[B]->clearDisplay();
  Serial.println("Display B Initialized.");

	//we must initialize rotary encoder
	ERotary[A].begin();
	ERotary[A].setup(readEncoderISR_A);
	bool circleValues = false;
	ERotary[A].setBoundaries(0, 100, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
	//rotaryEncoder.disableAcceleration(); //acceleration is now enabled by default - disable if you dont need it
	ERotary[A].setAcceleration(50); //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration
  
  ERotary[B].begin();
  ERotary[B].setup(readEncoderISR_B);
  ERotary[B].setBoundaries(0, 100, circleValues);
  ERotary[B].setAcceleration(50);

  player.begin();
  player.setVolume(soundVol); // (level is 0 to 30)

  // Draw initial display graphics
  for (int i=0; i <=1; i++){
    TCA9548A(i);
    display[i]->drawRect(0,0,128,16,WHITE); // Top yellow
    display[i]->drawRect(105, 17, 23, 47, 1); // Direction box
    display[i]->drawRect(0, 17, 106, 47, 1); // Speed Box

    // display[i]->setFont(NULL);
    // display[i]->setTextColor(WHITE);
    // display[i]->setCursor(3,4); // Top Position
    // if (i == A) {
    //   display[i]->print(trains.getPWMA());
    //   display[i]->print(" Hz");
    // } else {
    //   display[i]->print(trains.getPWMB());
    //   display[i]->print(" Hz");
    // }

    display[i]->display();

    disp_update_direction(i);
    disp_update_pwm(i);
    ui_update_direction(i);
  }

  //Put train on display B for funsies
  TCA9548A(1);
  // display[B]->drawBitmap(30, 1, epd_bitmap_train_car_tank, 23, 14, 1);
  // display[B]->drawBitmap(54, 1, epd_bitmap_train_car_box_full, 23, 14, 1);
  // display[B]->drawBitmap(78, 1, epd_bitmap_train_car_gondola_full, 23, 14, 1);
  display[B]->drawBitmap(102, 1, epd_bitmap_train_car_caboose, 23, 14, 1);
  display[B]->display();

  // Start UI
  setUpUI();

}

void loop() {
  // put your main code here, to run repeatedly:
  ArduinoOTA.handle();

  rotary_loop_A();
  rotary_loop_B();

  hornButton.read();
  bellButton.read();
  crazyButton.read();
  directionAButton.read();
  directionBButton.read();

  if (playTrack != 0 && (int)player.checkPlayState() == 0)
  {
    ui_update_sound(playTrack);
    playSound(playTrack);
  }

  if (soundVol != psoundVol)
  {
    // UI value changed
    change_volume(soundVol);
  }

  current_time = millis();

  if (current_time - rssi_update_time >= rssi_period || first_run) {
    int bars;
    RSSI = WiFi.RSSI();
    Serial.print("Signal strength: ");
    Serial.println(RSSI);
    
    if (RSSI > -60) { 
      bars = 4;
    } else if (RSSI <= -60 & RSSI > -70) {
      bars = 3;
    } else if (RSSI <= -70 & RSSI > -75) {
      bars = 2;
    } else if (RSSI <= -75 & RSSI > -82) {
      bars = 1;
    } else {
      bars = 0;
    }

    // display.clearDisplay();
    TCA9548A(A);
    display[A]->fillRect(107,1,19,14,BLACK); // Clear wifi box
  // Do some simple loop math to draw rectangles as the bars
  // Draw one bar for each "bar" Duh...
    for (int b=0; b <= bars; b++) {
      display[A]->fillRect(107 + (b*4),12 - (b*2),2,b*2,WHITE); 
    }
    display[A]->display();

    rssi_update_time = millis();
  }
  
  // Handle direction changes
  for (int i=0; i <= 1; i++) {
    if (directionCHANGED[i]){
      if (direction[i] == FORWARD){
        if (actualSpeed[i] == 0) { direction[i] = REVERSE; }
        speedScale[i] = MAXREV;
        inputSpeed[i] = map(inputSpeed[i], 0, MAXFWD, 0, speedScale[i]);
      } else if (direction[i] == REVERSE) {
        if (actualSpeed[i] == 0) { direction[i] = FORWARD; }
        speedScale[i] = MAXFWD;
        inputSpeed[i] = map(inputSpeed[i], 0, MAXREV, 0, speedScale[i]);
      }
      #ifdef DEBUG
        Serial.print("Direction Changed! Value: ");
        Serial.println(direction[i]);
      #endif

      //Show direction change arrow if @ speed
      if (actualSpeed[i] != 0)
      {
        TCA9548A(i);
        display[i]->fillRect(56,1,24,14,BLACK); // Clear dir box
        display[i]->fillRect(60, 4, 12, 3, 1);
        display[i]->fillRect(62, 9, 12, 3, 1);
        display[i]->fillTriangle(56, 5, 60, 2, 60, 9, 1);
        display[i]->fillTriangle(78, 10, 74, 6, 74, 13, 1);
        display[i]->display();
      }

      directionCHANGED[i] = false;
    }

  }

  // Handle direction change display
  for (int i=0; i <= 1; i++) {
    if (uiDIRECTION[i] != direction[i]){
      ui_update_direction(i);
      disp_update_direction(i);
      display[i]->fillRect(56,1,24,14,BLACK); // Clear dir box
      uiDIRECTION[i] = direction[i];
    }
  }

  //Control Speed
  bool schange[2];
  schange[A] = speedControl(A);
  schange[B]= speedControl(B);

  // Handle pwm changes
  for (int i=0; i <= 1; i++) {
    if (pwmCHANGED[i]){
      train[i].setPWM(trainPWMHz[i]);
      disp_update_pwm(i);
      pwmCHANGED[i] = false;
    }
  }

  //Sync Displays, encoders, sliders
  for (int i=0; i <= 1; i++) {
    if (schange[i] || first_run) {
      #ifdef DEBUG
        Serial.print("Train ");
        Serial.print(i);
        Serial.print(" speed: ");
        Serial.println(actualSpeed[i]);
      #endif

      // For display purposes use absolute value
      long speed100 = map(abs(actualSpeed[i]), 0, abs(speedScale[i]), 0, 100);
      disp_update_speed(i, speed100);
      ESPUI.updateSlider(trainSlider[i], speed100);
    }
  } 

  if (millis() - ui_update_time >= ui_period || first_run) {
    //update UI
    for (int i=0; i <= 1; i++){
      ui_update_direction(i);
    }
    ESPUI.updateSlider(volSlider, soundVol);
    ui_update_time = millis();
  }

  // Items to complete on initial start only
  if (first_run){
    for (int i=0; i <= 1; i++){
      disp_update_pwm(i);
      ESPUI.updateText(trainPWM[i], String(trainPWMHz[i]));  
    }
  }

  if (first_run){ first_run = false; }
}



bool speedControl(uint8_t id)
{
  // Update encoder if UI changes input speed
  for (int i=0; i <= 1; i++){
    // int emap = map((int)ERotary[i].readEncoder(), 0, 100, 0, speedScale[i]);
    if (uiSPEEDCHANGED[i]){
    // if (emap != inputSpeed[i]){
      long eimap = map(inputSpeed[i], 0, speedScale[i], 0, 100);
      ERotary[i].setEncoderValue(eimap);
      uiSPEEDCHANGED[i] = false;
    }
  }

  // Check for opposite signs to detect direction change
  int SRATE;
  if ((inputSpeed[id] ^ actualSpeed[id]) < 0){
    dCHANGEFLAG[id] = true;
    #ifdef DEBUG
      Serial.print("Train ");
      Serial.print(id);
      Serial.println(" detected direction change, slowing rate!");
    #endif
  } 
  
  if (inputSpeed[id] != actualSpeed[id] && dCHANGEFLAG[id]) {
    SRATE = CHANGE_RATE;
  } 
  else if (inputSpeed[id] == actualSpeed[id] && dCHANGEFLAG[id])
  {
    dCHANGEFLAG[id] = false;
    SRATE = SPEED_RATE;
  }
  else
  {
    SRATE = SPEED_RATE;
  }

  int call_Rate = inputSpeed[id] - actualSpeed[id];
  if (inputSpeed[id] > actualSpeed[id])
  {
    if ( abs(call_Rate) <= SRATE ){
      actualSpeed[id] = inputSpeed[id];
    } else {
      actualSpeed[id] += SRATE;
    }
    if (actualSpeed[id] > 0){ direction[id] = FORWARD; }
    else if (actualSpeed[id] < 0){ direction[id] = REVERSE; }
    commandMotor(id);
    return true;
  }
  else if (inputSpeed[id] < actualSpeed[id])
  {
    if ( abs(call_Rate) <= SRATE ){
      actualSpeed[id] = inputSpeed[id];
    } else {
      actualSpeed[id] -= SRATE;
    }
    if (actualSpeed[id] > 0){ direction[id] = FORWARD; }
    else if (actualSpeed[id] < 0){ direction[id] = REVERSE; }
    commandMotor(id);
    return true;
  }
  return false;
}

void commandMotor(uint8_t id)
{
  // bounds check, if greater than max, set it to max. This is need if the accel/decel rate is not a perfect multiple of the max speed(s)
  if (actualSpeed[id] > 254) { actualSpeed[id]=255; }
  // same deal, except negative.
  if (actualSpeed[id] < -254) { actualSpeed[id] = -255; }

  //send command to trains
  if (actualSpeed[id] == 0) //Stopped
  {
    train[id].setSpeed(actualSpeed[id]);
    train[id].run((L298N::Direction)direction[id]);
  }
  else if (actualSpeed[id] > 0) //Forward
  {
    train[id].setSpeed(actualSpeed[id]);
    train[id].run((L298N::Direction)direction[id]);
  }
  else // Reverse
  {
    train[id].setSpeed(-1*actualSpeed[id]);
    train[id].run((L298N::Direction)direction[id]);
  }
}
