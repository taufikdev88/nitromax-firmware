/*
 * Ini adalah file yang berhubungan dengan sampling dan penghitungan sensor tekanan angin
 * baik konfersi ke psi dan sebaliknya
 */
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
      Serial.print(',');
      Serial.print(analogData);
    }
    Serial.println();
  }
}

int8_t convert2Psi(uint16_t v){
  return map(v, 232, 259, 27, 33);
}
