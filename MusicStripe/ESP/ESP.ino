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
#include <LiquidCrystal.h>

#define WIFIPointer 0

#define SIZE    255
#define DELAY    20
#define HUE_MAX  6.0
#define HUE_DELTA 0.01

#define LightPin 15


//----------LED1
#define R1 4
#define G1 5
#define B1 16

//---------LED 2

#define R2 12
#define G2 13
#define B2 14


bool LED2_active = false;
bool LED1_active = false;

byte color0[] = { 0,0,0 };
byte color1[] = { 0,0,0 };

//long deltas[3] = { 5, 6, 7 };
long LED1Smooth[3];
long LED2Smooth[3];
long LED1rgbval;
long LED2rgbval;
// for reasons unknown, if value !=0, the LED doesn't light. Hmm ...
// and saturation seems to be inverted
float LED1hue = 0.0, LED1saturation = 1, LED1value = 1;
float LED2hue = 0.0, LED2saturation = 1, LED2value = 1;
/*
chosen LED SparkFun sku: COM-09264
has Max Luminosity (RGB): (2800, 6500, 1200)mcd
so we normalize them all to 1200 mcd -
R  250/600  =  107/256
G  250/950  =   67/256
B  250/250  =  256/256
*/
long LED1bright[3] = { 107, 67, 256 };
long LED2bright[3] = { 107, 67, 256 };
//long bright[3] = { 256, 256, 256};
long LED1k, LED1temp_value;
long LED2k, LED2temp_value;
int LED1pulsedelay = 0;
int LED2pulsedelay = 0;
// ----- Definitions



//IRrecv irrecv(IRPin);
//decode_results results;

// ----- RuntimeVariabes
char WifiTemp[50];
uint8_t temp[3];
uint8_t run = 0;
uint8_t ctrl = 0;
//-------Fade-------------


int LED1_freq = 500;
int LED2_freq = 500;
int LED1_counter = 0; //+-5
int LED2_counter = 0; //+-5

int LED1_mscounter = 0; //ms to freq
int LED2_mscounter = 0;
bool LED1_FlashOn;
int Display_freq;
int Display_mscounter;
int Display_counter;
bool Display_scroll = false;

uint8_t Display_max;

uint8_t LED1_mode = FixColorState;
uint8_t LED2_mode = FixColorState;
bool blinking = false;


bool scrollup = true;
bool fadeup = true;
uint8_t scrollcount = 0;
uint8_t fadecount = 0;
//LiquidCrystal lcd(RS, Enable, DR4, DR5, DR6, DR7);
int go;

bool WifiIsConnected = false;
String IPString = "";
bool LightStat = false;

uint8_t buffer[4];

#define OTAMode 26
#define EEPROMSize 512

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
		EEPROM.write(EEPROM_Pointer, LED2_active);


		EEPROM_Pointer++;
		EEPROM.write(EEPROM_Pointer, LED2_mode);

		EEPROM_Pointer++;
		EEPROM.write(EEPROM_Pointer, LED2_freq);


		EEPROM_Pointer++;

		for (byte i = 0; i < 3; i++)
		{
			EEPROM.write(EEPROM_Pointer, color1[i]);
			EEPROM_Pointer++;

		}

	}
	else
	{
		EEPROM_Pointer = 13;
		//WIFI_DATA | port|ssid|password
	//	Serial.println((String)EEPROM_Pointer + ":" + (String)port);

		EEPROM.write(EEPROM_Pointer
			, port);

		EEPROM_Pointer++;
		//Serial.println((String)EEPROM_Pointer + ": l: " + (String)ssid.length()+" ssid: "+ssid);

		EEPROM.write(EEPROM_Pointer
			, (uint8_t)ssid.length());
		EEPROM_Pointer++;


		for (uint8_t i = 0; i < ssid.length(); i++)
		{
			EEPROM.write(EEPROM_Pointer, (uint8_t)ssid.charAt(i));
			EEPROM_Pointer++;
		}

		//Serial.println((String)EEPROM_Pointer + ": l: " + (String)password.length() + " password: " + password);

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

}



