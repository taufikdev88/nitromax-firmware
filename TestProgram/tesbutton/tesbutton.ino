#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define RELAY_OFF 1
#define RELAY_ON !RELAY_OFF

#define OUT_SELENOID 38
#define OUT_UP 40
#define OUT_DOWN 41
#define OUT_MOTOR 42
#define OUT_PAUSE 43
#define OUT_AUTO 44
#define OUT_INFLATION 45
#define OUT_CAR 46

#define IN_CAR 36
#define IN_DOWN 39
#define IN_INFLATION 34
#define IN_MOTOR 35
#define IN_UP 37
#define IN_AUTO 32
#define IN_PAUSE 33

const uint8_t inputPin[7] = { IN_CAR, IN_DOWN, IN_INFLATION, IN_MOTOR, IN_UP, IN_AUTO, IN_PAUSE };
const uint8_t outputPin[8] = { OUT_CAR, OUT_DOWN, OUT_INFLATION, OUT_MOTOR, OUT_UP, OUT_AUTO, OUT_PAUSE, OUT_SELENOID };

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  for(uint8_t i=0; i<8; i++){
    pinMode(outputPin[i], OUTPUT);
    digitalWrite(outputPin[i], RELAY_OFF);
    
    if(i < 7) pinMode(inputPin[i], INPUT_PULLUP);
  }
  lcd.setCursor(0,0);
  lcd.print("MODE TEST BUTTON");
}

void loop() {
  digitalWrite(OUT_PAUSE, (isBtnPause() ? RELAY_ON : RELAY_OFF));
  digitalWrite(OUT_UP, (isBtnUp() ? RELAY_ON : RELAY_OFF));
  digitalWrite(OUT_DOWN, (isBtnDown() ? RELAY_ON : RELAY_OFF));
  digitalWrite(OUT_CAR, (isBtnCar() ? RELAY_ON : RELAY_OFF));
  digitalWrite(OUT_MOTOR, (isBtnMotor() ? RELAY_ON : RELAY_OFF));
  digitalWrite(OUT_INFLATION, (isBtnInf() ? RELAY_ON : RELAY_OFF));
  digitalWrite(OUT_AUTO, (isBtnAuto() ? RELAY_ON : RELAY_OFF));
}
/*
 * checked 7 feb 2021
 * 
 * Ini adalah file yang mengatur tentang pendeteksian tombol tombol
 * nitrogen dengan konsep sampling 
 */
#define MAX_DETECT 20
#define MIN_DETECT 15

bool pressed = false;
bool isBtnCar(){
  uint8_t cnt = 0;
  for(uint8_t i=0; i<MAX_DETECT; i++){
    if(digitalRead(IN_CAR) == 0) cnt++;
    delayMicroseconds(10);
  }
  if(cnt > MIN_DETECT) return true;
  return false;
}
bool isBtnInf(){
  uint8_t cnt = 0;
  for(uint8_t i=0; i<MAX_DETECT; i++){
    if(digitalRead(IN_INFLATION) == 0) cnt++;
    delayMicroseconds(10);
  }
  if(cnt > MIN_DETECT) return true;
  return false;
}
bool isBtnMotor(){
  uint8_t cnt = 0;
  for(uint8_t i=0; i<MAX_DETECT; i++){
    if(digitalRead(IN_MOTOR) == 0) cnt++;
    delayMicroseconds(10);
  }
  if(cnt > MIN_DETECT) return true;
  return false;
}
bool isBtnAuto(){
  uint8_t cnt = 0;
  for(uint8_t i=0; i<MAX_DETECT; i++){
    if(digitalRead(IN_AUTO) == 0) cnt++;
    delayMicroseconds(10);
  }
  if(cnt > MIN_DETECT) return true;
  return false;
}
bool isBtnPause(){
  uint8_t cnt = 0;
  for(uint8_t i=0; i<MAX_DETECT; i++){
    if(digitalRead(IN_PAUSE) == 0) cnt++;
    delayMicroseconds(10);
  }
  if(cnt > MIN_DETECT) return true;
  return false;
}
bool isBtnUp(){
  uint8_t cnt = 0;
  for(uint8_t i=0; i<MAX_DETECT; i++){
    if(digitalRead(IN_UP) == 0) cnt++;
    delayMicroseconds(10);
  }
  if(cnt > MIN_DETECT) return true;
  return false;
}
bool isBtnDown(){
  uint8_t cnt = 0;
  for(uint8_t i=0; i<MAX_DETECT; i++){
    if(digitalRead(IN_DOWN) == 0) cnt++;
    delayMicroseconds(10);
  }
  if(cnt > MIN_DETECT) return true;
  return false;
}
