#if !defined(OM_ESP_NOW_CLIENT) && !defined(OM_ESP_NOW_SERVER)
  #define OM_ESP_NOW_SERVER
#endif

#ifndef OM_ESP_NOW_SERVER
  #ifdef OM_ESP_NOW_SERVER
#endif

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <openMosfetEspNow.h>

void setup(){
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.begin(115200);
  Serial.print("WiFi.macAddress(): ");
  Serial.println(WiFi.macAddress());
}
 
void loop(){
}
#endif