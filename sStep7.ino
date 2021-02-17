/*
 * Step7 merupakan step pemrosesan transaksi, baik pengiriman data ke android
 * ke data logger maupun ke printer, dan disini jika mode transaksinya adalah 
 * tambal ban, proses pengisian jumlah lubang terdapat disini
 */
void vStep7(){
  lcd.clear();
  if(mode[1] == 2){
    ban = 0;
    while(1){  
      if((unsigned long) millis()-tRefresh > 500){
        lcdLine(1, "      NITROMAX      ");
        lcdLine(2, " Jumlah Lubang:     ");
        lcd.setCursor(16,1);
        lcd.print(ban);   
      }

      if(Serial1.available() > 0){
        globalString = Serial1.readStringUntil('\n');
        
        StaticJsonDocument<JSON_PACKET_LENGTH> doc;
        DeserializationError error = deserializeJson(doc, globalString);
        if(!error){
          if(doc["jumlah_lubang"]){
            ban = doc["jumlah_lubang"].as<String>().toInt();
            Serial1.println(F("{\"mode\":\"jumlah_lubang\",\"status\":\"ok\"}"));
            goto end1;
          }
        }
        Serial1.println(F("{\"mode\":\"jumlah_lubang\",\"status\":\"fail\"}"));
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
        ban = ban*10+n;
      } else if(customKey == 'G'){
  end1:
        if(ban < 1) continue;
        cekHarga(true); // harga akan masuk ke variable temp berupa String, isTambalFinish = true
        // harga ke Rp dan ribuan dipisahkan titik
        paket.harga = ribuanCek((String)(globalString.toInt()*ban));
        //paket.detail.jumlah_error = String(err);
        break;
      }
    }
  } else {
    ban = 0;
    if(paket.detail.tekanan.length() == 0){
      paket.harga = "Rp. 0";
      goto skipcekharga;
    }
    // disini untuk transaksi selain tambal ban, dihitung berdasarkan jumlah ban yang berhasil di jalankan
    int8_t idxB = 0;
    int8_t idxA = 0;
    while(idxB < paket.detail.tekanan_awal.length()-1){
      ban++;
      int8_t fB = paket.detail.tekanan_awal.substring(idxB, findComma(paket.detail.tekanan_awal, idxB)).toInt();
      int8_t fA = paket.detail.tekanan.substring(idxA, findComma(paket.detail.tekanan, idxA)).toInt();
      idxB = findComma(paket.detail.tekanan_awal, idxB) + 1;
      idxA = findComma(paket.detail.tekanan, idxA) + 1;
      
      Serial.println((String) "Tekanan Akhir dan Awal jauh ? " + (fA > fB));
      
      cekHarga(true, !(fA > fB));
      paket.harga = String(paket.harga.toInt() + globalString.toInt());
    }
    paket.harga = ribuanCek(paket.harga);
  }

skipcekharga:
  paket.detail.jumlah_error = String(err); 
  paket.detail.jumlah_ban = String(ban); 

  // menghapus file temp tadi
  SD.remove(FILE_TEMP);
  delay(1);
  
  lcdLine(1, "PROSES CETAK NOTA");
  
  sendSerial(CHECK_BT_CLIENT);
  if(readSerial()){
    if(globalString.indexOf(BT_CLIENT_ERROR) >= 0){
      lcd.clear();
      lcdLine(1, "PRINTER/APLIKASI");
      lcdLine(2, "TERPUTUS");
      lcdLine(4, "TRANSAKSI DIBACKUP  ");
      printJSON(true); // backup true, finish true
      delay(2000);
    } else {
      // printJSON dijalankan
      printJSON(); // backup false, finish true 
    }
  }  
  
  step = step1;
  tReload = tRefresh = 0;
}
