/*
 * checked 7 feb 2021
 * 
 * Step2 merupakan tempat dimana operator memilih mode transaksi 
 * isi tambah, isi baru maupun tambal ban
 */
void vStep2(){
  digitalWrite(OUT_PAUSE, (isBtnPause() ? RELAY_ON : RELAY_OFF));
  
  if((unsigned long) millis()-tRefresh > TREFRESH){
    tRefresh = millis();

    lcdLine(2, "Mode Transaksi:     ");
    for(uint8_t i=0; i<2; i++){
      uint8_t j = (mode[1]+i > 3 ? 0 : mode[1] + i);
      lcdLine(3+i, stringTransaksi[j].c_str());
    }
    lcd.setCursor(0,2);
    lcd.write(0);
  }

  customKey = customKeypad.getKey();
  if(customKey == 'U' || customKey == 'R'){
    if(--mode[1] > 3) mode[1] = 3;
  } else if(customKey == 'D' || customKey == 'L'){
    if(++mode[1] > 3) mode[1] = 0;
  } else if(customKey == 'G'){
    if(mode[1] <= 1){
      step = step3;
    } else if(mode[1] == 2){
      step = step4;
    } else {
      step = step5;
    }
  } else if(customKey == 'E' || customKey == '!'){
    step = step1;
  } else if(customKey == '*'){
    lcdReload();
  }
}
