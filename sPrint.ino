/*
 * checked 7 feb 2021
 * 
 * Kirip paket json ke ESP
 */
void printJSON(bool isBackup = false, bool isFinish = true){
  StaticJsonDocument<JSON_PACKET_LENGTH> doc;
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
  doc["jumlah_cekbocor"] = paket.jumlah_cekbocor;

  if(isBackup){
    // jika backup
    if(isFinish){
      // hika backup dan finish tetapi, printer dan android tidak terkoneksi
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
      // ini untuk menyimpan sementara file, agar saat mati sebelum finish, masuk ke file temp dengan id_mode = 1 karena backup
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
    // baik emergency atau normal, akan masuk sini yang penting terkoneksi dgn baik
    if(emergency){
      // jika mode emergency, tambahkan file ke backup
      doc["id_mode"] = "1";
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
    
    serializeJson(doc, Serial1);
    Serial1.println();
  }
}
