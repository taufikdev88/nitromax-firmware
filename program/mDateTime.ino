/*
 * NTP Configuration
 */
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600*7;
const int   daylightOffset_sec = 0;
struct tm timeinfo;
char date[20];

void dateTimeUpdate(){
  if(!isDateUpdated){
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    if(!getLocalTime(&timeinfo)){
      sendSerial(INFO_GET_TIME_ERROR);
      return;
    }
    isDateUpdated = true;
    strftime(date, 20, "%Y-%m-%d %H:%M ", &timeinfo);
    sendSerial(date);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
  } else {
    if(!getLocalTime(&timeinfo)){
      sendSerial(INFO_GET_TIME_ERROR);
      return;
    }
    strftime(date, 20, "%Y-%m-%d %H:%M ", &timeinfo);
    sendSerial(date);
  }
}
