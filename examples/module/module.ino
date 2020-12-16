#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "openMosfetEspNow.h"

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