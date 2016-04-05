#include <EEPROM.h>
#include <EEPROM\EEPROM.h>
#include <WiFiClient.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <GDBStub\src\GDBStub.h>
#include "MusicStripLib.h"
#include <IPAddress.h>


// First LCD - Address 0x27, 16 chars, 2 line display



#define WIFIPointer 0

#define SIZE    255
#define DELAY    20
#define HUE_MAX  6.0
#define HUE_DELTA 0.01




//----------LED1
#define R1 0
#define G1 3
#define B1 2

bool LED1_active = false;

byte color0[] = { 0,0,254 };

uint16_t color0Hue = 0;
// ----- RuntimeVariabes
char WifiTemp[50];
uint8_t temp[3];
uint8_t run = 0;
uint8_t ctrl = 0;
//-------Fade-------------


int LED1_freq = 500;
int LED1_counter = 0; //+-5

int LED1_mscounter = 0; //ms to freq
bool LED1_FlashOn;

uint8_t LED1_mode = FixColorState;


int go;

bool WifiIsConnected = false;
String IPString = "";
bool LightStat = false;

uint8_t buffer[4];

#define OTAMode 26
#define EEPROMSize 128

int tport;
uint8_t value;
long address;

String ssid = "ESP8266";
String password = "1234567890";
uint8_t port = 80;
enum ControlByte;
WiFiServer server(23);
WiFiClient client;
bool connected = false;
bool OTAUpdateReq = false;
int EEPROM_Pointer;

void EEPROM_Clear()
{
	for (int i = 0; i < EEPROMSize; i++)
	{
		EEPROM.write(i, 0);
	}
	EEPROM.commit();
}



void storeData(bool WIFI)
{  //0 = OTA |LED_DATA  | active|mode|freq|color[3] |  WIFI_DATA | port|ssid|password
	if (!WIFI) {
		EEPROM_Pointer = 1;

		//LED_DATA  | active|mode|freq|color[3]

		EEPROM.write(EEPROM_Pointer, LED1_active);
		EEPROM_Pointer++;

		EEPROM.write(EEPROM_Pointer, LED1_mode);
		EEPROM_Pointer++;

		EEPROM.write(EEPROM_Pointer, LED1_freq);

		EEPROM_Pointer++;

		for (byte i = 0; i < 3; i++)
		{

			EEPROM.write(EEPROM_Pointer, color0[i]);

			EEPROM_Pointer++;

		}
	

	}
	else
	{
		yield();
		EEPROM_Pointer = 7;
		//WIFI_DATA | port|ssid|password
	//	lcd.print((String)EEPROM_Pointer + ":" + (String)port);

		EEPROM.write(EEPROM_Pointer
			, port);

		EEPROM_Pointer++;
		//lcd.print((String)EEPROM_Pointer + ": l: " + (String)ssid.length()+" ssid: "+ssid);

		EEPROM.write(EEPROM_Pointer
			, (uint8_t)ssid.length());
		EEPROM_Pointer++;


		for (uint8_t i = 0; i < ssid.length(); i++)
		{
			EEPROM.write(EEPROM_Pointer, (uint8_t)ssid.charAt(i));
			EEPROM_Pointer++;
		}

		//lcd.print((String)EEPROM_Pointer + ": l: " + (String)password.length() + " password: " + password);
		
		EEPROM.write(EEPROM_Pointer
			, (uint8_t)password.length());

		EEPROM_Pointer++;

		for (uint8_t i = 0; i < password.length(); i++)
		{
			EEPROM.write(EEPROM_Pointer, (uint8_t)password.charAt(i));
			EEPROM_Pointer++;
		}
	
	}
	EEPROM.commit();
	delay(100);
}



