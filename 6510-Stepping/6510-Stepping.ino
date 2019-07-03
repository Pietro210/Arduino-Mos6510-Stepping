/**
 * MOS6510 CPU Stepping
 *
 * (c) 2014 Robert Crossfield
 **/

#include "memory.h"

/*
BOARD: Uno, Nano, Mini
PWM PINS: 3, 5, 6, 9, 10, 11
PWM FREQUENCY: 490 Hz (pins 5 and 6: 980 Hz)

BOARD: Mega
PWM PINS: 2 - 13, 44 - 46
PWM FREQUENCY: 490 Hz (pins 4 and 13: 980 Hz)
*/
int pinClock = 4;

int pinReady = 2;
int pinReset = 3;
int pinRW = 5;
int pinsData[8] = { 6, 7, 8, 9, 10, 11, 12, 13 };

bool autorun = false;
bool wasWrite = false;
uint16_t previousAddress = 0;

String commandInput = "";
boolean commandComplete = false;

uint16_t getAddress() {
  uint16_t address = 0;

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
  pinMode(A8, INPUT);
  pinMode(A9, INPUT);
  pinMode(A10, INPUT);
  pinMode(A11, INPUT);
  pinMode(A12, INPUT);
  pinMode(A13, INPUT);
  pinMode(A14, INPUT);
  pinMode(A15, INPUT);

  bitWrite(address, 0, digitalRead(A0));
  bitWrite(address, 1, digitalRead(A1));
  bitWrite(address, 2, digitalRead(A2));
  bitWrite(address, 3, digitalRead(A3));
  bitWrite(address, 4, digitalRead(A4));
  bitWrite(address, 5, digitalRead(A5));
  bitWrite(address, 6, digitalRead(A6));
  bitWrite(address, 7, digitalRead(A7));
  bitWrite(address, 8, digitalRead(A8));
  bitWrite(address, 9, digitalRead(A9));
  bitWrite(address, 10, digitalRead(A10));
  bitWrite(address, 11, digitalRead(A11));
  bitWrite(address, 12, digitalRead(A12));
  bitWrite(address, 13, digitalRead(A13));
  bitWrite(address, 14, digitalRead(A14));
  bitWrite(address, 15, digitalRead(A15));

  return address;
}

uint8_t getData() {
  for (int pin = 0; pin < 8; pin++) {
    pinMode(pinsData[pin], INPUT);
  }

  uint8_t data;

  for (int pin = 0; pin < 8; pin++) {
    bitWrite(data, pin, digitalRead(pinsData[pin]));
  }

  return data;
}

void setData(uint8_t data) {
  for (int pin = 0; pin < 8; pin++) {
    pinMode(pinsData[pin], OUTPUT);
  }

  for (int pin = 0; pin < 8; pin++) {
    if (data & 1) {
      digitalWrite(pinsData[pin], HIGH);
    }
    else {
      digitalWrite(pinsData[pin], LOW);
    }
    data >>= 1;
  }
}

void reset() {
  digitalWrite(pinReset, LOW);
  delay(10);
  digitalWrite(pinReset, HIGH);
  Serial.println("reset");
}

void step() {
  digitalWrite(pinReady, HIGH);
  delay(1);
  digitalWrite(pinReady, LOW);
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();

    commandInput += inChar;
    if (inChar == '\n') {
      commandComplete = true;
    }
  }
}

void setup() {
  commandInput.reserve(16);

  Serial.begin(9600);
  Serial.println("start");

  pinMode(pinClock, OUTPUT);
  pinMode(pinReady, OUTPUT);
  pinMode(pinReset, OUTPUT);
  pinMode(pinRW, INPUT);

  digitalWrite(pinReady, LOW);
  digitalWrite(pinReset, HIGH);

  analogWrite(pinClock, 127); // 50% on, 50% off
}

void loop() {
  bool isWrite = digitalRead(pinRW) == LOW;
  uint16_t address = getAddress();
  uint8_t data;
  char buff[16];

  if (isWrite) {
    data = getData();
  }
  else {
    if (address == 0xfffc) {
      data = 0x02;
    }
    else if (address == 0xfffd) {
      data = 0x00;
    }
    else {
      data = memory[address];
    }

    setData(data);
  }

  if (isWrite != wasWrite || address != previousAddress) {
    if (isWrite) {
      Serial.print('W');
    }
    else {
      Serial.print('R');
    }

    sprintf(buff, " 0x%04x", address);
    Serial.print(buff);

    Serial.print(" data: ");

    sprintf(buff, "0x%02x", data);
    Serial.print(buff);
    Serial.println();
  }

  wasWrite = isWrite;
  previousAddress = address;

  if (commandComplete == true) {
    commandComplete = false;

    if (commandInput == "r\n") {
      reset();
    }
    if (commandInput == "z\n") {
      step();
    }
    if (commandInput == "a\n") {
      autorun = true;
    }
    if (commandInput == "s\n") {
      autorun = false;
    }

    commandInput = "";
  }

  if (autorun) {
    step();
    delay(100);
  }
  else {
    delay(10);
  }
}
