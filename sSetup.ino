/*
 * checked 7 feb 2021
 * 
 * menyesuaikan pin IO arduino sbg input output dan inisialisasi 
 * awal seluruh komponen hardware
 */
void setupIO(){
  /*
   * Setup outputpin dan inputpin
   */
  for(uint8_t i=0; i<8; i++){
    pinMode(outputPin[i], OUTPUT);
    digitalWrite(outputPin[i], RELAY_OFF);
    
    if(i < 7) pinMode(inputPin[i], INPUT_PULLUP);
  }

  /*
   * Setup lcd dan karakter tambahan
   */
  lcdReload();

  Serial.begin(9600);
  Serial1.begin(9600);

  if(!SD.begin(53)){
    sdError();
  }
  delay(1);

  Serial.println("**************************************** PROGRAM DIMULAI ***************************************************");
  Serial.println("List SD Card: ");
  Serial.println();
  
  file = SD.open("/");
  printDirectory(file, 0);
  file.close();
}

void printDirectory(File dir, int numTabs) {
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
//      Serial.println("isinya");
//      while(entry.available()){
//        Serial.write((char) entry.read());
//      }
//      Serial.println();
    }
    entry.close();
  }
}

/*
 * Perintah untuk operator untuk mencocokkan tekanan awal dari mesin
 */
void setupPressure(){
  file = SD.open(FILE_CALIB);
  if(!file){
    paket.jumlah_kalibrasi = "1";
    Serial.println("belum kalibrasi semenjak sukses");
    goto skipreadingkalibrasi;
  }
  globalString = "";
  delay(1);
  while(file.available()){
    char a = file.read();
    if(isDigit(a)) globalString += a;
  }
  paket.jumlah_kalibrasi = String(globalString.toInt()+1);
  Serial.print("kalibrasi ke-");
  Serial.println(paket.jumlah_kalibrasi);
skipreadingkalibrasi:
  file.close();
  delay(1);
  SD.remove(FILE_CALIB);
  delay(1);
  file = SD.open(FILE_CALIB, FILE_WRITE);
  if(!file){
    Serial.println("gagal kalibrasi");
    goto skipwritingkalibrasi;
  }
  Serial.println("jumlah kalibrasi tersimpan");
  file.print(paket.jumlah_kalibrasi);
  delay(1);
skipwritingkalibrasi:
  file.close();
  delay(1);
  
  currentPressure = 33;
  lcdLine(1, "   Mohon Samakan    ");
  lcdLine(2, " Tekanan Awal Mesin ");
  lcdLine(3, "          Psi       ");
  lcd.setCursor(7,2);
  lcd.print(currentPressure);
  lcd.setCursor(0,0);
  lcd.print(paket.jumlah_kalibrasi);
  lcdLine(4, "  Ent Untuk Lanjut  ");
  
  while(1){
    if((unsigned long) millis()-tReload >= TRELOAD){
      tReload = millis();
  
      lcdReload();
      lcdLine(1, "   Mohon Samakan    ");
      lcdLine(2, " Tekanan Awal Mesin ");
      lcdLine(3, "          Psi       ");
      lcd.setCursor(7,2);
      lcd.print(currentPressure);
      lcd.setCursor(0,0);
      lcd.print(paket.jumlah_kalibrasi);
    }
  
    digitalWrite(OUT_PAUSE, (isBtnPause() ? RELAY_ON : RELAY_OFF));
    digitalWrite(OUT_UP, (isBtnUp() ? RELAY_ON : RELAY_OFF));
    digitalWrite(OUT_DOWN, (isBtnDown() ? RELAY_ON : RELAY_OFF));

    getPressure();
    if(detectedPressure > (NORMAL_PRESSURE + OFFSET_PRESSURE)){
      break;
    }
    
    customKey = customKeypad.getKey();
    if(customKey == 'G'){
      break;
    } else if(customKey == '*'){
      lcdReload();
      lcdLine(1, "   Mohon Samakan    ");
      lcdLine(2, " Tekanan Awal Mesin ");
      lcdLine(3, "          Psi       ");
      lcd.setCursor(7,2);
      lcd.print(currentPressure);
      lcd.setCursor(0,0);
      lcd.print(paket.jumlah_kalibrasi);
    }
  }

  digitalWrite(OUT_MOTOR, RELAY_ON);
  delay(TPRESSING);
  digitalWrite(OUT_MOTOR, RELAY_OFF);
  delay(TPRESSING);
  digitalWrite(OUT_SELENOID, RELAY_ON);
  delay(500);
  digitalWrite(OUT_INFLATION, RELAY_ON);
  delay(TPRESSING);
  digitalWrite(OUT_INFLATION, RELAY_OFF);
  
  bool calibrated = false;
  int8_t oldDetectedPressure = 0;
  while(1){
    digitalWrite(OUT_PAUSE, (isBtnPause() ? RELAY_ON : RELAY_OFF));
    
    if((unsigned long) millis()-tReload >= TRELOAD){
      tReload = millis();
      lcdReload();
    }
    if((unsigned long) millis()-tRefresh >= TREFRESH){
      tRefresh = millis();
      lcdLine(1, "    Mendeteksi      ");
      lcdLine(2, " Tekanan Awal Mesin ");
      lcdLine(3, "   Ref:    Cur:     ");
      lcd.setCursor(7,2);
      lcd.print(currentPressure);
      lcd.setCursor(15,2);
      lcd.print(detectedPressure);
    }
    
    customKey = customKeypad.getKey();
    if(customKey == '*'){
      lcdReload();
    }
    
    getPressure();
    if(detectedPressure != oldDetectedPressure){
      if(detectedPressure <= (NORMAL_PRESSURE+OFFSET_PRESSURE) && oldDetectedPressure == currentPressure){
        calibrated = true;
        break;
      } else if(detectedPressure <= (NORMAL_PRESSURE+OFFSET_PRESSURE) && oldDetectedPressure != currentPressure){
        calibrated = false;
        break;
      }
      oldDetectedPressure = detectedPressure;
    }
  }

  digitalWrite(OUT_SELENOID, RELAY_OFF);
  if(!calibrated){    
    lcd.clear();
    lcdLine(1, "TEKANAN MESIN TIDAK ");
    lcdLine(2, "       SESUAI       ");
    lcdLine(4, "  MENYALAKAN ULANG  ");
    restartFunc();
  }
}

