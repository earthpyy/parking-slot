#include <Arduino.h>

// settings
const int maxSlot = 10; // max possible slot in parking lot
const int delayTime = 10; // delay time when each digit show
const boolean autoCalibrate = true; // auto calibration before start
const boolean debug = false; // open 'Serial Monitor'

// calibration
int divider[2] = {40, 15}; // default divider if 'autoCalibrate' is off
const int divideValue = 10; // divide length used in auto calibration
const int calibrationCountdown = 3; // countdown before calibration start

// variable
int val;
boolean carExist[2] = {false, false};
int slotLeft = maxSlot;

// port config
const int bcdPins[4] = {0, 1, 2, 3};
const int transistorPins[2] = {5, 4};
const int ldrPins[2] = {A1, A0};

void setup() {
  // debug
  if (debug)
    Serial.begin(9600);
  for (int i = 0; i < 2; i++)
    pinMode(transistorPins[i], OUTPUT);
  for (int i = 0; i < 4; i++)
    pinMode(bcdPins[i], OUTPUT);
  for (int i = 0; i < 2; i++)
    pinMode(ldrPins[i], INPUT);

  // auto calibration
  if (autoCalibrate) {
    for (int i = calibrationCountdown; i > 0; i--) {
      for (int j = 0; j < 50; j++) // show each digit 1 second
        showDigit(i); // show time remaining
    }
    for (int i = 0; i < 2; i++) {
      divider[i] = analogRead(ldrPins[i]) + divideValue;
    }
    for (int j = 0; j < 50; j++) // show digit 1 second
      showDigit(99); // show 99 means calibrating
    while (analogRead(ldrPins[0]) < divider[0] && analogRead(ldrPins[1] < divider[1])) {
      showDigit(0); // show 00 means calibration completed
    }
  }
}

void loop() {
  for (int i = 0; i < 2; i++) {
    val = analogRead(ldrPins[i]);
    if (val < divider[i]) {
      carExist[i] = true;
    } else {
      if (carExist[i]) {
        if (i == 0 && slotLeft > 0) { // in
          slotLeft--;
        } else if (i == 1 && slotLeft < maxSlot) { // out
          slotLeft++;
        } else if (i == 0 && slotLeft == 0) {
          while (true) {
            showDigit(0);
            val = analogRead(ldrPins[1]);
            if (val < divider[1]) {
              carExist[1] = true;
            } else {
              if (carExist[1]) {
                slotLeft = -1;
                // delay(delayTime);
                break;
              }
              carExist[1] = false;
            }
          }
        }
      }
      carExist[i] = false;
    }
  }

  showDigit(slotLeft);

 // debug
 if (debug) {
   Serial.print(analogRead(ldrPins[0]));
   Serial.print(" | ");
   Serial.println(analogRead(ldrPins[1]));
 }
}

void showDigit(int num) {
  digitalWrite(transistorPins[0], 1);
  digitalWrite(transistorPins[1], 0);
  sendDigit(num / 10);
  delay(delayTime);
  digitalWrite(transistorPins[0], 0);
  digitalWrite(transistorPins[1], 1);
  sendDigit(num % 10);
  delay(delayTime);
}

void sendDigit(int dec) {
  digitalWrite(bcdPins[0], dec % 2);
  dec /= 2;
  digitalWrite(bcdPins[1], dec % 2);
  dec /= 2;
  digitalWrite(bcdPins[2], dec % 2);
  dec /= 2;
  digitalWrite(bcdPins[3], dec % 2);
}
