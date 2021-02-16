#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

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

#define SEN_PRESSURE A3

#define NORMAL_PRESSURE 2
#define OFFSET_PRESSURE 1

const uint8_t inputPin[7] = { IN_CAR, IN_DOWN, IN_INFLATION, IN_MOTOR, IN_UP, IN_AUTO, IN_PAUSE };
const uint8_t outputPin[8] = { OUT_CAR, OUT_DOWN, OUT_INFLATION, OUT_MOTOR, OUT_UP, OUT_AUTO, OUT_PAUSE, OUT_SELENOID };
int8_t detectedPressure = 0;

void setup() {
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  Serial.begin(9600);
  
  for(uint8_t i=0; i<8; i++){
    pinMode(outputPin[i], OUTPUT);
    digitalWrite(outputPin[i], RELAY_OFF);
    
    if(i < 7) pinMode(inputPin[i], INPUT_PULLUP);
  }
  pinMode(SEN_PRESSURE, INPUT);
  
  digitalWrite(OUT_SELENOID, RELAY_OFF);  
}

unsigned long tRefresh = 0;
uint8_t oldDetectedPressure = 0;
uint8_t referencePressure = 30;
#define TPRESSING 150
void loop() {
  if(Serial.available() > 0){
    String data = Serial.readStringUntil('\n');
    if(data.indexOf("ON") >= 0){
      digitalWrite(OUT_SELENOID, RELAY_ON);
    } else if(data.indexOf("OFF") >= 0){
      digitalWrite(OUT_SELENOID, RELAY_OFF);
    } else if(data.indexOf("UP")>= 0){
      digitalWrite(OUT_UP, RELAY_ON);
      delay(TPRESSING);
      digitalWrite(OUT_UP, RELAY_OFF);
    } else if(data.indexOf("DW") >= 0){
      digitalWrite(OUT_DOWN, RELAY_ON);
      delay(TPRESSING);
      digitalWrite(OUT_DOWN, RELAY_OFF);
    } else {
      referencePressure = data.toInt(); 
    }
  }
  
  getPressure();
  if(detectedPressure != oldDetectedPressure){
    if(detectedPressure <= (NORMAL_PRESSURE + OFFSET_PRESSURE) && oldDetectedPressure >= (referencePressure - OFFSET_PRESSURE) && oldDetectedPressure <= (referencePressure + OFFSET_PRESSURE)){
      Serial.println("******************Terdeteksi dicabut berhasil");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(" DICABUT DI TEKANAN ");
      lcd.setCursor(9,1);
      lcd.print(oldDetectedPressure);
      delay(2000);
    }
    oldDetectedPressure = detectedPressure;
  }

  if((unsigned long) millis()-tRefresh >= 500){
    tRefresh = millis();
    lcd.setCursor(0,0);
    lcd.print("Detected: ");
    lcd.print(detectedPressure);
    lcd.print(" Psi         ");
    lcd.setCursor(0,1);
    lcd.print("Reference: ");
    lcd.print(referencePressure);
    lcd.print("       ");
  }

  digitalWrite(OUT_DOWN, (isBtnDown() ? RELAY_ON : RELAY_OFF));
  digitalWrite(OUT_UP, (isBtnUp() ? RELAY_ON : RELAY_OFF));
  digitalWrite(OUT_PAUSE, (isBtnPause() ? RELAY_ON : RELAY_OFF));
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

/*
 * Ini adalah file yang berhubungan dengan sampling dan penghitungan sensor tekanan angin
 * baik konfersi ke psi dan sebaliknya
 */
uint8_t sensorValue[10] = { 0 };
unsigned long tSampling = 0;

void getPressure(void){
  if((unsigned long) millis()-tSampling >= 100){
    tSampling = millis();

    
    unsigned long analogData = 0;
    for(int i=0; i<1000; i++){
      analogData = analogData + analogRead(SEN_PRESSURE);
      delayMicroseconds(10);
    }
    analogData = analogData / 1000;
    
    sensorValue[9] = sensorValue[8];
    sensorValue[8] = sensorValue[7];
    sensorValue[7] = sensorValue[6];
    sensorValue[6] = sensorValue[5];
    sensorValue[5] = sensorValue[4];
    sensorValue[4] = sensorValue[3];
    sensorValue[3] = sensorValue[2];
    sensorValue[2] = sensorValue[1];
    sensorValue[1] = sensorValue[0];
    sensorValue[0] = convert2Psi(analogData);
  
//    Serial.print(analogData);
//    Serial.print(',');
//    Serial.println();
    Serial.print(sensorValue[0]);
    
    bool newValue = true;
    for(uint8_t idx=1; idx<10; idx++){      
      Serial.print((String) ',' + sensorValue[idx]);
      
      if(sensorValue[0] != sensorValue[idx]){
        newValue = false;
        break;
      }
    }
    if(newValue){
      detectedPressure = sensorValue[0];
      Serial.print(" ==> ");
      Serial.print(detectedPressure);
    }
    Serial.println();
  }
}

int8_t convert2Psi(uint16_t v){
  return map(v, 232, 247, 27, 30);
}
