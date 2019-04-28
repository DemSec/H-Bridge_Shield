#define HAVE_HWSERIAL1
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
  // set up software serial to allow serial communication to our TX and RX pins
  SoftwareSerial Serial1(10, 11);
#endif

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  Serial.begin(115200);   // USB serial port of Due (PROGRAMMING)
  Serial1.begin(115200);  // ESP8266 on Serial Port 1 of Due (UART 1)
}

char rx_byte = 0;
int fwd = 0;
int bwd = 0;
int lft = 0;
int rgt = 0;
long timeMotor1 = 0;
long timeMotor2 = 0;
bool led = 0;

void loop() {
  // send terminal byte to ESP8266
  if (Serial.available() > 0) {
    rx_byte = Serial.read();
    Serial1.print(rx_byte);
  }
  // send ESP8266 byte to terminal
  if (Serial1.available() > 0) {
    rx_byte = Serial1.read();
    switch(rx_byte) {
      case 'F': if (lft == 1) {lft = 2;} else {AllCountsReset(); fwd = 1;} break;
      case 'W': if (fwd == 1) {fwd = 2;} else if (bwd == 1) {bwd = 2;} else {AllCountsReset();} break;
      case 'D': if (fwd == 2) {fwd = 3;} else if (bwd == 2) {bwd = 3;} else {AllCountsReset();} break;
      case 'B': AllCountsReset(); bwd = 1; break;
      case 'L': AllCountsReset(); lft = 1; break;
      case 'T': if (lft == 2) {lft = 3;} else if (rgt == 2) {rgt = 3;} else {AllCountsReset();} break;
      case 'R': AllCountsReset(); rgt = 1; break;
      case 'G': if (rgt == 1) {rgt = 2;} else {AllCountsReset();} break;
      default: AllCountsReset(); break;
    }
    if (fwd == 3) {
      AllCountsReset();
      Forward();
    }
    if (bwd == 3) {
      AllCountsReset();
      Backward();
    }
    if (lft == 3) {
      AllCountsReset();
      Left();
    }
    if (rgt == 3) {
      AllCountsReset();
      Right();
    }
    Serial.print(rx_byte);
  }
  MotorsUpdate();
}

void AllCountsReset() {
  fwd = 0;
  bwd = 0;
  lft = 0;
  rgt = 0;
}

void Forward() {
  SetMotor(1, 1, 1);
}

void Backward() {
  SetMotor(1, -1, 1);
}

void Left() {
  SetMotor(2, 1, 1);
}

void Right() {
  SetMotor(2, -1, 1);
}

void StopAll() {
  SetMotor(1, 0, 0);
  SetMotor(2, 0, 0);
}

void StopMoving() {
  SetMotor(1, 0, 0);
}

void StopTurning() {
  SetMotor(2, 0, 0);
}

void SetMotor(int id, int direction, float time) {
  long t = millis();
  
  if (id == 1) {
    if (direction > 0) {
      digitalWrite(4, HIGH);
      digitalWrite(5, LOW);
    }
    else if (direction < 0) {
      digitalWrite(4, LOW);
      digitalWrite(5, HIGH);
    }
    else {
      digitalWrite(4, LOW);
      digitalWrite(5, LOW);
    }
    timeMotor1 = (long)(time * 1000) + t;
  }
  if (id == 2) {
    if (direction > 0) {
      digitalWrite(6, HIGH);
      digitalWrite(7, LOW);
    }
    else if (direction < 0) {
      digitalWrite(6, LOW);
      digitalWrite(7, HIGH);
    }
    else {
      digitalWrite(6, LOW);
      digitalWrite(7, LOW);
    }
    timeMotor2 = (long)(time * 1000) + t;
  }
}

void MotorsUpdate() {
  long t = millis();
  
  if (timeMotor1 == 0) {
    // Skip
  }
  else if (t > timeMotor1) {
    SetMotor(1, 0, 0);
  }
  
  if (timeMotor2 == 0) {
    // Skip
  }
  else if (t > timeMotor2) {
    SetMotor(2, 0, 0);
  }
}
