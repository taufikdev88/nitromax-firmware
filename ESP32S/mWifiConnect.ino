/*
 * WiFi Configuration
 */
//#define HOTSPOT_SSID "RumahNikita"
//#define HOTSPOT_PASS "1WeKKEadyTKlGZ29EgqO5ndZSwjMpSRdcjJImvAltuCsQJE1pHJUclXmWzXrRTd"
#define HOTSPOT_SSID "DIGIPONIC"
#define HOTSPOT_PASS "@digiponic"
//#define HOTSPOT_SSID "unknown"
//#define HOTSPOT_PASS "baksoenak"

void wifiConnect(){
  if(!isDateUpdated){
    WiFi.begin(HOTSPOT_SSID, HOTSPOT_PASS);
    //Serial.println("connecting to hotspot");
    uint8_t cnt = 0;
    while(WiFi.status() != WL_CONNECTED){
      delay(500);
      //Serial.print(".");
      if(++cnt >= 28){
        sendSerial(INFO_WIFI_ERROR);
        //Serial.println();
        ESP.restart();
      }
    }
  }
  sendSerial(INFO_SUCCESS);
  //Serial.println();
}
