#if defined(OM_ESP_NOW_CLIENT) || defined(OM_ESP_NOW_SERVER) || defined(OM_ESP_NOW_SERVER_ASYNC)
	#include "openMosfetEspNow.h"
#endif 

#ifdef OM_ESP_NOW_CLIENT
	#if defined(ESP8266)
		#include <espnow.h>
  		#include <ESP8266WiFi.h>
	#elif defined(ESP32)
		#include <esp_now.h>
  		#include <WiFi.h>
	#endif
	
	void (*OpenMosfetEspNowClient::bbsFiredCallBack)(unsigned long) = NULL;
	void (*OpenMosfetEspNowClient::batteryVoltageCallBack)(float) = NULL;
	void (*OpenMosfetEspNowClient::selectorStateCallBack)(uint8_t) = NULL;
	bool OpenMosfetEspNowClient::isPaired = false;
	uint8_t OpenMosfetEspNowClient::serverMacAddress[6];

	void OpenMosfetEspNowClient::begin(void (*bbsFiredCallBack)(unsigned long), void (*batteryVoltageCallBack)(float), void (*selectorStateCallBack)(uint8_t)) {

		OpenMosfetEspNowClient::bbsFiredCallBack = bbsFiredCallBack;
		OpenMosfetEspNowClient::batteryVoltageCallBack = batteryVoltageCallBack;
		OpenMosfetEspNowClient::selectorStateCallBack = selectorStateCallBack;


		WiFi.mode(WIFI_AP);

		if(!OpenMosfetEspNowClient::isPaired) {
			OpenMosfetEspNowClient::enablePairing();
		}

		WiFi.disconnect();

		if(esp_now_init() != 0) {
			#ifdef DEBUG
				Serial.println("ESP-NOW initialization failed");
			#endif
			delay(500);
			ESP.restart();
			return;
		}
		#ifdef DEBUG
			else{
				Serial.print("Client listenning on: ");
				Serial.println(WiFi.macAddress());
			}
		#endif

		esp_now_register_recv_cb(OpenMosfetEspNowClient::handleMessage);   // this function will get called once all data is sent
	}

	void OpenMosfetEspNowClient::enablePairing() {
		OpenMosfetEspNowClient::isPaired = false;
		String Prefix = "Slave:";
		String Mac = WiFi.macAddress();
		String SSID = Prefix + Mac;
		String Password = "123456789";
		bool result = WiFi.softAP(SSID.c_str(), Password.c_str(), OM_ESPNOW_SERVER_CHANNEL, 0);

		#ifdef DEBUG
		if (!result) {
			Serial.println("AP Config failed.");
		} else {
			Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
		}
		#endif
	}

	void OpenMosfetEspNowClient::handleMessage(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
		#ifdef DEBUG
			Serial.println("OpenMosfetEspNowClient::handleMessage");
		#endif

		// read the first int to find which structure we received
		const EspnowStatus incomingStatus = (EspnowStatus)*incomingData;
		switch(incomingStatus){
			case EspnowStatus::MAC_ADDRESS :
				{// note the use of a scope
					struct_status_MacAdress macAddress_s;
					memcpy(&macAddress_s, incomingData, sizeof(struct_status_MacAdress));
					#ifdef DEBUG
						Serial.printf("macAddress: %02X:%02X:%02X:%02X:%02X:%02X\n", macAddress_s.macAddress[0], macAddress_s.macAddress[1], macAddress_s.macAddress[2], macAddress_s.macAddress[3], macAddress_s.macAddress[4], macAddress_s.macAddress[5]);
					#endif

					// pair the device if not paired
					if(!OpenMosfetEspNowClient::isPaired){
						memcpy(OpenMosfetEspNowClient::serverMacAddress, macAddress_s.macAddress, sizeof(uint8_t[6]));
						OpenMosfetEspNowClient::isPaired = true;
					}
				}
			break;

			case EspnowStatus::BBS_FIRED :
				{
					OpenMosfetEspNowClient::bbsFiredCallBack( ((struct_status_bbsFired*) incomingData)->bbsFired );
					#ifdef DEBUG
						Serial.printf("bbsFired: %lu\n", ((struct_status_bbsFired*) incomingData)->bbsFired);
					#endif
				}
			break;

			case EspnowStatus::BATTERY_VOLTAGE :
				{
					OpenMosfetEspNowClient::batteryVoltageCallBack( ((struct_status_batteryVoltage*) incomingData)->batteryVoltage );
					#ifdef DEBUG
						Serial.printf("batteryVoltage: %f\n", ((struct_status_batteryVoltage*) incomingData)->batteryVoltage);
					#endif
				}
			break;

			case EspnowStatus::SELECTOR_STATE :
				{	
					OpenMosfetEspNowClient::selectorStateCallBack( ((struct_status_selectorState*) incomingData)->selectorState );
					#ifdef DEBUG
						Serial.printf("selectorState: %u\n", ((struct_status_selectorState*) incomingData)->selectorState);
					#endif
				}
			break;
			#ifdef DEBUG
				default :
						Serial.printf("unknown code: %i\n", incomingStatus);
				break;
			#endif
		}
	}
