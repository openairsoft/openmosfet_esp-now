#if !defined(OM_ESP_NOW_CLIENT) && !defined(OM_ESP_NOW_SERVER)
  #ifndef OM_ESP_NOW_SERVER
    #define OM_ESP_NOW_SERVER
  #endif
#endif

#ifdef OM_ESP_NOW_SERVER
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
}
 
void loop(){
}
#endif