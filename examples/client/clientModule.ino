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

void displayBbsFired(unsigned long bbsFired){
  Serial.printf("Bbs fired: %lu\n", bbsFired);
}

void displayBatteryVoltage(float batteryVoltage){
  Serial.printf("Battery voltage: %f\n", batteryVoltage);
}

void displaySelectorState(uint8_t selectorState){
  Serial.printf("Selector state : %u\n", selectorState);
}

void setup(){
  Serial.begin(115200);
  OpenMosfetEspNowClient::begin(&displayBbsFired, &displayBatteryVoltage, &displaySelectorState);
}
 
void loop(){
}
#endif