void loadData()
{
	//0 = OTA |LED_DATA  | active|mode|freq|color[3] |  WIFI_DATA | port|ssid|password
	int	EEPROM_Pointer = 0;

	if (EEPROM.read(EEPROM_Pointer) == OTAMode)
		OTAUpdateReq = true;
	//LED 
	if (EEPROM.read(EEPROM_Pointer + 1) != 0) {
		EEPROM_Pointer = 1;
		//LED1
		LED1_active = EEPROM.read(EEPROM_Pointer);
		EEPROM_Pointer++;

		LED1_mode = EEPROM.read(EEPROM_Pointer);
		EEPROM_Pointer++;

		LED1_freq = EEPROM.read(EEPROM_Pointer);
		EEPROM_Pointer++;

		for (byte i = 0; i < 3; i++)
		{
			color0[i] = EEPROM.read(EEPROM_Pointer);
			EEPROM_Pointer++;

		}
	}
	else
		EEPROM_Pointer = 7;


//	lcd.print((String)EEPROM_Pointer);
	go = EEPROM.read(EEPROM_Pointer);
	if (go != 0)
		port = go;
	EEPROM_Pointer++;

	go = EEPROM.read(EEPROM_Pointer);

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



	//lcd.print(port);
	//lcd.print(ssid);
	//lcd.print(password);


}


String WifiConnect()
{


	WiFi.begin(ssid.c_str(), password.c_str());

	
	int i = 0;
	while (WiFi.status() != WL_CONNECTED) {
		i++;
		if (i == 20) {
			FallBack();
			return "";
		}
			delay(500);
	}


	server.begin();
	connected = true;

	return (WiFi.localIP().toString() + ":" + port);

}



void FallBack()
{
	
	WiFi.mode(WIFI_AP);
	WiFi.softAP("ESPCONFIG");
	Serial.println(WiFi.softAPIP());
	Serial.println(port);
	server.begin();

	byte go = 0;
	int counter = 0;
	bool gotdata[3] = { false,false,false };
	while (!client.connected()) {
		// try to connect to a new client
		client = server.available(); delay(10);
	}

	while (counter < 3) {

		while (client.connected()) {
			if (counter == 3)
				break;
			delay(100);

			while (client.available()>0) {
				//lcd.print(counter);
				byte req = client.read();

				// Read the first line of the request
				if (req == ControlByte::SSID) {
					if(!GetVerified(client.read(), true))
					break;
					ssid = String(WifiTemp);
					

					if (!gotdata[0])
						counter++;
					gotdata[0] = true;
				//	lcd.print(ssid);
					if (counter == 3)
						break;
				
				}

				else if (req == ControlByte::PWD) {

					if (!GetVerified(client.read(), true))
						break;
					password = String(WifiTemp);
				//	password.remove(run, password.length() - run);
					if (!gotdata[1])
						counter++;
					gotdata[1] = true;
				//	lcd.print(password);

					
					if (counter == 3)
						break;
				}

				else if (req == ControlByte::Port) {
					while (client.available() <= 0)
						delay(50);

					port = client.read();
					while (!client.available())
						delay(10);
					if (client.read() != Stop)
						break;
					if (!gotdata[2])
						counter++;
					gotdata[2] = true;
				
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
			delay(10);


		}


	}
	//lcd.print("ENDED");
	storeData(true); ESP.restart();
}


void setup() {

	EEPROM.begin(128);

	Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY,1);
	pinMode(0, OUTPUT);
	pinMode(2, OUTPUT);
	pinMode(3, OUTPUT);
	ssid = "ESP8266";
	password = "1234567890";
	port = 23;

//	EEPROM_Clear();

	connected = false;
	OTAUpdateReq = false;
	run = 0;

	analogWriteFreq(200);

loadData();

	if (OTAUpdateReq) {
		setColor(new byte[3]{ 255,0,0 });
		WiFi.mode(WIFI_STA);
		WiFi.begin(ssid.c_str(), password.c_str());
		int i = 0;
		while (WiFi.status() != WL_CONNECTED) {
			i++;
			if (i == 20)
			{
		
				EEPROM.write(0, 0);
				EEPROM.commit();
				ESP.restart();
			}
			delay(500);
		}


		connected = true;


	
		
	//	ArduinoOTA.setPassword((const char *)"1234567890");
		ArduinoOTA.onEnd([]() {

			EEPROM.write(0, 0);
			EEPROM.commit();
		});

		ArduinoOTA.begin();
		OTAUpdateReq = true;
	}
	else{
		WiFi.mode(WIFI_STA);
		WiFiServer server(port);
		IPString = WifiConnect();
		Serial.println(IPString);
		yield();
		StateWriter( LED1_mode);
		yield();
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

		
				if (client.available()) {
					byte go = 0;
					byte req = client.read();
					switch (req)
					{

					case LED1SwitchStade:
						if (GetVerified(1,false))
							ChangeLEDState(buffer[0]);
						break;

					case LED1Data:
						if (!GetVerified(3, false))
							break;

						color0[0] = buffer[0];
						color0[1] = buffer[1];
						color0[2] = buffer[2];
						ChangeLEDState(LED1_mode);
						break;
	
					case LEDState:
						if (!GetVerified(2, false))
							break;
						if (!buffer[0]) {
							LED1_active = buffer[1];
							ChangeLEDState(LED1_mode);
						}
						break;

					case LED1Frequency:
						if (!GetVerified(1, false))
							break;
						LED1_freq = buffer[0];
						break;
				
					case SaveStartupCFG:
						while (client.available() <= 0)
							delay(10);
						if (client.read() == Stop)
							storeData(false);
						break;
			
					case OTAUpdate:
						EEPROM.write(0, OTAMode);
						EEPROM.commit();
						ESP.restart();
						break;
			
					default:
						while (client.read() != Stop) {
							if (!client.available())
								break;
							delay(1);
						}
						break;
					}
					yield();
				}
				yield();
			
				LoopLedCall();
			}
			
		
	

			while (!client.connected()) {

				yield();
				
				client.stop();                                    // not connected, so terminate any prior client
				client = server.available();
				LoopLedCall();
			
			}

		}
	}
	else
	{
		ArduinoOTA.handle();

	}


}