void loadData()
{
	//0 = OTA |LED_DATA  | active|mode|freq|color[3] |  WIFI_DATA | port|ssid|password
	int	EEPROM_Pointer = 0;
	int mode = EEPROM.read(EEPROM_Pointer);
	if (mode == OTAMode)
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
	if (EEPROM.read(EEPROM_Pointer + 1) != 0) {
		//LED2

		LED2_active = EEPROM.read(EEPROM_Pointer);
		EEPROM_Pointer++;

		LED2_mode = EEPROM.read(EEPROM_Pointer);
		EEPROM_Pointer++;

		LED2_freq = EEPROM.read(EEPROM_Pointer);
		EEPROM_Pointer++;

		for (byte i = 0; i < 3; i++)
		{
			color1[i] = EEPROM.read(EEPROM_Pointer);
			EEPROM_Pointer++;

		}

	}
	else
		EEPROM_Pointer = 13;

//	Serial.println((String)EEPROM_Pointer);
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

	//Serial.println(port);
	//Serial.println(ssid);
	//Serial.println(password);


}


String WifiConnect()
{


	WiFi.begin(ssid.c_str(), password.c_str());

	server = WiFiServer(port);
	server.begin();
	int i = 10;
	while (WiFi.status() != WL_CONNECTED) {
		i++;
		if (i == 5) {
			FallBack();
			return "";
		}
			delay(500);
	}


	connected = true;

	return (WiFi.localIP().toString() + ":" + port);

}



