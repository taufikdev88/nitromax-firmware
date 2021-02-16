/*
 * Bluetooth Configuration
 */
uint8_t address[6]  = {0x66, 0x22, 0x21, 0x67, 0x34, 0xC8};
String name = "RPP02N";
bool connected;
#define BTNAME "NitroMax"

void setupEmergency(){
  emergency = true;
  SerialBT.begin(BTNAME, true);
  connected = SerialBT.connect(name);
  //connected = SerialBT.connect(address);
  
  if(!connected){
    Serial.println("printer not connected");
    while(!SerialBT.connected(10000)){
      Serial.println(".");
    }
  }

  SerialBT.disconnect();
  SerialBT.connect();
  if(connected){    
      printer.begin();        // Init printer (same regardless of serial type)
      printer.setSize('S');
      printer.println(F("Printer Conected"));
      printer.feed(2);
      printer.sleep();      // Tell printer to sleep
      delay(3000L);         // Sleep for 3 seconds
      printer.wake();       // MUST wake() before printing again, even if reset
      printer.setDefault(); // Restore printer to defaults
  }
  sendSerial(INFO_PRINTER_READY);
}

void setupNormal(){
  emergency = false;
  SerialBT.begin(BTNAME);
  sendSerial(INFO_SUCCESS);
}