#endif

#ifdef OM_ESP_NOW_SERVER
#endif

#ifdef OM_ESP_NOW_SERVER_ASYNC
	// #include <WifiEspNow.h>
	#include <WiFi.h>
	#include <esp_now.h>

	esp_now_peer_info_t OpenMosfetEspNowAsyncServer::slaves[OM_ESPNOW_SERVER_NBSLAVES_MAX] = {};
	uint8_t OpenMosfetEspNowAsyncServer::slaveCnt = 0;
	struct_status_bbsFired OpenMosfetEspNowAsyncServer::tmp_bbsFired_s;
	struct_status_batteryVoltage OpenMosfetEspNowAsyncServer::tmp_batteryVoltage_s;
	struct_status_selectorState OpenMosfetEspNowAsyncServer::tmp_selectorState_s;

	/**
	 * Wifi must ba enabled before calling this method
	 */
	void OpenMosfetEspNowAsyncServer::begin()
 	{
		if(esp_now_init() != 0) {
			#ifdef DEBUG
				Serial.println("ESP-NOW initialization failed");
			#endif
		}
	}

	//from https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/ESPNow/Multi-Slave/Master/Master.ino
	void OpenMosfetEspNowAsyncServer::autoAddPeers()
 	{
		int8_t scanResults = WiFi.scanNetworks();
		//reset slaves
		memset(OpenMosfetEspNowAsyncServer::slaves, 0, sizeof(OpenMosfetEspNowAsyncServer::slaves));
		OpenMosfetEspNowAsyncServer::slaveCnt = 0;
		Serial.println("");
		if (scanResults == 0) {
			#ifdef DEBUG
				Serial.println("No WiFi devices in AP Mode found");
			#endif
		} else {
			#ifdef DEBUG
				Serial.print("Found "); Serial.print(scanResults); Serial.println(" devices ");
			#endif
			for (int i = 0; i < scanResults; ++i) {
			// Print SSID and RSSI for each device found
			String SSID = WiFi.SSID(i);
			int32_t RSSI = WiFi.RSSI(i);
			String BSSIDstr = WiFi.BSSIDstr(i);

			#ifdef DEBUG
				Serial.print(i + 1); Serial.print(": "); Serial.print(SSID); Serial.print(" ["); Serial.print(BSSIDstr); Serial.print("]"); Serial.print(" ("); Serial.print(RSSI); Serial.print(")"); Serial.println("");
			#endif
			delay(10);
			// Check if the current device starts with `Slave`
			if (SSID.indexOf("Slave") == 0) {
				// SSID of interest
				#ifdef DEBUG
					Serial.print(i + 1); Serial.print(": "); Serial.print(SSID); Serial.print(" ["); Serial.print(BSSIDstr); Serial.print("]"); Serial.print(" ("); Serial.print(RSSI); Serial.print(")"); Serial.println("");
				#endif
				// Get BSSID => Mac Address of the Slave
				int mac[6];

				if ( 6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x",  &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5] ) ) {
				for (int ii = 0; ii < 6; ++ii ) {
					OpenMosfetEspNowAsyncServer::slaves[OpenMosfetEspNowAsyncServer::slaveCnt].peer_addr[ii] = (uint8_t) mac[ii];
				}
				}
				OpenMosfetEspNowAsyncServer::slaves[OpenMosfetEspNowAsyncServer::slaveCnt].channel = OM_ESPNOW_SERVER_CHANNEL; // pick a channel
				OpenMosfetEspNowAsyncServer::slaves[OpenMosfetEspNowAsyncServer::slaveCnt].encrypt = 0; // no encryption
				OpenMosfetEspNowAsyncServer::slaveCnt++;
			}
			}
		}

		// clean up ram
		WiFi.scanDelete();

		if (OpenMosfetEspNowAsyncServer::slaveCnt > 0) {
			for (int i = 0; i < OpenMosfetEspNowAsyncServer::slaveCnt; i++) {

				for (int ii = 0; ii < 6; ++ii ) {
					Serial.print((uint8_t) OpenMosfetEspNowAsyncServer::slaves[i].peer_addr[ii], HEX);
					if (ii != 5) Serial.print(":");
				}

				// check if the peer exists
				bool exists = esp_now_is_peer_exist(OpenMosfetEspNowAsyncServer::slaves[i].peer_addr);
				if (exists) {
					// Slave already paired.
					#ifdef DEBUG
						Serial.println("Already Paired");
					#endif
				} else {
					// Slave not paired, attempt pair
					esp_err_t addStatus = esp_now_add_peer(&OpenMosfetEspNowAsyncServer::slaves[i]);
					#ifdef DEBUG
						if (addStatus == ESP_OK) {
							Serial.println("Pair success");
						} else {
							Serial.println("Pair failed");
						}
						delay(100);// todo: check id this is needed
					#endif
				}
			}
		} else {
			// No slave found to process
			Serial.println("No Slave found to process");
		}
	}

	void OpenMosfetEspNowAsyncServer::sendData(uint8_t* data, size_t len)
	{
		#ifdef DEBUG
			Serial.println("OpenMosfetEspNowAsyncServer::sendData");
		#endif

		for (int i = 0; i < OpenMosfetEspNowAsyncServer::slaveCnt; i++) {
			const uint8_t *peer_addr = OpenMosfetEspNowAsyncServer::slaves[i].peer_addr;

			esp_err_t result = esp_now_send(peer_addr, data, len);
			
			#ifdef DEBUG
				Serial.print("Send Status: ");
				if (result == ESP_OK) {
					Serial.println("Success");
				} else {
					Serial.println("Failure");
				}
			#endif
		}
	}

	// void OpenMosfetEspNowAsyncServer::addPeer(uint8_t peer[6])
 	// {
	// 	// Register peer
	// 	esp_now_peer_info_t peerInfo;
	// 	memcpy(peerInfo.peer_addr, peer, sizeof(uint8_t[6]));
	// 	peerInfo.channel = 0;  
	// 	peerInfo.encrypt = false;


	// 	esp_err_t addStatus = esp_now_add_peer(&peerInfo);

	// 	if (addStatus == ESP_OK) {
	// 		Serial.println("Pair success");
	// 	} else if {
	// 		Serial.println("Pair failed");
	// 	}
	// }

	// void OpenMosfetEspNowAsyncServer::removePeer(uint8_t peer[6])
 	// {
	// 	// Add peer        
	// 	if (esp_now_del_peer(peer) != ESP_OK){
	// 		#ifdef DEBUG
	// 			Serial.println("Failed to remove peer");
	// 		#endif
	// 	}
	// }

	void OpenMosfetEspNowAsyncServer::sendBbsFired(unsigned long bbsFired)
 	{
		#ifdef DEBUG
			Serial.println("OpenMosfetEspNowAsyncServer::sendBbsFired");
		#endif

		OpenMosfetEspNowAsyncServer::tmp_bbsFired_s.bbsFired = bbsFired;

		xTaskCreatePinnedToCore(
		OpenMosfetEspNowAsyncServer::asyncSendBbsFired, /* Function to implement the task */
		"asyncSendBbsFired", /* Name of the task */
		10000,  /* Stack size in words */
		NULL,  /* Task input parameter */
		OM_ESPNOW_SERVER_ASYNC_TASK_PRIORITY,  /* Priority of the task */
		NULL,  /* Task handle. */
		1); /* Core where the task should run */
	}

	void OpenMosfetEspNowAsyncServer::asyncSendBbsFired(void *)
 	{
		#ifdef DEBUG
			Serial.println("OpenMosfetEspNowAsyncServer::asyncSendBbsFired");
		#endif

		// send to all peers
		OpenMosfetEspNowAsyncServer::sendData((uint8_t *) &OpenMosfetEspNowAsyncServer::tmp_bbsFired_s, sizeof(struct_status_bbsFired));
		// esp_err_t result = esp_now_send(NULL, (uint8_t *) &OpenMosfetEspNowAsyncServer::tmp_bbsFired_s, sizeof(struct_status_bbsFired));

		#ifdef DEBUG
			Serial.println(uxTaskGetStackHighWaterMark(NULL));
		#endif

		vTaskDelete(NULL);
	}

	
	void OpenMosfetEspNowAsyncServer::sendBatteryVoltage(float batteryVoltage)
 	{
		#ifdef DEBUG
			Serial.println("OpenMosfetEspNowAsyncServer::sendBatteryVoltage");
		#endif

		OpenMosfetEspNowAsyncServer::tmp_batteryVoltage_s.batteryVoltage = batteryVoltage;

		xTaskCreatePinnedToCore(
		OpenMosfetEspNowAsyncServer::asyncSendBatteryVoltage, /* Function to implement the task */
		"asyncSendBatteryVoltage", /* Name of the task */
		10000,  /* Stack size in words */
		NULL,  /* Task input parameter */
		OM_ESPNOW_SERVER_ASYNC_TASK_PRIORITY,  /* Priority of the task */
		NULL,  /* Task handle. */
		1); /* Core where the task should run */
	}

	void OpenMosfetEspNowAsyncServer::asyncSendBatteryVoltage(void *)
 	{
		#ifdef DEBUG
			Serial.println("OpenMosfetEspNowAsyncServer::asyncSendBatteryVoltage");
		#endif

		// send to all peers
		OpenMosfetEspNowAsyncServer::sendData((uint8_t *) &OpenMosfetEspNowAsyncServer::tmp_batteryVoltage_s, sizeof(struct_status_batteryVoltage));
		// esp_err_t result = esp_now_send(NULL, (uint8_t *) &OpenMosfetEspNowAsyncServer::tmp_batteryVoltage_s, sizeof(struct_status_batteryVoltage));

		#ifdef DEBUG
			Serial.println(uxTaskGetStackHighWaterMark(NULL));
		#endif

		vTaskDelete(NULL);
	}

	
	void OpenMosfetEspNowAsyncServer::sendSelectorState(uint8_t selectorState)
 	{
		#ifdef DEBUG
			Serial.println("OpenMosfetEspNowAsyncServer::sendSelectorState");
		#endif
		OpenMosfetEspNowAsyncServer::tmp_selectorState_s.selectorState = selectorState;

		xTaskCreatePinnedToCore(
		OpenMosfetEspNowAsyncServer::asyncSendSelectorState, /* Function to implement the task */
		"asyncSendSelectorState", /* Name of the task */
		10000,  /* Stack size in words */
		NULL,  /* Task input parameter */
		OM_ESPNOW_SERVER_ASYNC_TASK_PRIORITY,  /* Priority of the task */
		NULL,  /* Task handle. */
		1); /* Core where the task should run */
	}

	void OpenMosfetEspNowAsyncServer::asyncSendSelectorState(void *)
 	{
		#ifdef DEBUG
			Serial.println("OpenMosfetEspNowAsyncServer::asyncSendSelectorState");
		#endif

		// send to all peers
		OpenMosfetEspNowAsyncServer::sendData((uint8_t *) &OpenMosfetEspNowAsyncServer::tmp_selectorState_s, sizeof(struct_status_selectorState));
		// esp_err_t result = esp_now_send(NULL, (uint8_t *) &OpenMosfetEspNowAsyncServer::tmp_selectorState_s, sizeof(struct_status_selectorState));

		#ifdef DEBUG
			Serial.println(uxTaskGetStackHighWaterMark(NULL));
		#endif

		vTaskDelete(NULL);
	}
#endif