#include <EEPROM.h>
#include <EEPROM\EEPROM.h>
#include <WiFiClient.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <GDBStub\src\GDBStub.h>

#include <IPAddress.h>


#define OTAMode 26

#include "../MusicLibrary/src/_micro-api/libraries/MusicLibraryLib/src/MusicStripLib.h"


uint8_t value;
long address;
int run = 0;
String ssid = "ESP8266";
String password = "1234567890";
uint8_t port = 80;
enum ControlByte;
WiFiServer server(23);
WiFiClient client;
bool connected = false;
bool OTAUpdateReq = false;

void EEPROM_Clear()
{
	for (int i = 0; i < 128; i++)
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
	uint8_t go = EEPROM.read(EEPROM_Pointer);

	EEPROM_Pointer++;
	char ssidtemp[go];
	for (int i = 0; i < go; i++)
	{
		ssidtemp[i] = EEPROM.read(EEPROM_Pointer);
		EEPROM_Pointer++;
	}


	if (go != 0)
		ssid = String(ssidtemp);
	ssid.remove(go, ssid.length() - go);
	go = EEPROM.read(EEPROM_Pointer);
	memset(ssidtemp, 0, sizeof(ssidtemp));
	*ssidtemp = NULL;
	EEPROM_Pointer++;
	char temp1[go];


	for (int i = 0; i < go; i++)
	{
		temp1[i] = EEPROM.read(EEPROM_Pointer);
		EEPROM_Pointer++;
	}

	if (go != 0)
		password = temp1;

	go = EEPROM.read(EEPROM_Pointer);
	if (go != 0)
		port = go;
	EEPROM_Pointer++;

}

void storeData_WIFI()
{
	EEPROM_Clear();
	int EEPROM_Pointer = 1;
	EEPROM.write(EEPROM_Pointer
		, (uint8_t)ssid.length());
	EEPROM_Pointer++;


	for (uint8_t i = 0; i < ssid.length(); i++)
	{
		EEPROM.write(EEPROM_Pointer, (uint8_t)ssid.charAt(i));
		EEPROM_Pointer++;
	}
	EEPROM.write(EEPROM_Pointer
		, (uint8_t)password.length());

	EEPROM_Pointer++;

	for (uint8_t i = 0; i < password.length(); i++)
	{
		EEPROM.write(EEPROM_Pointer, (uint8_t)password.charAt(i));
		EEPROM_Pointer++;
	}

	EEPROM.write(EEPROM_Pointer
		, port);
	EEPROM.commit();

}

