#ifndef NOWINTERFACE_H
#define NOWINTERFACE_H

#ifndef OM_ESPNOW_SERVER_CHANNEL
	#define OM_ESPNOW_SERVER_CHANNEL 3
#endif

#ifndef OM_ESPNOW_SERVER_ASYNC_TASK_PRIORITY
	#define OM_ESPNOW_SERVER_ASYNC_TASK_PRIORITY 0
#endif

#include <Arduino.h>

enum class EspnowCommand {
	TRIGGER_STATE = 0,
	TRIGGER_BUMP = 1,
	SELECTOR_STATE = 2,
	GEARBOX_UNCOCK = 3
	};

typedef struct struct_command_triggerState {
  	const EspnowCommand command = EspnowCommand::TRIGGER_STATE;
	uint8_t state;
} struct_command_triggerState;

typedef struct struct_command_selectorState {
  	const EspnowCommand command = EspnowCommand::SELECTOR_STATE;
	uint8_t state;
} struct_command_selectorState;

enum class EspnowStatus {
	MAC_ADDRESS = 0,
	BBS_FIRED = 1,
	BATTERY_VOLTAGE = 2,
	SELECTOR_STATE = 3
	};

typedef struct struct_status_MacAdress {
  	const EspnowStatus status = EspnowStatus::MAC_ADDRESS;
	uint8_t macAddress[6];
} struct_status_MacAdress;

typedef struct struct_status_bbsFired {
  	const EspnowStatus status = EspnowStatus::BBS_FIRED;
	unsigned long bbsFired;
} struct_status_bbsFired;

typedef struct struct_status_batteryVoltage {
  	const EspnowStatus status = EspnowStatus::BATTERY_VOLTAGE;
	float batteryVoltage;
} struct_status_batteryVoltage;

typedef struct struct_status_selectorState {
  	const EspnowStatus status = EspnowStatus::SELECTOR_STATE;
	uint8_t selectorState;
} struct_status_selectorState;

#ifdef OM_ESP_NOW_CLIENT
	class OpenMosfetEspNowClient
	{
		private:
			static bool isPaired;
			static void (*bbsFiredCallBack)(unsigned long);
			static void (*batteryVoltageCallBack)(float);
			static void (*selectorStateCallBack)(uint8_t);
			static unsigned long currentBbsFired;
			static float currentBatteryVoltage;
			static uint8_t currentSelectorState;
		public:
			static uint8_t serverMacAddress[6];
			/**
			 * You can set callbacks to NULL if you don't want to use them
			 */
			static void begin(void (*bbsFiredCallBack)(unsigned long), void (*batteryVoltageCallBack)(float), void (*selectorStateCallBack)(uint8_t));
			static void enablePairing();
			static void handleMessage(uint8_t* mac, uint8_t *incomingData, uint8_t len);
			static unsigned long getCurrentBbsFired();
			static float getCurrentBatteryVoltage();
			static uint8_t getCurrentSelectorState();

	};
#endif
#ifdef OM_ESP_NOW_SERVER

#endif
#ifdef OM_ESP_NOW_SERVER_ASYNC
	// #include <WifiEspNow.h>
	#include <esp_now.h>
	#ifndef OM_ESPNOW_SERVER_NBSLAVES_MAX
		#define OM_ESPNOW_SERVER_NBSLAVES_MAX 20
	#endif

	class OpenMosfetEspNowAsyncServer
	{
		private:
			static esp_now_peer_info_t slaves[OM_ESPNOW_SERVER_NBSLAVES_MAX];
			static uint8_t slaveCnt;
			static struct_status_bbsFired tmp_bbsFired_s;
			static struct_status_batteryVoltage tmp_batteryVoltage_s;
			static struct_status_selectorState tmp_selectorState_s;
			static void asyncSendBbsFired(void *);
			static void asyncSendBatteryVoltage(void *);
			static void asyncSendSelectorState(void *);
			static void sendData(uint8_t* data, size_t len);
		public:
			static void begin();
			static void autoAddPeers();
			// static void addPeer(uint8_t peer[6]);
			// static void removePeer(uint8_t peer[6]);
			static void handleMessage(uint8_t* mac, uint8_t *incomingData, uint8_t len);
			static void sendBbsFired(unsigned long bbsFired);
			static void sendBatteryVoltage(float batteryVoltage);
			static void sendSelectorState(uint8_t selectorState);
	};
#endif

#endif