/*
 * checked 7 feb 2021
 * 
 * Step1 merupakan layar standby dimana disini operator memilih jenis kendaraan
 * dan disini merupakan tempat menerima perintah dari android
 */
void vStep1(){
  digitalWrite(OUT_PAUSE, (isBtnPause() ? RELAY_ON : RELAY_OFF));
      
  if((unsigned long) millis()-tRefresh >= TREFRESH){
    tRefresh = millis();

    lcdLine(2, "Jenis Kendaraan:    ");
    lcdLine(3, "  (84) Motor        ");
    lcdLine(4, "  (85) Mobil        ");
    if(mode[0] == 0) lcd.setCursor(0,2);
    else lcd.setCursor(0,3);
    lcd.write(0);
  }

  if(Serial1.available() > 0){
    globalString = Serial1.readStringUntil('\n');

    StaticJsonDocument<JSON_PACKET_LENGTH> doc;
    DeserializationError error = deserializeJson(doc, globalString);
    if(!error){
      if(doc["mode_transaksi"]){
        mode[0] = !(doc["mode_transaksi"].as<String>().toInt() % 2);
        mode[1] = (doc["mode_transaksi"].as<String>().toInt() - 1) / 2;
        ban = doc["jumlah_ban"].as<String>().toInt();
        referencePressure = String(doc["tekanan"] | "33").toInt();
        Serial1.println(F("{\"mode\":\"input\",\"status\":\"ok\"}"));
        step = step6;
        return;
      } else if(doc["motor"]){
        lcd.clear();
        file = SD.open(FILE_PRICE);
        delay(1);
        if(file){
          file.close();
          SD.remove(FILE_PRICE);
          delay(1);
        }
        file = SD.open(FILE_PRICE, FILE_WRITE);
        if(file){
          file.print(globalString);
          file.close();
          Serial1.println(F("{\"mode\":\"ganti harga\",\"status\":\"ok\"}"));
          lcdLine(1, "HARGA BERUBAH");
          return;
        } else {
          Serial1.println(F("{\"mode\":\"input\",\"status\":\"fail\"}"));
          sdError();
        }
        delay(1000);
      }
    } else {
      Serial1.println(F("{\"mode\":\"input\",\"status\":\"gagal\"}"));
    }
  }
  
  digitalWrite(OUT_CAR, (isBtnCar() ? RELAY_ON : RELAY_OFF));
  digitalWrite(OUT_MOTOR, (isBtnMotor() ? RELAY_ON : RELAY_OFF));
      
  customKey = customKeypad.getKey();
  if(customKey == '!' && !emergency){
    if(!checkClient()) return;
    recovery();
  } else if(customKey == '!' && emergency){
    lcd.clear();
    lcdLine(1, "   Recovery Only    ");
    lcdLine(2, "   On Normal Mode   ");
    delay(1000);
  } else if(customKey == '#'){
    askReboot();
  } else if(customKey == 'U' || customKey == 'D' || customKey == 'L' || customKey == 'R'){
    mode[0] = (mode[0] == 0 ? 1 : 0);
  } else if(customKey == '@'){
    askMode();
  } else if(customKey == 'G'){
    if(!checkClient()) return;
    step = step2;
  } else if(customKey == '1'){
    if(!checkClient()) return;
    mode[0] = 0;
    mode[1] = 0;
    ban = 0;
    step = step3;
  } else if(customKey == '2'){
    if(!checkClient()) return;
    mode[0] = 1;
    mode[1] = 0;
    ban = 0;
    step = step3;
  } else if(customKey == '3'){
    if(!checkClient()) return;
    mode[0] = 0;
    mode[1] = 1;
    ban = 0;
    step = step3;
  } else if(customKey == '4'){
    if(!checkClient()) return;
    mode[0] = 1;
    mode[1] = 1;
    ban = 0;
    step = step3;
  } else if(customKey == '5'){
    if(!checkClient()) return;
    mode[0] = 0;
    mode[1] = 2;
    ban = 0;
    step = step3;
  } else if(customKey == '6'){
    if(!checkClient()) return;
    mode[0] = 1;
    mode[1] = 2;
    ban = 0;
    step = step3;
  } else if(customKey == '*'){
    lcdReload();
  }
}