String WifiConnect()
{



	

	WiFi.begin(ssid.c_str(), password.c_str());

	server = WiFiServer(port);
	server.begin();
	int i = 10;
	while (WiFi.status() != WL_CONNECTED) {
		i++;
		if (i == 20)
			return "failed";
		delay(500);
	}


	connected = true;

	digitalWrite(0, HIGH);	digitalWrite(2, HIGH);
	return (WiFi.localIP().toString() + ":" + port);//

													//return true;

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

#ifdef CONFIG

 ssid = "WIFI-WG";
 password = "Weltfrieden67110";
 port = 23;
 value;
 address;
 run = 0;


void setup() {
	EEPROM.begin(128);
	// put your setup code here, to run once:
	Serial.begin(115200);

}

void loop() {
	// put your main code here, to run repeatedly:


	value = EEPROM.read(address);

	Serial.print(address);
	Serial.print("\t");
	Serial.print(value, DEC);
	Serial.println();

	// advance to the next address of the EEPROM
	address = address + 1;

	// there are only 512 bytes of EEPROM, from 0 to 511, so if we're
	// on address 512, wrap around to address 0
	if (address == 64) {
		storeData_WIFI();
		address = 0; delay(100);
	}
	delay(100);
}
#endif

#ifdef NORMAL

void setup() {
	

	ssid = "ESP8266";
	password = "1234567890";
	port = 23;
	pinMode(2, INPUT);
	WiFiServer server(23);

	connected = false;
	OTAUpdateReq = false;
	run = 0;
	Serial.begin(115200);
	EEPROM.begin(128);
	delay(1000);

if (digitalRead(2) == LOW)
{

	WiFi.mode(WIFI_AP);
	WiFi.softAP("ESPCONFIG");

	IPAddress myIP = WiFi.softAPIP();

	Serial.print(myIP);

	Serial.print(":"+(String)port);

	server.begin();

	byte go = 0;
	int counter = 0;
	bool gotdata[3] = { false,false,false };		
	while (!client.connected()) {
		// try to connect to a new client
		client = server.available();// delay(100);
	}
	while (counter < 3) {
	
		while (client.connected()) {
			if (counter == 3)
				break;
			delay(100);
			
			while (client.available()>0) {
				//Serial.println(counter);
				byte req = client.read();
			
				// Read the first line of the request
				if (req == ControlByte::SSID) {
					while (client.available() <= 4)
						delay(50);
					run = client.read();
					if (run == 0)
						break;
					char buffer[run];
					for (int i = 0; i < run; i++)
					{

						buffer[i] = (char)client.read();
					}
					ssid = String(buffer);
					ssid.remove(run, ssid.length() - run);

					if (!gotdata[0])
						counter++;
					gotdata[0] = true;
					client.flush();
					if (counter == 3)
						break;
				}

				else if (req == ControlByte::PWD) {

					while (client.available() <= 4)
						delay(50);
					run = client.read();
					if (run == 0)
						break;
	
					char buffer1[run];
			
					for (int i = 0; i < run; i++)
					{
						buffer1[i] = (char)client.read();
					}
		
					password = String(buffer1);
					password.remove(run, password.length()  - run);
					if (!gotdata[1])
						counter++;
					gotdata[1] = true;
					Serial.println(password);

					client.flush();
					if (counter == 3)
						break;
				}

				else if (req == ControlByte::Port) {
					while (client.available() <= 0)
						delay(50);

					port = client.read();

					if (!gotdata[2])
						counter++;
					gotdata[2] = true;
					client.flush();
					if (counter == 3)
						break;
				}
				

			}
		}
		while (!client.connected()) {

			if (counter == 3)
				break;
			client.stop(); 
		// not connected, so terminate any prior client
			client = server.available();
			delay(100);


		}
		

	}
	storeData_WIFI(); WiFi.softAPdisconnect();
}

loadData();


	if (OTAUpdateReq) {
		WiFi.mode(WIFI_STA);
		WiFi.begin(ssid.c_str(), password.c_str());
		Serial.println(ssid.c_str());
		Serial.println(password.c_str());
		while (WiFi.waitForConnectResult() != WL_CONNECTED) {
			Serial.println("Connection Failed! Rebooting...");
			delay(5000);
			ESP.restart();
		}
		ArduinoOTA.onStart([]() {
			Serial.write(OTAStart);
		});
		ArduinoOTA.onEnd([]() {
			Serial.write(OTAEnd);
			EEPROM.write(0, 0);
		});

		ArduinoOTA.onError([](ota_error_t error) {
			Serial.write(OTAError);
			if (error == OTA_AUTH_ERROR) Serial.write(0);
			else if (error == OTA_BEGIN_ERROR) Serial.write(1);
			else if (error == OTA_CONNECT_ERROR) Serial.write(2);
			else if (error == OTA_RECEIVE_ERROR) Serial.write(3);
			else if (error == OTA_END_ERROR) Serial.write(4);
		});
		ArduinoOTA.begin();
		OTAUpdateReq = true;
	}
	else{
		WiFi.mode(WIFI_STA);
		Serial.println(WifiConnect());

		pinMode(0, OUTPUT); pinMode(2, OUTPUT);
		digitalWrite(0, LOW);	digitalWrite(2, LOW);


	}
	
	
}


void loop() {
	if (!OTAUpdateReq) {
		if (!connected) {

			ESP.reset();
		}
		else
		{

			// Check if a client has connected
			while (client.available()) {
				byte go = 0;
				byte req = client.read();
				// Read the first line of the request
				switch (req)
				{
				case Reconnect:
					WifiDeconnect();
					break;
				case GetLightState:
					Serial.write(req);
					while (Serial.available() <= 0)
						delay(50);
					client.write(Serial.read());
					break;
				case ControlByte::SSID:
					while (client.available() <= 3)
						delay(50);

					ssid = "";

					run = client.read();
					if (run == 0)
						break;
					char buffer[run];
					for (int i = 0; i < run; i++)
					{
						if (Serial.available() <= 0)
							delay(50);
						buffer[i] = (char)Serial.read();
					}
					ssid = String(buffer);

					storeData_WIFI();
					delete[] buffer;
					break;
				case ControlByte::PWD:

					while (client.available() <= 3)
						delay(50);

					password = "";

					run = client.read();
					if (run == 0)
						break;
					char buffer1[run];
					for (int i = 0; i < run; i++)
					{
						if (Serial.available() <= 0)
							delay(50);
						buffer1[i] = (char)Serial.read();
					}
					password = String(buffer1);

					storeData_WIFI();
					delete[] buffer1;
					break;
				case ControlByte::Port:
					while (client.available() <= 3)
						delay(50);

					port = client.read();
					storeData_WIFI();
					break;

					//LED1_CRTL
				case LED1Data:
					Serial.write(req);
					for (byte i = 0; i < 3; i++)
					{
						while (Serial.available() <= 0)
							delay(50);
						client.write(Serial.read());
					}

					break;
				case LED1Frequency:
					Serial.write(req);
					while (Serial.available() <= 0)
						delay(50);
					client.write(Serial.read());
					break;
				case LED1SwitchStade:
					Serial.write(req);
					while (Serial.available() <= 0)
						delay(50);
					client.write(Serial.read());
					break;
					//LED2_CRTL
				case LED2Data:
					Serial.write(req);
					break;
				case LED2Frequency:
					Serial.write(req);
					while (Serial.available() <= 0)
						delay(50);
					client.write(Serial.read());
					break;
				case LED2SwitchStade:
					Serial.write(req);
					while (Serial.available() <= 0)
						delay(50);
					client.write(Serial.read());
					break;

				case OTAUpdate:
					Serial.write(req);

					EEPROM.write(0, OTAMode);
					ESP.restart();
					break;


				}

			}

			while (!client.connected()) {


				client.stop();                                    // not connected, so terminate any prior client
				client = server.available();


			}
		}
	}
	else
	{
		ArduinoOTA.handle();
	}


}
#endif

