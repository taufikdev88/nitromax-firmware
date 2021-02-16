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
