/*
 * checked 7 feb 2021
 * 
 * Ini adalah file yang berisi tentang fungsi-fungsi yang digunakan di
 * seluruh step mulai setup sampai finish
 * 
 * readSerial butuh sendSerial dulu, data sendSerial hanya untuk komunikasi khusus mega dgn esp, sedangkan mega dgn android lgsg json
 */
bool waitForAnswer = 0;

void sendSerial(char* packet){
  waitForAnswer = 1;
  Serial1.print(KEYWORD);
  Serial1.println(packet);
}

bool readSerial(){
  tRefresh = millis();
  while(waitForAnswer){
    if(Serial1.available() > 0){
      String dataIn = Serial1.readStringUntil('\n');
      Serial.println(dataIn);

      if(dataIn.indexOf(KEYWORD) >= 0){
        globalString = dataIn.substring(KEYWORD_LENGTH);
        return true;
      } else {
        return false;
      }
      waitForAnswer = 0;
    }
    if((unsigned long) millis()-tRefresh >= TRELOAD){
      Serial.println("Ada masalah dengan Serial1");
      lcdReload();
      lcdLine(1, "Komunikasi Timeout! ");
      delay(1000);
      waitForAnswer = 0;
    }
  }
  return false;
}

//*********************************************************
void lcdLine(uint8_t l, char* t){
  lcd.setCursor(0,l-1);
  lcd.print(t);
}

void sdError(){
  lcd.clear();
  lcdLine(1, "SD Card Error!!!");
  lcdLine(2, "Panggil Teknisi");
  while(1);
}

void lcdReload(){
  lcd.begin();
  lcd.backlight();
  lcd.createChar(0, customChar);
  lcd.clear();
}

String ribuanCek(String nominal){
  int8_t len = nominal.length();
  int8_t dlen = 3;  
  while(len > dlen){
    nominal = nominal.substring(0,len-dlen) + '.' + nominal.substring(len-dlen);
    dlen += 4;
    len += 1;
  }
  nominal = "Rp. " + nominal;
  return nominal;
}

void (* restartFunc) (void) = 0;

void askReboot(){
  lcd.clear();
  lcdLine(1, "Nyalakan Ulang ?    ");
  lcdLine(2, "  (Ent) Ya          ");
  lcdLine(3, "  (Esc) Batal       ");
  while(1){
    customKey = customKeypad.getKey();
    if(customKey == 'G') restartFunc();
    if(customKey == 'E') break;
  }
}

void recovery(){
  lcd.clear();
  lcdLine(1,"Recovery ?          ");
  lcdLine(2,"  (Ent) Ya          ");
  lcdLine(3,"  (Esc) Batal       ");

  tReload = millis();
  while(1){
    customKey = customKeypad.getKey();
    if(customKey == 'G'){
      lcd.clear();
      
      file = SD.open(FILE_BACKUP); // <------------------- file open
      if(!file){
        lcdLine(1,"File Recovered!");
        delay(1000);
        break;
      }

      sendSerial(RECOVERY_START);
      while(file.available()){
        char a = (char) file.read();
        Serial1.write(a);
        Serial.write(a);
      }
      file.close(); // <------------------- file close
      Serial1.print(RECOVERY_STOP);
            
      lcdLine(1,"Recovered!");
      delay(1000);
      
      SD.remove(FILE_BACKUP);
      break;
    } else if(customKey == 'E'){
      break;
    }
  }
}

bool checkClient(){
  sendSerial(CHECK_BT_CLIENT);
  if(readSerial()){
    if(globalString.indexOf(BT_CLIENT_ERROR) >= 0){
      lcd.clear();
      lcdLine(1, "BLUETOOTH TERPUTUS  ");
      lcdLine(2, "MOHON CEK KEMBALI   ");
      delay(1000);
      return false;
    } else {
      return true;
    }
  } 
  return false;
}

void askMode(){
  lcd.clear();
  lcdLine(1, "Ganti Mode ?        ");
  lcdLine(2, "  (Ent) Ya          ");
  lcdLine(3, "  (Esc) Batal       ");
  
  while(1){  
    customKey = customKeypad.getKey();
    if(customKey == 'E') break;
    if(customKey == 'G') changeMode();
  }
}

void changeMode(){
  emergency = !emergency;
  SD.remove(FILE_EMERGENCY);
  delay(1);
  file = SD.open(FILE_EMERGENCY, FILE_WRITE);
  if(file){
    file.print((emergency ? "1" : "0"));
    file.close(); 
  } else {
    Serial.println("tidak bisa ganti emergency");
    sdError();
  }

  lcd.clear();
  lcdLine(1, "MODE BERUBAH        ");
  lcdLine(2, "MENYALAKAN ULANG    ");
  sendSerial(ASK_TO_RESTART);
  delay(1000);
  restartFunc();
}

/*
 * harga yang dicek masuk ke globalStrings
 */
void cekHarga(bool isTambalFinish = true, bool gagal = false){
  StaticJsonDocument<JSON_PACKET_LENGTH> doc;
  
  file = SD.open(FILE_PRICE); // <------------------- file open
  deserializeJson(doc, file); // <------------------- file read
  delay(1);
  file.close(); // <------------------- file close
  delay(1);
  
  if(mode[0] == 0){
    switch(mode[1]){
      case 0: globalString = doc["motor"]["kuras"] | "5000"; break;
      case 1: globalString = doc["motor"]["tambah"] | "3000"; break;
      case 2: globalString = (isTambalFinish ? doc["motor"]["tambal"] : doc["motor"]["tambah"]) | "15000"; break;
    }
  } else if(mode[0] == 1){
    switch(mode[1]){
      case 0: globalString = doc["mobil"]["kuras"] | "10000"; break;
      case 1: globalString = doc["mobil"]["tambah"] | "4000"; break;
      case 2: globalString = (isTambalFinish ? doc["mobil"]["tambal"] : doc["mobil"]["tambah"]) | "20000"; break;
    }
  }
  if(gagal){
    if(mode[0] == 0){
      globalString = String(globalString.toDouble() * String(doc["motor"]["gagal"] | "0").toDouble());
    } else if(mode[1] == 1){
      globalString = String(globalString.toDouble() * String(doc["mobil"]["gagal"] | "0").toDouble());
    }
  }
}

void sesuaikanTekanan(){
  if(mode[0] == 0) referencePressure = constrain(referencePressure,MOTOR_MIN_PRESSURE,MOTOR_MAX_PRESSURE);
  else if(mode[0] == 1) referencePressure = constrain(referencePressure,CAR_MIN_PRESSURE,CAR_MAX_PRESSURE);
  
  if(currentPressure < referencePressure){
    for(currentPressure; currentPressure < referencePressure; ++currentPressure){
      digitalWrite(OUT_UP, RELAY_ON);
      delay(TPRESSING);
      digitalWrite(OUT_UP, RELAY_OFF);
      delay(TPRESSING);
    }
  } else if(currentPressure > referencePressure){
    for(currentPressure; currentPressure > referencePressure; --currentPressure){
      digitalWrite(OUT_DOWN, RELAY_ON);
      delay(TPRESSING);
      digitalWrite(OUT_DOWN, RELAY_OFF);
      delay(TPRESSING);
    }
  }
}

int8_t findComma(String s, int8_t i){
  return s.indexOf(',', i);
}

void lcdWifi(){
  lcdReload();
  lcdLine(1, "SIAPKAN WIFI HOTSPOT");
  lcdLine(2, "SSID: DIGIPONIC     ");
  lcdLine(3, "PASS: @digiponic    ");
  lcdLine(4, "  Ent Untuk Lanjut  ");
}
