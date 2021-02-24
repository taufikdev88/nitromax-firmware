#include "COMMUNICATION.h"

#include "BluetoothSerial.h"
#include "Adafruit_Thermal.h"
#include "Adafruit_Thermal.h"
#include <ArduinoJson.h>
#include <WiFi.h>
#include "time.h"

bool emergency = false;
bool recovery = false;
bool isDateUpdated = false;
BluetoothSerial SerialBT;
Adafruit_Thermal printer(&SerialBT);

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
}

void loop() {
  StaticJsonDocument<JSON_PACKET_LENGTH> doc;

  //******************************** Mikro to ESP -> use keyworad
  //******************************** Mikro to Android -> use JSON in normal mode
  //******************************** Mikro to Printer -> use JSON in emergency mode
  //******************************** Khusus recovery, karena banyak paket json, maka ditandai recovery start
  if(Serial2.available() > 0){    
    if(recovery){
      char d = ' ';
      while(Serial2.available()){
        d = (char) Serial2.read();
        if(d != RECOVERY_STOP){
          SerialBT.write(d);
          Serial.print(d); 
        } else {
          Serial.println("Recovery Stop");
          recovery = false;
          Serial2.flush();
        }
      }
      return;
    }
    
    String dataIn = Serial2.readStringUntil('\n');
    dataIn = dataIn.substring(0, dataIn.indexOf('\r'));
    
    Serial.println(dataIn);

    DeserializationError error = deserializeJson(doc, dataIn);
    if(!error){ // kalau tidak error, berarti dia menggunakan json
      Serial.println("Dapat paket json");
      if(!emergency){
        serializeJson(doc, SerialBT);
        SerialBT.println();
        Serial.println("Print ke android selesai");
      } else {
        paket.no_transaksi = doc["no_transaksi"].as<String>();
        paket.tgl_transaksi = doc["tgl_transaksi"].as<String>();
        paket.jenis_kendaraan = doc["jenis_kendaraan"].as<String>();
        paket.harga = doc["harga"].as<String>();
        paket.detail.mode_transaksi = doc["detail"][0]["mode_transaksi"].as<String>();
        paket.detail.jumlah_ban = doc["detail"][0]["jumlah_ban"].as<String>();
        paket.detail.jumlah_error = doc["detail"][0]["jumlah_error"].as<String>();
        paket.detail.tekanan = doc["detail"][0]["tekanan"].as<String>();
        paket.detail.tekanan_awal = doc["detail"][0]["tekanan_awal"].as<String>();
        printRPP02N();
        Serial.println("Print ke printer selesai");
      }
      dataIn = "";
    } else if(dataIn.indexOf(KEYWORD) >= 0){
      dataIn = dataIn.substring(KEYWORD_LENGTH);
      
      if(dataIn.indexOf(ASK_WIFI_CONNECT) >= 0){
        Serial.println("Konek ke wifi");
        wifiConnect();
      } else
      if(dataIn.indexOf(ASK_SETUP_EMERGENCY) >= 0){
        Serial.println("Setup emergency");
        setupEmergency();
      } else
      if(dataIn.indexOf(ASK_SETUP_NORMAL) >= 0){
        Serial.println("Setup normal");
        setupNormal();
      } else 
      if(dataIn.indexOf(ASK_DATE_TIME) >= 0){
        Serial.println("Update tanggal");
        dateTimeUpdate();
      } else
      if(dataIn.indexOf(CHECK_BT_CLIENT) >= 0){
        Serial.println("Cek Client Bluetooth");
        checkBtClient();
      } else
      if(dataIn.indexOf(ASK_TO_RESTART) >= 0){
        Serial.println("Restart esp");
        ESP.restart();
      } else
      if(dataIn.indexOf(RECOVERY_START) >= 0){
        Serial.println("Recovery Start");
        recovery = true;
      }
    }
  }

  //********************* android to ESP to Mikro only on normal mode
  if(!emergency){
    if(SerialBT.available() > 0){
      String dataBT = SerialBT.readStringUntil('\n');

      DeserializationError error = deserializeJson(doc, dataBT);
      if(!error){
        // jika tidak error, langsung saja terus kan ke arduino mega
        Serial2.println(dataBT);
      }
    }
  }
}

void sendSerial(String packet){
  Serial2.print(KEYWORD);
  Serial2.println(packet);
}
