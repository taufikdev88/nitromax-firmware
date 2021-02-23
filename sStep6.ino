/*
 * Step 6 merupakan step dalam eksekusi semua perintah yang telah diatur
 * 
 */
void vStep6(){
  // ambil jumlah transaksi sekarang
  String path = (String) date.substring(0,4) + date.substring(5,7) + date.substring(8,10) + ".txt";
  file = SD.open(path);
  if(!file){
    paket.no_transaksi = "1";
    Serial.println("belum ada transaksi hari ini");
    goto skipreading;
  }
  globalString = "";
  delay(1);
  while(file.available()){
    char a = file.read();
    if(isDigit(a)) globalString += a;
  }
  paket.no_transaksi = String(globalString.toInt()+1);
  Serial.print("transaksi hari ini ke-");
  Serial.println(paket.no_transaksi);
skipreading:
  file.close();
  delay(1);
  SD.remove(path);
  delay(1);
  file = SD.open(path, FILE_WRITE);
  if(!file){
    Serial.println("gagal catat nomer transaksi");
    goto skipwriting;
  }
  Serial.println("transaksi tersimpan");
  file.print(paket.no_transaksi);
  delay(1);
skipwriting:
  file.close();
  delay(1);

  // pengisian nilai paket setelah mendapatkan no transaksi
  paket.tgl_transaksi = date;
  paket.jenis_kendaraan = (mode[0] ? jenisKendaraan[1] : jenisKendaraan[0]);
  paket.detail.mode_transaksi = modeTransaksi[((mode[0] == 0 ? 0 : 1) + (mode[1] * 2))];
  cekHarga(false);
  paket.harga = ribuanCek((String)(globalString.toInt()*ban));
  paket.detail.jumlah_ban = String(ban);
  
  paket.detail.tekanan_awal = "";
  paket.detail.tekanan = referencePressure;
  
  // backup ke sdcard dengan parameter finish = false
  printJSON(true, false); // backup ke file  dan finish = false
  
  paket.detail.tekanan = "";

  // ubah jumlah ban untuk mode tambal ban
  if(mode[1] == 2) ban *= 2;

  uint8_t err = 0;

  // setelah pengolahan paket sudah selesai, sesuaikan dengan pressure yang dipilih
  lcd.clear();
  lcdLine(1, "MENYESUAIKAN TEKANAN");

  sesuaikanTekanan();

  digitalWrite((mode[0] ? OUT_CAR : OUT_MOTOR), RELAY_ON);
  delay(TPRESSING);
  digitalWrite((mode[0] ? OUT_CAR : OUT_MOTOR), RELAY_OFF);

  // proses pendeteksian dan pengolahan ada di dalam for ini
  for(ban; ban>0; ban--){
    // ambil dan sampling tekanan sekarang
    tRefresh = millis(); // pinjam variable tRefresh buat menghitung

    lcd.clear();
    lcdLine(1, " Mendeteksi Tekanan ");
    lcdLine(2, "      Awal Ban      ");
    lcdLine(4, "  Segera Tancapkan  ");

    digitalWrite(OUT_SELENOID, RELAY_OFF);
    delay(300);

    detectedPressure = 1; // reset nilai
    Serial.println("Mendeteksi tekanan awal");
    while(true){
      if(Serial1.available()){
        globalString = Serial1.readStringUntil('\n');

        StaticJsonDocument<128> doc;
        DeserializationError error = deserializeJson(doc, globalString);
        if(!error){
          if(doc["tekanan"]){
            referencePressure = doc["tekanan"].as<String>().toInt();
            Serial1.println(F("{\"mode\":\"pressure\",\"status\":\"ok\"}"));
            sesuaikanTekanan();
            continue;
          } else if(doc["open"]){
            Serial1.println(F("{\"mode\":\"pressure\",\"status\":\"ok\"}"));
            break;
          }
          Serial1.println(F("{\"mode\":\"pressure\",\"status\":\"fail\"}"));
        }
      }
      
      if((unsigned long) millis()-tReload >= TUPDATEPRESSURE){
        tReload = millis();
        sesuaikanTekanan();
      }
      
      if((unsigned long) millis()-tRefresh >= TREFRESH){
        tRefresh = millis();
        
        lcdLine(3, " Ref:      Cur:      ");
        lcd.setCursor(5,2);
        lcd.print(referencePressure);
        lcd.setCursor(15,2);
        lcd.print(detectedPressure);
      }
      
      getPressure();
      if(detectedPressure > (NORMAL_PRESSURE + OFFSET_PRESSURE)){
        break;
      }

//      digitalWrite(OUT_UP, (isBtnUp() ? RELAY_ON : RELAY_OFF));
//      digitalWrite(OUT_DOWN, (isBtnDown() ? RELAY_ON : RELAY_OFF));
      if(mode[1] == 0 && isBtnAuto()) break;
      if(mode[1] != 0 && isBtnInf()) break;
      
      customKey = customKeypad.getKey();
      if(customKey == 'G' || (mode[1] == 0 && isBtnAuto()) || (mode[1] != 0 && isBtnInf())){
        break;
      } else if(customKey == 'U'){
        tReload = millis();
        referencePressure++;
      } else if(customKey == 'D'){
        tReload = millis();
        referencePressure--;
      } else if(customKey == 'L'){
        tReload = millis();
        referencePressure /= 10;
      } else if(customKey == 'R'){
        tReload = millis();
        referencePressure *= 10;
      } else if(customKey == '0' || customKey == '1' || customKey == '2' || customKey == '3' || customKey == '4' || customKey == '5' || customKey == '6' || customKey == '7' || customKey == '8' || customKey == '9'){
        tReload = millis();
        int n = (int) customKey - 48;
        if((referencePressure*10+n) > CAR_MAX_PRESSURE){
          referencePressure = 0;
        }
        referencePressure = referencePressure*10+n;
      } else if(customKey == '*'){
        lcdReload();
      }
    }
    Serial.println("tekanan awal didapatkan");
    startPressure = detectedPressure;
    sesuaikanTekanan();
        
    digitalWrite(OUT_SELENOID, RELAY_ON);
    delay(500);
    
    digitalWrite((mode[1] == 0 ? OUT_AUTO : OUT_INFLATION), RELAY_ON);
    delay(300);
    digitalWrite((mode[1] == 0 ? OUT_AUTO : OUT_INFLATION), RELAY_OFF);
    
    int8_t oldDetectedPressure = 0;
    while(1){
      if(Serial1.available()){
        globalString = Serial1.readStringUntil('\n');

        StaticJsonDocument<128> doc;
        DeserializationError error = deserializeJson(doc, globalString);
        if(!error){
          if(doc["end"]){
            Serial1.println(F("{\"mode\":\"end\",\"status\":\"ok\"}"));
            ban = 1;
            step = step7;
            break;
          }
          Serial1.println(F("{\"mode\":\"end\",\"status\":\"fail\"}"));
        }
      }
      
      getPressure();
      if(detectedPressure != oldDetectedPressure){
        // jika detectedPressure adalah pressure yang normal 1-2 psi dan oldpressure merupakan diantara reference  pressure maka dicabut
        if(detectedPressure <= (NORMAL_PRESSURE + OFFSET_PRESSURE) && oldDetectedPressure >= (referencePressure - OFFSET_PRESSURE) && oldDetectedPressure <= (referencePressure + OFFSET_PRESSURE)){
          Serial.println("Terdeteksi dicabut berhasil");
          
          paket.detail.tekanan += String(referencePressure) + ",";
          paket.detail.tekanan_awal += String(startPressure) + ",";
          if(!emergency) Serial1.println(F("{\"mode\":\"cabut\",\"status\":\"ok\"}"));
          break;
        } else 
        if(detectedPressure <= (NORMAL_PRESSURE + OFFSET_PRESSURE) && ( oldDetectedPressure < (referencePressure - OFFSET_PRESSURE) || oldDetectedPressure > (referencePressure + OFFSET_PRESSURE) )){
          Serial.println("Terdeteksi error");
          err++;
        }
        oldDetectedPressure = detectedPressure;
      }
      
      if((unsigned long) millis()-tRefresh > TREFRESH){
        tRefresh = millis();

        lcdLine(2, " KUOTA:    ERR:     ");
        lcd.setCursor(7,1);
        lcd.print(ban);
        lcd.setCursor(15,1);
        lcd.print(err);
        lcdLine(3, "Ref:   Sta:   Cur:  ");
        lcd.setCursor(4,2);
        lcd.print(referencePressure);
        lcd.setCursor(11,2);
        lcd.print(startPressure);
        lcd.setCursor(18,2);
        lcd.print(detectedPressure);
        lcdLine(4, (emergency ? " (Ent) Cetak Struk  " : " (Ent) Kirim Struk  "));
      }
      
      if((unsigned long) millis()-tReload > TRELOAD){
        tReload = millis();
        
        lcdReload();
        sendSerial(ASK_DATE_TIME);
        if(readSerial()){
          if(globalString.indexOf(INFO_GET_TIME_ERROR) == -1){
            date = globalString.substring(0, 16);
            String line1 = date + (emergency ? " EMG" : " NRM");
            lcdLine(1, line1.c_str()); 
          }
        }
      }

//      if(isBtnPause() && !pressed){
//        pressed = true;
//      } else if(!isBtnPause() && pressed){
//        err++;
//        pressed = false;
//      }

      digitalWrite(OUT_PAUSE, (isBtnPause() ? RELAY_ON : RELAY_OFF));
      if(mode[1] == 0) digitalWrite(OUT_AUTO, (isBtnAuto() ? RELAY_ON : RELAY_OFF));
      if(mode[1] != 0) digitalWrite(OUT_INFLATION, (isBtnInf() ? RELAY_ON : RELAY_OFF));
      
      customKey = customKeypad.getKey();
      if(customKey == 'G'){
        ban = 1;
        step = step7;
        break;
      } else if(customKey == '*'){
        lcdReload();
      }

      //******************************* dummy
//      else if(customKey == 'E'){
//        paket.detail.tekanan += String(referencePressure) + ",";
//        paket.detail.tekanan_awal += "7,";
//        if(!emergency) Serial1.println(F("{\"mode\":\"cabut\",\"status\":\"ok\"}"));
//        break;
//      } else if(customKey == 'D'){
//        paket.detail.tekanan += String(referencePressure) + ",";
//        paket.detail.tekanan_awal += String(referencePressure) + ",";
//        if(!emergency) Serial1.println(F("{\"mode\":\"cabut\",\"status\":\"ok\"}"));
//        break;
//      }
      //******************************* dummy
    }
  }
  step = step7;
}
