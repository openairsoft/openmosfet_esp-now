#include "openMosfetEspNow.h"
#include <espnow.h>

void OpenMosfetEspNowClient::begin() {
	if(esp_now_init() != 0) {
		Serial.println("ESP-NOW initialization failed");
		return;
  	}

  	esp_now_register_recv_cb(OpenMosfetEspNowClient::handleMessage);   // this function will get called once all data is sent

	uint8_t peer[] {0x30, 0xAE, 0xA4, 0xF8, 0x0B, 0x1C};
}

void OpenMosfetEspNowClient::handleMessage(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
	#ifdef DEBUG
		Serial.println("OpenMosfetEspNowClient::handleMessage");
	#endif
	struct_status_MacAdress macAddress_s;
	struct_status_BbsFired bbsFired_s;
	struct_status_BatteryVoltage batteryVoltage_s;
	struct_status_SelectorState selectorState_s;

	// read the first int to find which structure we received
	const EspnowStatus incomingStatus = (EspnowStatus)*incomingData;
	switch(incomingStatus){
		case EspnowStatus::MAC_ADDRESS :
			memcpy(&macAddress_s, incomingData, sizeof(struct_status_MacAdress));
			#ifdef DEBUG
				Serial.printf("macAddress: %02X:%02X:%02X:%02X:%02X:%02X\n", macAddress_s.macAddress[0], macAddress_s.macAddress[1], macAddress_s.macAddress[2], macAddress_s.macAddress[3], macAddress_s.macAddress[4], macAddress_s.macAddress[5]);
			#endif
		break;

		case EspnowStatus::BBS_FIRED :
			memcpy(&bbsFired_s, incomingData, sizeof(struct_status_BbsFired));
			#ifdef DEBUG
				Serial.printf("bbsFired: %li\n", bbsFired_s.bbsFired);
			#endif
		break;

		case EspnowStatus::BATTERY_VOLTAGE :
			memcpy(&batteryVoltage_s, incomingData, sizeof(struct_status_BatteryVoltage));
			#ifdef DEBUG
				Serial.printf("batteryVoltage: %f\n", batteryVoltage_s.batteryVoltage);
			#endif
		break;

		case EspnowStatus::SELECTOR_STATE :
			memcpy(&selectorState_s, incomingData, sizeof(struct_status_SelectorState));
			#ifdef DEBUG
				Serial.printf("selectorState: %i\n", selectorState_s.selectorState);
			#endif
		break;
	}
}
