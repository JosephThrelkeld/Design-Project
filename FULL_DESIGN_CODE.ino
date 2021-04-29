//GSM Libraries
#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"

#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>

#include <TinyGPS++.h>

int countDownStart;

void checkButton();
void button_StateChange();
void manualCheckToSwitch();
int buttonState = 0;
int lastState = 0;
int currentState = 0;
int pressStart = 0;
int pressEnd = 0;
int holdTime = 0;
int unPressedTime = 0;
unsigned long currTime = 0;

enum HoldType {
  noPress,
  noHold, //<1 sec
  oneSecHold, // 1< <3 sec
  threeSecHold  // 3< sec
};
bool doubleTap = false;
HoldType currHoldType = noPress;

unsigned long currLEDTime = 0;
unsigned long previousLEDTime = 0;
const unsigned int emsInterval = 250;
int ledState = LOW;

unsigned long currSpeakerTime = 0;
unsigned long previousSpeakerTime = 0;
bool outputTone = false;
const int emsTone = 262;

enum Mode {
  off,
  normalMode,
  EMS_Mode,
  malfuncMode
};
Mode mode = off;


static const int buttonPin = 7;
static const int speakerPin = 8;
static const int ledPin = 9;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

}

void loop() {
  checkButton();
  switch (mode) {
    case off:
      Serial.println("off");
      if (ledState != LOW) {
        ledState = LOW;
        digitalWrite(ledPin, ledState);
      }
      break;
    case normalMode:
      Serial.println("normalMode");
      if (ledState != HIGH) {
        ledState = HIGH;
        digitalWrite(ledPin, ledState);
      }
      break;
    case EMS_Mode:
      Serial.println("EMS_Mode");
      currTime = millis();
      currLEDTime = millis();
      currSpeakerTime = millis();
      
      if (currLEDTime - previousLEDTime >= emsInterval) {
        previousLEDTime = currLEDTime;

        if (ledState == LOW) {
          ledState = HIGH;
        } else {
          ledState = LOW;
        }
        digitalWrite(ledPin, ledState);
      }
      
      if (outputTone) {
        if (currSpeakerTime - previousSpeakerTime >= emsInterval) {
          previousSpeakerTime = currSpeakerTime;
          noTone(speakerPin);
          outputTone = false;
        }
      } else if (currSpeakerTime - previousSpeakerTime >= emsInterval) {
        previousSpeakerTime = currSpeakerTime;
        tone(speakerPin, emsTone);
        outputTone = true;

      }
      
      if (countDownStart == 0) countDownStart = millis();
      else if (currTime - countDownStart > 30000) {
        //send sms
        noTone(speakerPin);
        outputTone = false;
        countDownStart = 0;
        mode = normalMode;
      }
      break;
      
    case malfuncMode:
      Serial.println("malfuncMode");
      break;
  }
}

void toMalfuncMode() {
  mode = malfuncMode;
}

void turnOff() {
  tone(speakerPin, 262);
  delay(100);
  noTone(speakerPin);
  delay(100);
  tone(speakerPin, 131);
  delay(100);
  noTone(speakerPin);
  mode = off;
}

void toEMS_Mode() {
  mode = EMS_Mode;
}

void toNormMode() {
  tone(speakerPin, 262);
  delay(100);
  noTone(speakerPin);
  delay(100);
  tone(speakerPin, 262);
  delay(100);
  noTone(speakerPin);
  mode = normalMode;
}

void checkButton() {
  buttonState = digitalRead(7);
  if (buttonState != lastState)buttonStateChange();

  if (buttonState == HIGH) {
    currTime = millis();
    holdTime = currTime - pressStart;
    if (holdTime >= 3000) {
      Serial.println("threeSecHold");
      currHoldType = threeSecHold;
    } else if (holdTime >= 1000) {
      Serial.println("oneSecHold");
      currHoldType = oneSecHold;
    } else {
      Serial.println("noHold");
      currHoldType = noHold;
    }
  }

  manualCheckToSwitch();
  lastState = buttonState;
}

void buttonStateChange() {
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


void manualCheckToSwitch() {
  if (doubleTap && mode == normalMode) turnOff();
  switch (currHoldType) {
    case noHold:
      break;
    case threeSecHold:
      if (mode == normalMode) toEMS_Mode();
      currHoldType = noPress;
      break;
    case oneSecHold:
      if (mode == EMS_Mode || mode == off) toNormMode();
    default:
      currHoldType = noPress;
      break;
  }
}
