#if !defined(OM_ESP_NOW_CLIENT) && !defined(OM_ESP_NOW_SERVER) && !defined(OM_ESP_NOW_SERVER_ASYNC)
  #ifndef OM_ESP_NOW_CLIENT
    #define OM_ESP_NOW_CLIENT
  #endif
#endif

#ifdef OM_ESP_NOW_CLIENT

#include <Arduino.h>

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ESP32)
  #include <WiFi.h>
#endif

#include <openMosfetEspNow.h>

void setup(){
  OpenMosfetEspNowClient::begin();
  Serial.begin(115200);
}
 
void loop(){
}
#endif