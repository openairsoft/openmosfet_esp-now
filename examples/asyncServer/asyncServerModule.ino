#if !defined(OM_ESP_NOW_CLIENT) && !defined(OM_ESP_NOW_SERVER) && !defined(OM_ESP_NOW_SERVER_ASYNC)
  #ifndef OM_ESP_NOW_SERVER_ASYNC
    #define OM_ESP_NOW_SERVER_ASYNC
  #endif
#endif

#ifdef OM_ESP_NOW_SERVER_ASYNC
#include <Arduino.h>

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ESP32)
  #include <WiFi.h>
#endif

#include <openMosfetEspNow.h>

void setup(){
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.begin(115200);
  Serial.print("WiFi.macAddress(): ");
  Serial.println(WiFi.macAddress());

  
  OpenMosfetEspNowAsyncServer::begin();
  OpenMosfetEspNowAsyncServer::autoAddPeers();
}

unsigned long beforeSend;
unsigned long afterSend;
void loop(){
  // Send message via ESP-NOW
  struct_status_MacAdress macAddress_s;
  WiFi.macAddress(macAddress_s.macAddress);


  Serial.println("----------multi-core-----------");
  Serial.print("running on core: ");
  Serial.println(xPortGetCoreID());
  beforeSend = micros();
  delay(100);
  OpenMosfetEspNowAsyncServer::sendBbsFired(4);
  delay(100);
  OpenMosfetEspNowAsyncServer::sendBatteryVoltage(12.2);
  delay(100);
  OpenMosfetEspNowAsyncServer::sendSelectorState(1);
  afterSend = micros();
  Serial.printf("delta : %lu\n", afterSend - beforeSend);
  Serial.println("---------------------");


  delay(1000);
}
#endif