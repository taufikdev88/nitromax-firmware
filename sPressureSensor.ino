/*
 * Ini adalah file yang berhubungan dengan sampling dan penghitungan sensor tekanan angin
 * baik konversi ke psi dan sebaliknya
 */

int8_t newPressure = 0;
int8_t detectedPressure = 0;
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

    if((int8_t) sensorValue[0] > 3) Serial.print(sensorValue[0]);
    bool newValue = true;
    for(uint8_t idx=1; idx<10; idx++){
      if((int8_t) sensorValue[0] > 3) Serial.print((String) ',' + sensorValue[idx]);


      if(idx == 3){
        newPressure = sensorValue[0];
      }
      if(sensorValue[0] != sensorValue[idx]){
        newValue = false;
        break;
      }
    }
  
    if(newValue){
      detectedPressure = sensorValue[0];
      if((int8_t) sensorValue[0] > 3){
        Serial.print(" ==> ");
        Serial.print(detectedPressure);
        Serial.print(',');
        Serial.print(analogData);
      }
    }
    if((int8_t) sensorValue[0] > 3) Serial.println();
  }
}

int8_t convert2Psi(uint16_t v){
//  return map(v, 232, 259, 27, 33); // versi sebelum menu tambal ban terpisah
  return map(v, 102, 256, 0, 33); // versi terakhir kali kalibrasi gampang
//  return map(v, 225, 251, 27, 32); // sebelum yg terakhir
}
