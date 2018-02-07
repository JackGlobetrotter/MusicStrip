/*
EEPROM Structure
0:	OTA bit
1:	Configured bit
2:	Version
3:	Version
4:	Version
5:	Port


*/




#include "WirelessStripeLib.h"
#include <WiFiManager.h>
#include <EEPROM.h>
#include <WiFiClient.h>

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WiFiUdp.h>

//#include <GDBStub\src\GDBStub.h>
#include "WirelessStripeLib.h"
#include <IPAddress.h>


#define OTAMode 26
#define EEPROM_Size 128
int delaycount = 0;

byte configured;
byte version[3] = {0,0,1};
int tport;
uint8_t value;
long address;
int run = 0;
const char *ssid = "WirelessStripe";
String password = "1234567890";
uint8_t port = 80;
enum ControlByte;
WiFiServer server(23);
WiFiClient client;
bool connected = false;
bool OTAUpdateReq = false;
WiFiManager WifiMan;

void EEPROM_Clear()
{
	for (int i = 0; i < EEPROM_Size; i++)
	{
		EEPROM.write(i, 0);
	}
	EEPROM.commit();
}

void loadData()
{
	
	int	EEPROM_Pointer = 0;
	int mode = EEPROM.read(EEPROM_Pointer);
	if (mode == OTAMode)
		OTAUpdateReq = true;


	EEPROM_Pointer++;
	configured = EEPROM.read(EEPROM_Pointer++);

	//port= EEPROM.read(EEPROM_Pointer++);

}


void WifiConnect()
{

	WifiMan.autoConnect();
	

	/*int i = 10;
	while (WiFi.status() != WL_CONNECTED) {
		i++;
		if (i == 20)
			FallBack();
		delay(500);
	}*/
	server = WiFiServer(port);
	server.begin();

	connected = true;



}

bool WifiDeconnect()
{
	Serial.write(ControlByte::Reconnect);
	connected = false;
	WiFi.disconnect();
	int i = 0;
	while (WiFi.status() != WL_DISCONNECTED) {
		delay(500);
		if (i == 10)
			return false;
		i++;
	}
	return true;


}


//#define CONFIG
#define NORMAL



#ifdef NORMAL






void setup() {
	
	Serial.begin(115200);
	ssid = "ESP8266";
	password = "1234567890";
	port = 23;
	pinMode(2, INPUT);
	WiFiServer server(23);

	connected = false;
	OTAUpdateReq = false;
	run = 0;
//	Serial.begin(115200);
	EEPROM.begin(EEPROM_Size);
	delay(200);
	
	WifiMan;

loadData();
Serial.println(configured);
if (configured == 0) 
{
	WifiMan.startConfigPortal(); 
	if (WiFi.status() == WL_CONNECTED) {
		configured = 1; EEPROM.write(1, 1); 
		EEPROM.commit();
		Serial.println("got cfg");
	} 
}


if (OTAUpdateReq) {
	WifiMan.autoConnect();
	int i = 0;
	/*while (WiFi.status() != WL_CONNECTED) {
		i++;
		if (i == 20)
		{
			EEPROM.write(0, 0);
			EEPROM.commit();
			FallBack();
		}
		delay(500);
	}*/


	connected = true;


	Serial.print(WiFi.localIP().toString() + ":OTA");//

	ArduinoOTA
		.onStart([]() {
		String type;
		if (ArduinoOTA.getCommand() == U_FLASH)
			type = "sketch";
		else // U_SPIFFS
			type = "filesystem";

		// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
		//Serial.println("Start updating " + type);
	})
		.onEnd([]() {
		EEPROM.write(0, 0);
		EEPROM.commit();
	});

	ArduinoOTA.begin();

	//	ArduinoOTA.setPassword((const char *)"1234567890");


	OTAUpdateReq = true;
}
	else{
		WifiConnect();


	}
	
	
}

void ReadRest(int Looper) {
	for (size_t i = 0; i < Looper; i++)
	{
		while (!client.available())
			delay(10);
		Serial.write(client.read());
	}

}

void loop() {
	
	delaycount = 0;
	if (!OTAUpdateReq) {
		if (!connected) {

			delay(500);
			delaycount++;
			if (delaycount > 10)
				ESP.restart();
		}
	
	else
	{
		Serial.println("loop, normal");
		while (client.connected()) {
			// Check if a client has connected
			//Serial.println("got client");

			while (client.available()) {
				byte go = 0;
				byte req = client.read();
				switch (req)
				{


					//	//LED1_CRTL
				case LED1Data:
					Serial.write(req);
					ReadRest(4);
					break;
				case LED1Frequency:
					Serial.write(req);
					ReadRest(2);
					break;
				case LED1SwitchStade:
					Serial.write(req);
					ReadRest(2);

					break;
					//LED2_CRTL
				case LED2Data:
					Serial.write(req);
					ReadRest(4);
					break;
				case LED2Frequency:
					Serial.write(req);
					ReadRest(2);
					break;
				case LED2SwitchStade:
					Serial.write(req);
					ReadRest(2);

					break;
				case LEDState:
					Serial.write(req);
					ReadRest(3);
					break;
				case OTAUpdate:
					Serial.write(req);

					EEPROM.write(0, OTAMode);
					EEPROM.commit();
					ESP.restart();
					break;
				case SaveStartupCFG:
					Serial.write(req);
					ReadRest(1);
				default:
					while (client.available())
						Serial.print((byte)100);
					break;
				}
				yield();
			}
			yield();
		}



		while (!client.connected()) {
			//Serial.println("loop, noclient");
			delay(100);
			yield();

			client.stop();                                    // not connected, so terminate any prior client
			client = server.available();

			if (client.connected()) {
				Serial.println("loop, got new client");
				client.flush();
				delay(500); yield();
				client.write(version[0]);
				client.write(version[1]);
				client.write(version[2]);


				delaycount = 0;
				if (!client.available()) {
					delaycount++; delay(100);
					yield();
					if (delaycount > 10)
					{
						client.stop();
						return;
					}
				}
				if (client.available() && client.read() != 1)
				{
					client.stop();
				}
				else 
				{
					Serial.println("got set byte");
				}

			}
		}

	}
}
	else
	{
		ArduinoOTA.handle();
	}


}
#endif

