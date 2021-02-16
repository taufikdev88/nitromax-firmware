#include "ESP32S/COMMUNICATION.h"
#include "DEFINES.h"

void setup(){
  setupIO(); // sudah dicek
  setupPressure(); // sudah dicek
  setupWiFi(); // sudah dicek
  setupDateTime(); // sudah dicek
  runLastMode(); // sudah dicek
  checkTransaction(); // sudah dicek
  step = step1; // set step ke step1
  err = 0;
}

void loop(){
  if((unsigned long) millis()-tReload >= TRELOAD){
    tReload = millis();

    lcdReload();
    sendSerial(ASK_DATE_TIME);
    if(readSerial()){
      date = globalString.substring(0, 16);
      String line1 = date + (emergency ? " EMG" : " NRM");
      lcdLine(1, line1.c_str());
    }
  }

  digitalWrite(OUT_SELENOID, RELAY_OFF); 
  switch(step){
    case step1: vStep1(); break;
    case step2: vStep2(); break;
    case step3: vStep3(); break;
    case step4: vStep4(); break;
    case step5: vStep5(); break;
    case step6: vStep6(); break;
    case step7: vStep7(); break;
    default: vStep1(); break;
  }
}
