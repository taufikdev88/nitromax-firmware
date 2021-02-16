/*
 * Step4 merupakan langkah untuk memasukkan tekanan angin akhir
 */
void vStep4(){
  digitalWrite(OUT_PAUSE, (isBtnPause() ? RELAY_ON : RELAY_OFF));
  
  if((unsigned long) millis()-tRefresh > 500){
    tRefresh = millis();

    lcdLine(2, "Tekanan Angin:      ");
    lcdLine(3, "                    ");
    lcd.setCursor(10,2);
    lcd.print(referencePressure);
    lcdLine(4, "                    ");
  }

  customKey = customKeypad.getKey();
  if(customKey == 'U'){
    referencePressure++;
  } else if(customKey == 'D'){
    referencePressure--;
  } else if(customKey == 'L'){
    referencePressure /= 10;
  } else if(customKey == 'R'){
    referencePressure *= 10;
  } else if(customKey == '0' || customKey == '1' || customKey == '2' || customKey == '3' || customKey == '4' || customKey == '5' || customKey == '6' || customKey == '7' || customKey == '8' || customKey == '9'){
    int n = (int) customKey - 48;
    if((referencePressure*10+n) > CAR_MAX_PRESSURE){
      referencePressure = 0;
    }
    referencePressure = referencePressure*10+n;
  } else if(customKey == 'G'){
    if(referencePressure == 0){
      lcdLine(3, "   Pressure Salah   ");
      delay(1000);
    } else {
      step = step5;
    }
  } else if(customKey == 'E'){
    step = step3;
  } else if(customKey == '!'){
    step = step1;
  } else if(customKey == '*'){
    lcdReload();
  }
}
