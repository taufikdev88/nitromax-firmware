/*
 * checked 7 feb 2021
 * 
 * Kirip paket json ke ESP
 */
void printJSON(bool isBackup = false, bool isFinish = true){
  tReload = millis();
  
  DynamicJsonDocument doc(JSON_PACKET_LENGTH);
  doc["mode"] = "output";
  doc["status"] = "ok";
  doc["id_mode"] = (isBackup ? "1" : "0");
  doc["no_transaksi"] = paket.no_transaksi;
  doc["tgl_transaksi"] = paket.tgl_transaksi;
  doc["jenis_kendaraan"] = paket.jenis_kendaraan;
  doc["detail"][0]["mode_transaksi"] = paket.detail.mode_transaksi;
  doc["detail"][0]["jumlah_ban"] = paket.detail.jumlah_ban;
  doc["detail"][0]["jumlah_error"] = paket.detail.jumlah_error;
  doc["detail"][0]["tekanan"] = paket.detail.tekanan;
  doc["detail"][0]["tekanan_awal"] = paket.detail.tekanan_awal;
  doc["harga"] = paket.harga;
  doc["jumlah_kalibrasi"] = paket.jumlah_kalibrasi;

  if(isBackup){
    if(isFinish){
      file = SD.open(FILE_BACKUP, FILE_WRITE); // <------------------- file open
      if(!file){
        sdError();
      }
      if(serializeJson(doc, file)){ // <------------------- file write
        file.print('\n'); // <------------------- file write
      }
      delay(1);
      file.close(); // <------------------- file close 
      delay(1);
    } else {
      file = SD.open(FILE_TEMP, FILE_WRITE); // <----------------- file open
      if(!file){
        sdError();
      }
      serializeJson(doc, file); // <----------------- file write
      delay(1);
      file.close(); // <------------------- file close
      delay(1);
    }
  } else {
    if(emergency){
      file = SD.open(FILE_BACKUP, FILE_WRITE); // <------------------- file open
      if(!file){
        sdError();
      }
      if(serializeJson(doc, file)){ // <------------------- file write
        file.print('\n'); // <------------------- file write
      }
      delay(1);
      file.close(); // <------------------- file close 
      delay(1);
    }
    if(emergency && ban == 0) return;
    // syarat agar menghapus jumlah kalibrasi harus mengirim ke android / disimpan ke file
    if(SD.exists(FILE_CALIB)){
      SD.remove(FILE_CALIB);
    }
    serializeJson(doc, Serial1);
    Serial1.println();
  }
}
