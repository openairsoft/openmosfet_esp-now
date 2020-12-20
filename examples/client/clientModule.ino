#if !defined(OM_ESP_NOW_CLIENT) && !defined(OM_ESP_NOW_SERVER)
  #ifndef OM_ESP_NOW_CLIENT
    #define OM_ESP_NOW_CLIENT
  #endif
#endif

#ifdef OM_ESP_NOW_CLIENT

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <openMosfetEspNow.h>

void setup(){
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  OpenMosfetEspNowClient::begin();
  Serial.begin(115200);
  Serial.print("WiFi.macAddress(): ");
  Serial.println(WiFi.macAddress());
}
 
void loop(){
}
#endif