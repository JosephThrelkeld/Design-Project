//GSM Libraries
#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"

//GPS Libraries
#include <TinyGPS++.h>
class Controller {
  public:
    Controller();
    void manualCheckToSwitch();
    void toMalfuncMode();
    void toNormMode();
    void toEMS_Mode();
    void turnOff();
  private:
    enum mode {
      off,
      normalMode,
      EMS_Mode,
      malfunc_Mode
    }
    enum mode currMode;
}
class Button {
  public:
    Button();
    void checkButton();
    void stateChange();
    void manualCheckToSwitch();
    bool getDoubleTap();
    HoldType getCurrHoldType();
  private:
    bool doubleTap;
    enum HoldType currHoldType;
    int buttonState = 0;
    int lastState = 0;
    int currentState = 0;
    int pressStart = 0;
    int pressEnd = 0;
    int holdTime = 0;
    int unPressedTime = 0;
    int currTime = 0;
    HoldType
}

enum HoldType {
  noPress,
  noHold, //<1 sec
  oneSecHold, // 1< <3 sec
  threeSecHold  // 3< sec
}


//GSM objs
SMSGSM sms;
int numdata;
boolean GSM_started = false;
char smsbuffer[160];
char n[20];


//GPS objs
TinyGPSPlus gps;
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 4800;
SoftwareSerial ss(RXPin, TXPin);

//Button objs


bool doubleTap = false;
enum HoldType currHoldType = noHold;


//state var and enum


//Pin assignment
//GSM.cpp defines gsm pins as tx = 2, rx = 3
static const int buttonPin = 7;
static const int speakerPin = 8;
static const int ledPin = 9;
static const int gpsRXpin = 10;
static const int gpsTXpin = 11;

void setup() {
  Serial.begin(9600);
  //Button setup
  pinmode(7, INPUT);
  //Speaker setup
  pinmode(8, OUTPUT);
  //LED setup
  pinmode(9, OUTPUT);
  //GPS setup
  ss.begin(GPSBaud);
  //SMS setup
  //Heart rate monitor setup
}

void loop() {
  checkButton();
  switch (mode) {
    case normalMode:
    case EMS_Mode:
    case malfunc_mode:
    case off:

  }
}

//Button
void Button::checkButton() {
  buttonState = digitalRead(7);
  if (buttonState != lastState)return buttonStateChange();

  if (pressStart != 0) {
    currTime = millis();
    holdTime = currTime - pressStart;
    if (holdTime >= 3000) {
      currHoldType = threeSecHold;
    } else if (holdTime >= 1000) {
      currHoldType = oneSecHold;
    } else {
      currHoldType = noHold;
    }
  }

  manualCheckToSwitch();
  lastState = buttonState;
}

void Button::stateChange() {
  if (buttonState == HIGH) {
    pressStart = millis();
    unPressedTime = pressStart - pressEnd;
    if (unPressedTime < 500) {
      doubleTap = true;
    }
  } else {
    doubleTap = false;
    pressStart = 0;
    pressEnd = millis();
    currHoldType = noPress;
  }
}

void Controller::manualCheckToSwitch() {
  if (doubleTap && currMode == normalMode) turnOff();
  switch (currHoldType) {
    case noHold:
      break;
    case threeSecHold:
      if (mode == normalMode) toEMS_Mode();
    case oneSecHold:
      if (mode == EMS_Mode || mode == off) toNormMode();
    default:
      currHoldType = noPress;
      break;
  }
}

//States
void EMS_State();
void

//GSM
void sendEMS_SMS();

//GPS
String getGPS_Coords();

//Heartbeat sensor
int getHeartBeat();

//Switch States
void toMalfuncMode() {
  mode = malfuncMode;
}

void turnOff() {
  mode = off;
}

void toEMS_Mode() {
  mode = EMS_Mode;
}

void toNormMode() {
  mode = normalMode;
}

//Notifications:

//LED
void LED_EMS_Blink();
void LED_Malfunc_Tone();
void LED_On();
void LED_Off();

//Speaker
void normTone();
void EMS_Tone();
void off_Tone();
void malfunc_Tone();


