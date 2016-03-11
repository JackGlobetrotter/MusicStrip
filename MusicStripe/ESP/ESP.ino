#include <EEPROM.h>
#include <EEPROM\EEPROM.h>
#include <WiFiClient.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <GDBStub\src\GDBStub.h>
#include <MusicStripLib.h>
#include <IPAddress.h>


#define OTAMode 26


int tport;
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
	delay(200);

if (digitalRead(2) == LOW)
{

	WiFi.mode(WIFI_AP);
	WiFi.softAP("ESPCONFIG");

	IPAddress myIP = WiFi.softAPIP();
	delay(200);
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
		int i = 0;
		while (WiFi.status() != WL_CONNECTED) {
			i++;
			if (i == 20)
			{
				Serial.print("Connection Failed! Rebooting...");

				ESP.restart();
			}
			delay(500);
		}


		connected = true;


		Serial.print(WiFi.localIP().toString() + ":OTA");//

		ArduinoOTA.onStart([]() {
			Serial.write(OTAStart);
		});
	//	ArduinoOTA.setPassword((const char *)"1234567890");
		ArduinoOTA.onEnd([]() {
			Serial.write(Start);
			Serial.write(OTAEnd);
			Serial.write(Stop);
			EEPROM.write(0, 0);
			EEPROM.commit();
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
		Serial.print(WifiConnect());


	}
	
	
}

void ReadRest(int Looper) {
	for (size_t i = 0; i < Looper; i++)
	{
		while (!client.available)
			delay(10);
		Serial.write(client.read());
	}

}

void loop() {
	if (!OTAUpdateReq) {
		if (!connected) {

			ESP.reset();
		}
		else
		{
			while(client.connected()){
			// Check if a client has connected
				while (client.available()) {
					byte go = 0;
					byte req = client.read();
					switch (req)
					{
					case Reconnect:
						if (Serial.read() == Stop)
							WifiDeconnect();
						break;
					case GetLightState:
						Serial.write(req);
						if (Serial.read() == Stop)
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
						if (Serial.read() != Stop)
							break;
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
						if (Serial.read() != Stop)
							break;
						password = String(buffer1);

						storeData_WIFI();
						delete[] buffer1;
						break;
					case ControlByte::Port:
						while (client.available() <= 1)
							delay(50);

						tport = client.read();
						if (Serial.read() != Stop)
							break;
						port = tport;
						storeData_WIFI();
						break;

						//LED1_CRTL
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
						ReadRest(2);
						break;
					case OTAUpdate:
						Serial.write(req);

						EEPROM.write(0, OTAMode);
						EEPROM.commit();
						ESP.restart();
						break;
					}
					yield();
				}
				yield();
			}
			
			if(!client.connected())
			Serial.write(ClientLeft);

			while (!client.connected()) {

				delay(2);
				client.stop();                                    // not connected, so terminate any prior client
				client = server.available();

				if(client.connected())
					Serial.write(ClientArrived);
			}

		}
	}
	else
	{
		ArduinoOTA.handle();
	}


}
#endif