void LoopLedCall()
{
	if (LED1_mode == SmoothState &&LED1_active)
	{
		Smooth();
	}
}



bool GetVerified(int count,bool Fallback)
{
	if (!Fallback) {
		for (size_t i = 0; i < count; i++)
		{
			while (!client.available()) {
				delay(10); yield();
			}
			buffer[i] = client.read();

		}

		if (client.read() == Stop)
			return true;
		else return false;
	}
	else
	{
	//	lcd.print(count);
		for (size_t i = 0; i < count; i++)
		{
			while (!client.available())
				delay(10);
			WifiTemp[i] = client.read();

		}

		if (client.read() == Stop)
			return true;
		else return false;
	}
}


void getRGB(int hue, int sat, int val) {
	hue = hue % 360;
		switch (hue / 60) {
		case 0:
			color0[0] = 255;
			color0[1] = (((255 )*hue) / 60) ;
			color0[2]  = 0;
			break;

		case 1:
			color0[0] = (((255 )*(60 - (hue % 60))) / 60) ;
			color0[1] = val;
			color0[2] = 0;
			break;

		case 2:
			color0[0] = 0;
			color0[1] = 255;
			color0[2] = (((255 )*(hue % 60)) / 60) ;
			break;

		case 3:
			color0[0] = 0;
			color0[1] = (((255 )*(60 - (hue % 60))) / 60) ;
			color0[2] = 255;
			break;

		case 4:
			color0[0] = (((255 )*(hue % 60)) / 60) ;
			color0[1] = 0;
			color0[2] = 255;
			break;

		case 5:
			color0[0] = 255;
			color0[1] = 0;
			color0[2] = (((255 )*(60 - (hue % 60))) / 60) ;
			break;
		}

		
	
}

//Spped with LED1_freq, LED1/2_active
void Smooth()
{
	if (LED1_counter >= LED1_freq)
	{
		LED1_counter = 0;
		if (color0Hue >= 65534)
			color0Hue = 0;
		else
			color0Hue++;

		getRGB(color0Hue, 255, 255);
		setColor(color0);
		delay(2);
	}
	else
	{
		LED1_counter++;
		setColor(color0);
		delay(1);
	}



	

}

void ChangeLEDState( uint8_t State) {
	
		LED1_mode = State;
	StateWriter( State);

}



void StateWriter( uint8_t State)
{
	if (State == FixColorState)
	{	
		if (LED1_active) {
			setColor(color0);
		}
		else {
			setColor(new byte[3]{ 0,0,0 });
		}	
	
	}
	else
		if (State == SmoothState)
		{
			LED1_counter = 0;
			if (LED1_freq == 0)
				LED1_freq = 120;
		}


}





void setColor(byte *mlt) {



		analogWrite(R1, mlt[0]*4);
		analogWrite(G1, mlt[1]*4);
		analogWrite(B1, mlt[2]*4);


	
}

void FixColor()
{
	
		setColor(color0);
	
}

