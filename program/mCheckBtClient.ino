void checkBtClient(){
  if(SerialBT.hasClient() > 0){
    //Serial.println("ada client terhubung");
    sendSerial(INFO_SUCCESS);
  } else {
    //Serial.println("tidak ada client terhubung");
    sendSerial(BT_CLIENT_ERROR);
  }
}
