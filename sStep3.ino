/*
 * checked 7 feb 2021
 * 
 * Step3 merupakan langkah dimana operator memasukkan jumlah ban yang ingin diproses
 */
void vStep3(){
  digitalWrite(OUT_PAUSE, (isBtnPause() ? RELAY_ON : RELAY_OFF));
  
  if((unsigned long) millis()-tRefresh > 500){
    tRefresh = millis();

    lcdLine(2, "Jumlah Ban:         ");
    lcdLine(3, "                    ");
    lcd.setCursor(10,2);
    lcd.print(ban);
    lcdLine(4, "                    ");
  }

  customKey = customKeypad.getKey();
  if(customKey == 'U'){
    ban++;
  } else if(customKey == 'D'){
    ban--;
  } else if(customKey == 'L'){
    ban /= 10;
  } else if(customKey == 'R'){
    ban *= 10;
  } else if(customKey == '0' || customKey == '1' || customKey == '2' || customKey == '3' || customKey == '4' || customKey == '5' || customKey == '6' || customKey == '7' || customKey == '8' || customKey == '9'){
    int n = (int) customKey - 48;
    if((ban*10+n) > CAR_MAX_PRESSURE){
      ban = 0;
    }
    ban = ban*10+n;
  } else if(customKey == 'G'){
    if(ban == 0){
      lcdLine(3,"  Jumlah Ban Salah  ");
      delay(1000);
    } else {
      step = step4;
    }
  } else if(customKey == 'E'){
    step = step2;
  } else if(customKey == '!'){
    step = step1;
  } else if(customKey == '*'){
    lcdReload();
  }
}