/*
 * Perintah untuk operator menyiapkan hotspot dengan nama yang telah ditentukan
 * dan memerintah esp untuk melakukan sambungan ke hotspot tsb
 */
void setupWiFi(){
  lcdWifi();
  while(1){
    if((unsigned long) millis()-tReload >= TRELOAD){
      tReload = millis();
      lcdWifi();
    }
    
    customKey = customKeypad.getKey();
    if(customKey == 'G'){
      break;
    } else if(customKey == 'E'){
      restartFunc();
    } else if(customKey == '*'){
      lcdWifi();
    }
  }

  lcd.clear();
  lcdLine(1, "MENCOBA UNTUK       ");
  lcdLine(2, "MENGHUBUNGKAN WIFI  ");
  
trySetupWifi:
  sendSerial(ASK_WIFI_CONNECT);
  if(readSerial(60000)){
    if(globalString.indexOf(INFO_WIFI_ERROR) >= 0){
      lcd.clear();
      lcdLine(1, "Gagal Terhubung.....");
      delay(1000);
      setupWiFi();
    }
  } else {
    goto trySetupWifi;
  }
}

/*
 * Perintah untuk esp mengupdate internal tanggalnya sesuai tanggal ntp server
 */
void setupDateTime(){
  lcd.clear();
  lcdLine(1, "UPDATE TANGGAL");
  lcdLine(2, "PASTIKAN INTERNET");
  lcdLine(3, "TERSEDIA");

trySetupDateTime:
  sendSerial(ASK_DATE_TIME);
  if(readSerial()){
    if(globalString.indexOf(INFO_GET_TIME_ERROR) >= 0){
      lcd.clear();
      lcdLine(1, "Gagal update........");
      delay(1000);
      Serial.println(err);
      if(++err >= 3) {
        askReboot();
      }
      setupDateTime();
    } else if(globalString.length() > 10) {
      date = globalString.substring(0, 16);
      String line1 = date + "   ";
      lcdLine(1, line1.c_str());
      Serial.println("berhasil mendapatkan tanggal");
    }
  } else {
    goto trySetupDateTime;
  }
}

/*
 * untuk cek mode yang terakhir digunakan
 */
void runLastMode(){
  if(SD.exists(FILE_EMERGENCY)){
    Serial.println("File emergency ditemukan");

    file = SD.open(FILE_EMERGENCY);
    if(file){
      Serial.println("Bisa basa emergency");
      if((char) file.read() == '1'){
        emergency = true;
      } else {
        emergency = false;
      }
      file.close();
    } else {
      Serial.println("Gagal baca emergency");
      emergency = false;
    }
  } else {
    Serial.println("File emergency tidak ditemukan");
    
    emergency = false;
    file = SD.open(FILE_EMERGENCY, FILE_WRITE);
    if(file){
      Serial.println("Berhasil nulis emergency");
      file.print("0");
      file.close();
    } else {
      Serial.println("Gabisa nulis emergency");
    }
  }

  lcdLine(2, "MEMULAI BLUETOOTH   ");
  lcdLine(3, "                    ");
  lcdLine(4, "                    ");
  
  if(emergency) sendSerial(ASK_SETUP_EMERGENCY);
  else sendSerial(ASK_SETUP_NORMAL);
  readSerial();
}

/*
 * mengecek transaksi yang belum sempat tersimpan di android, namun mesin dimatikan
 */
void checkTransaction(){
  file = SD.open(FILE_TEMP);
  if(file){
    globalString = "";
    while(file.available()){
      globalString += (char) file.read();
    }
    file.close();
    delay(1);
    file = SD.open(FILE_BACKUP, FILE_WRITE);
    if(file.print(globalString)){
      file.print('\n');
    }
    file.close();
    delay(1);
    SD.remove(FILE_TEMP);
    delay(1);
    Serial.println("Ditemukan transaksi yang belum selesai dan berhasil dipindahkan ke backup");
  }
  file.close();
  delay(1);
  Serial.println("Cek transaksi selesai");
}