void FallBack()
{
	Serial.println("Fallback");
	
	WiFi.mode(WIFI_AP);
	WiFi.softAP("ESPCONFIG");

	IPAddress ip = WiFi.softAPIP();
	IPString = (String)ip.toString() + ":"+(String)port;
	DisplayIP();
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
				//Serial.println(counter);
				byte req = client.read();

				// Read the first line of the request
				if (req == ControlByte::SSID) {
					if(!GetVerified(client.read(), true))
					break;
					ssid = String(WifiTemp);
					

					if (!gotdata[0])
						counter++;
					gotdata[0] = true;
				//	Serial.println(ssid);
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
				//	Serial.println(password);

					
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
	//Serial.println("ENDED");
	storeData(true); ESP.restart();
}





void setup() {

	pinMode(LightPin, OUTPUT);



	EEPROM.begin(512);
	Serial.begin(115200);
	//lcd.begin(16, 2);
	ssid = "ESP8266";
	password = "1234567890";
	port = 23;

	WiFiServer server(23);

	connected = false;
	OTAUpdateReq = false;
	run = 0;
	analogWriteRange(254);
	analogWriteFreq(200);
	//FallBack();
loadData();

//lcd.clear();
//while (true) delay(100000);
	if (OTAUpdateReq) {

		WiFi.mode(WIFI_STA);
		WiFi.begin(ssid.c_str(), password.c_str());
		int i = 0;
		while (WiFi.status() != WL_CONNECTED) {
			i++;
			if (i == 20)
			{
				Serial.print("Connection err");
				EEPROM.write(0, 0);
				EEPROM.commit();
				ESP.restart();
			}
			delay(500);
		}


		connected = true;


		IPString = WiFi.localIP().toString() + ":OTA";//
		DisplayIP();
		ArduinoOTA.onStart([]() {
			Serial.print("Updating");
		//	lcd.setCursor(0, 1);
		});
	//	ArduinoOTA.setPassword((const char *)"1234567890");
		ArduinoOTA.onEnd([]() {
			Serial.print("finished");
			EEPROM.write(0, 0);
			EEPROM.commit();
		});

		ArduinoOTA.onError([](ota_error_t error) {
			Serial.write(OTAError);
			if (error == OTA_AUTH_ERROR) Serial.print("AUTH_ERROR");
			else if (error == OTA_BEGIN_ERROR) Serial.print("BEGIN_ERROR");
			else if (error == OTA_CONNECT_ERROR) Serial.print("CONNECT_ERROR");
			else if (error == OTA_RECEIVE_ERROR) Serial.print("RECEIVE_ERROR");
			else if (error == OTA_END_ERROR) Serial.print("END_ERROR");
		});
		ArduinoOTA.begin();
		OTAUpdateReq = true;
	}
	else{
		WiFi.mode(WIFI_STA);
		IPString = WifiConnect();
		DisplayIP();
		yield();
		StateWriter(0, LED1_mode);
		yield();
		StateWriter(1, LED2_mode);
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

		
				while (client.available()) {
					byte go = 0;
					byte req = client.read();
					switch (req)
					{

					case LED1SwitchStade:
						if (GetVerified(1,false))
							ChangeLEDState(true, buffer[0]);

						break;

					case LED2SwitchStade:
						if (GetVerified(1, false))
							ChangeLEDState(false, buffer[0]);
						break;
					case LightToggle:
						if (client.available() <= 0)
							delay(50);
						temp[0] = client.read();

						ToggleLightSwitch((bool)temp[0]);

						//	}
						break;
					case GetLightState:

						client.write((uint8_t)LightStat);

						break;

					case LED1Data:
						if (!GetVerified(3, false))
							break;


						color0[0] = buffer[0];
						color0[1] = buffer[1];
						color0[2] = buffer[2];
						ChangeLEDState(0, LED1_mode);




						break;
					case LED2Data:

						if (!GetVerified(3, false))
							break;

						color1[0] = buffer[0];
						color1[1] = buffer[1];
						color1[2] = buffer[2];
						ChangeLEDState(1, LED2_mode);



						break;
					case LEDState:
						if (!GetVerified(2, false))
							break;

						if (!buffer[0]) {
							LED1_active = buffer[1];
							ChangeLEDState(true, LED1_mode);

						}
						else {
							LED2_active = buffer[1];
							ChangeLEDState(false, LED2_mode);
						}break;

					case LED1Frequency:
						if (!GetVerified(1, false))
							break;
						LED1_freq = buffer[0];
						break;
					case LED2Frequency:
						if (!GetVerified(1, false))
							break;
						LED2_freq = buffer[0];

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
						while (client.available())
							Serial.print((byte)100);
						break;
					}
					yield();
				}
				yield();
				LoopLedCall();
			}
			
			if(!client.connected())
			DisplayIP();

			while (!client.connected()) {

				yield();
				
				client.stop();                                    // not connected, so terminate any prior client
				client = server.available();
				LoopLedCall();
				if(client.connected())
					DisplayColorStripe();
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


	if (LED1_active)
		switch (LED1_mode)
		{
		case SmoothState:
			Smooth(0);

			break;
		case FadeState:
			Fadeing(false);
			break;
		case FlashState:
			if (LED1_freq == LED1_mscounter) {
				if (LED1_FlashOn) {
					setColor(new byte[3]{ 0,0,0 }, 0);
					LED1_FlashOn = false;
				}
				else
				{
					setColor(color0, 0);
					LED1_FlashOn = true;
				}
				LED1_mscounter = 0;

			}
			else {
				LED1_mscounter++;
				delay(5);
			}
			break;







		}
}



void ToggleLightSwitch(bool OnOff)
{
	LightStat = OnOff;
	digitalWrite(LightPin, LightStat);
	

}


byte flash[8] = {
	0b10000,
	0b11000,
	0b11100,
	0b11110,
	0b11110,
	0b11100,
	0b11000,
	0b10000
};




bool GetVerified(int count,bool Fallback)
{
	if (!Fallback) {
		for (size_t i = 0; i < count; i++)
		{
			while (!client.available())
				delay(10);
			buffer[i] = client.read();

		}

		if (client.read() == Stop)
			return true;
		else return false;
	}
	else
	{
	//	Serial.println(count);
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

//void loop()
//{
//
//	while (Serial.available()) {
//		ctrl = Serial.read();
//		switch (ctrl) {
//		case OTAUpdate:
//			WifiUpdate();
//			break;
//
//		case LED1SwitchStade:
//			if (GetVerified(1))
//				ChangeLEDState(true, buffer[0]);
//
//			break;
//
//		case LED2SwitchStade:
//			if (GetVerified(1))
//				ChangeLEDState(false, buffer[0]);
//			break;
//		case LightToggle:
//			if (Serial.available() <= 0)
//				delay(50);
//			temp[0] = Serial.read();
//
//			ToggleLightSwitch((bool)temp[0]);
//
//			//	}
//			break;
//		case GetLightState:
//
//			Serial.write((uint8_t)LightStat);
//
//			break;
//
//		case LED1Data:
//			if (!GetVerified(3))
//				break;
//
//
//			color0[0] = buffer[0];
//			color0[1] = buffer[1];
//			color0[2] = buffer[2];
//			ChangeLEDState(true, LED1_mode);
//
//
//
//
//			break;
//		case LED2Data:
//
//			if (!GetVerified(3))
//				break;
//
//			color1[0] = buffer[0];
//			color1[1] = buffer[1];
//			color1[2] = buffer[2];
//			ChangeLEDState(false, LED2_mode);
//
//
//
//			break;
//		case LEDState:
//			if (!GetVerified(2))
//				break;
//
//			if (!buffer[0]) {
//				LED1_active = buffer[1];
//				ChangeLEDState(true, LED1_mode);
//
//			}
//			else {
//				LED2_active = buffer[1];
//				ChangeLEDState(false, LED2_mode);
//			}break;
//
//		case LED1Frequency:
//			if (!GetVerified(1))
//				break;
//			LED1_freq = buffer[0];
//			break;
//		case LED2Frequency:
//			if (!GetVerified(1))
//				break;
//			LED2_freq = buffer[0];
//
//			break;
//		case ClientLeft:
//			DisplayIP();
//			break;
//		case SaveStartupCFG:
//			while (Serial.available() <= 0)
//				delay(10);
//			if (Serial.read() == Stop)
//				storeData(false);
//			break;
//		case ClientArrived:
//			DisplayColorStripe();
//			break;
//		default:
//
//			break;
//		}
//
//
//	}
//	if (LED1_active)
//		switch (LED1_mode)
//		{
//		case SmoothState:
//			Smooth(0);
//
//			break;
//		case FadeState:
//			Fadeing(false);
//			break;
//		case FlashState:
//			if (LED1_freq == LED1_mscounter) {
//				if (LED1_FlashOn) {
//					setColor(new byte[3]{ 0,0,0 }, 0);
//					LED1_FlashOn = false;
//				}
//				else
//				{
//					setColor(color0, 0);
//					LED1_FlashOn = true;
//				}
//				LED1_mscounter = 0;
//
//			}
//			else {
//				LED1_mscounter++;
//				delay(5);
//			}
//			break;
//
//
//
//		}
//}

void 	DisplayIP() {
	//lcd.clear();
	if (IPString.length() > 16) {
		Serial.print(IPString.substring(0, 15));
	//	lcd.setCursor(0, 1);
		Serial.print(IPString.substring
			(16, IPString.length()));
	}
	else
		Serial.println(IPString);

}
//LED1/2_freq, LED1/2_active > LED1/2_mscounter
void Flash(bool LEDStripe)
{
	if (LED1_active && !LEDStripe) {
		LED1_mscounter++;
		if (LED1_mscounter < LED1_freq / 2) {
			FixColor(LEDStripe);

		}
		else
		{
			byte out[] = { 0,0,0 };
			setColor(out, 0);
		}
		if (LED1_mscounter == LED1_freq)
			LED1_mscounter = 0;
	}
	if (LED2_active && LEDStripe) {
		LED2_mscounter++;
		if (LED2_mscounter < LED2_freq / 2) {
			FixColor(LEDStripe);

		}
		else
		{
			byte out[] = { 0,0,0 };
			setColor(out, 0);
		}
		if (LED2_mscounter == LED2_freq)
			LED2_mscounter = 0;


	}
}

//Spped with LED1_freq, LED1/2_active
void Smooth(bool LEDStripe)
{
	int divider = !LED1_active * 6 + !LED2_active*+6;

	if (LED1_active && !LEDStripe) {

		LED1hue += HUE_DELTA;
		if (LED1hue > HUE_MAX) {
			LED1hue = 0.0;
		}
		LED1rgbval = HSV_to_RGB(LED1hue, LED1saturation, LED1value);
		LED1Smooth[0] = (LED1rgbval & 0x00FF0000) >> 16; // there must be better ways
		LED1Smooth[1] = (LED1rgbval & 0x0000FF00) >> 8;
		LED1Smooth[2] = LED1rgbval & 0x000000FF;

		analogWrite(R1, LED1Smooth[0] * LED1bright[0] / 256);
		analogWrite(G1, LED1Smooth[1] * LED1bright[1] / 256);
		analogWrite(B1, LED1Smooth[2] * LED1bright[2] / 256);
		delay(LED1_freq / divider);
	}
	if (LED2_active && LEDStripe) {
		LED2hue += HUE_DELTA;
		if (LED2hue > HUE_MAX) {
			LED2hue = 0.0;
		}
		LED2rgbval = HSV_to_RGB(LED2hue, LED2saturation, LED2value);
		LED2Smooth[0] = (LED2rgbval & 0x00FF0000) >> 16; // there must be better ways
		LED2Smooth[1] = (LED2rgbval & 0x0000FF00) >> 8;
		LED2Smooth[2] = LED2rgbval & 0x000000FF;
		analogWrite(R2, LED2Smooth[0] * LED2bright[0] / 256);
		analogWrite(G2, LED2Smooth[1] * LED2bright[1] / 256);
		analogWrite(B2, LED2Smooth[2] * LED2bright[2] / 256);
		delay(LED2_freq / divider);
	}

}


void DisplayColorStripe()
{

	//lcd.clear();
	Serial.println(GetDisplayString(0));
	//lcd.setCursor(0, 1);
	Serial.println(GetDisplayString(1));
}

void ChangeLEDState(bool One, uint8_t State) {
	if (One) {
		LED1_mode = State;


	}
	else {
		LED2_mode = State;

	}
	StateWriter(One, State);

}

void HardSaveData() {}

void StateWriter(bool One, uint8_t State)
{
	switch (State)
	{
	case FixColorState:


		if (!One&&LED1_active) {
			setColor(color0, 0);
		}
		else if (!One&&!LED1_active) {
			setColor(new byte[3]{ 0,0,0 }, 0);
		}
		else if (One&&LED2_active) {
			setColor(color1, 1);
		}
		else {
			setColor(new byte[3]{ 0,0,0 }, 1);
		}

		break;

	case FadeState:

		LED1_counter = 0;
		LED1_mscounter = 0;
		fadeup = true;

		break;
	case FlashState:
		if (One)
			LED1_mscounter = 0;
		else
			LED1_mscounter = 0;
		// = 0;
		fadeup = true;

		break;
	}

	DisplayColorStripe();

}

String GetDisplayString(bool Line)
{

	String ret;
	if (!Line) {
		if (LED1_active)
		{
			ret = String(LED1_mode, DEC) + ": " + (String)color0[0] + "/" + (String)color0[1] + "/" + (String)color0[2];
		}
		else
		{
			ret = "------------";
		}
	}
	else {
		if (LED2_active)
		{
			ret = String(LED2_mode, DEC) + ": " + (String)color1[0] + "/" + (String)color1[1] + "/" + (String)color1[2];
		}
		else
		{
			ret = "------------";
		}
	}
	return ret;
}







void Fadeing(bool LEDStripe) //LED1_counter over 100 =  {X} over {color0[1,2,3]}
{
	int divider = LED1_active * 4 + LED2_active*+4;

	if (LED1_active && !LEDStripe) {
		if (LED1_mscounter >= LED1_freq / divider) {
			LED1_mscounter = 0;
			if (fadeup)
				LED1_counter = LED1_counter + 1;
			else
				LED1_counter = LED1_counter - 1;
			byte currentcolor[] = { (color0[0] * LED1_counter / 100),(color0[1] * LED1_counter / 100) ,(color0[2] * LED1_counter / 100) };
			setColor(currentcolor, 0);

			if (LED1_counter == 100)
				fadeup = false;
			if (LED1_counter == 0)
				fadeup = true;


		}
		else
			LED1_mscounter++;

	}

	if (LED2_active && LEDStripe) {
		if (LED2_mscounter >= LED2_freq / divider) {
			LED2_mscounter = 0;
			if (fadeup)
				LED2_counter = LED2_counter + 1;
			else
				LED2_counter = LED2_counter - 1;
			byte currentcolor[] = { (color1[0] * LED2_counter / 100),(color1[1] * LED2_counter / 100) ,(color1[2] * LED2_counter / 100) };
			setColor(currentcolor, 0);

			if (LED2_counter == 100)
				fadeup = false;
			if (LED2_counter == 0)
				fadeup = true;


		}
		else
			LED2_mscounter++;
	}
	delay(1);
}




void SoundPulsation()
{
	/*if (Serial.available() >= 3)
	{

	Serial.readBytes(mix, 3);
	Serial.flush();
	//delay(100);
	}

	//setColor(mix);
	*/
}

void setColor(byte *mlt, bool stripe) {


	if (!stripe)
	{
		analogWrite(R1, mlt[0]*4);
		analogWrite(G1, mlt[1]*4);
		analogWrite(B1, mlt[2]*4);


	}
	else
	{
		analogWrite(R2, mlt[0]);
		analogWrite(G2, mlt[1]);
		analogWrite(B2, mlt[2]);
	}
}

void FixColor(bool LEDStripe)
{
	if (LED1_active && LEDStripe)
		setColor(color0, 0);
	if (LED2_active && LEDStripe)
		setColor(color1, 1);
}




long HSV_to_RGB(float h, float s, float v) {
	/* modified from Alvy Ray Smith's site: http://www.alvyray.com/Papers/hsv2rgb.htm */
	// H is given on [0, 6]. S and V are given on [0, 1].
	// RGB is returned as a 24-bit long #rrggbb
	int i;
	float m, n, f;

	// not very elegant way of dealing with out of range: return black
	if ((s<0.0) || (s>1.0) || (v<1.0) || (v>1.0)) {
		return 0L;
	}

	if ((h < 0.0) || (h > 6.0)) {
		return long(v * 255) + long(v * 255) * 256 + long(v * 255) * 65536;
	}
	i = floor(h);
	f = h - i;
	if (!(i & 1)) {
		f = 1 - f; // if i is even
	}
	m = v * (1 - s);
	n = v * (1 - s * f);
	switch (i) {
	case 6:
	case 0:
		return long(v * 255) * 65536 + long(n * 255) * 256 + long(m * 255);
	case 1:
		return long(n * 255) * 65536 + long(v * 255) * 256 + long(m * 255);
	case 2:
		return long(m * 255) * 65536 + long(v * 255) * 256 + long(n * 255);
	case 3:
		return long(m * 255) * 65536 + long(n * 255) * 256 + long(v * 255);
	case 4:
		return long(n * 255) * 65536 + long(m * 255) * 256 + long(v * 255);
	case 5:
		return long(v * 255) * 65536 + long(m * 255) * 256 + long(n * 255);
	}
}

