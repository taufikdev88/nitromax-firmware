/*
 * checked 7 feb 2021
 * 
 * Step5 merupakan langkan konfirmasi dari seluruh pilihan 
 */
void vStep5(){
  digitalWrite(OUT_PAUSE, (isBtnPause() ? RELAY_ON : RELAY_OFF));
  
  if((unsigned long) millis()-tRefresh>500){
    tRefresh = millis();

    // konfirmasi
    lcdLine(1,"Kendaraan : ");
    lcd.print((mode[0] == 0 ? "Motor   ":"Mobil   "));
    lcdLine(2,"Mode      : ");
    switch(mode[1]){
      case 0: lcd.print("Isi Baru"); break;
      case 1: lcd.print("Tambah  "); break;
      case 2: lcd.print("Cek Ban "); break;
      case 3: lcd.print("B.Tambal"); break;
    }
    lcdLine(3,"Jumlah ban:         ");
    lcd.setCursor(12,2);
    lcd.print(ban);
    lcdLine(4,"Tekanan   :         ");
    lcd.setCursor(12,3);
    lcd.print(referencePressure);
  }
  
  customKey = customKeypad.getKey();
  if(customKey == 'G'){
    step = step6;
  } else if(customKey == 'E'){
    step = step1;
  }
}
