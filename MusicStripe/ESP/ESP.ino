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

#define LightPin 15


//----------LED1
#define R1 4
#define G1 14
#define B1 16

bool LED1_active = false;

byte color0[] = { 0,0,0 };

//long deltas[3] = { 5, 6, 7 };
long LED1Smooth[3];
long LED1rgbval;
// for reasons unknown, if value !=0, the LED doesn't light. Hmm ...
// and saturation seems to be inverted
float LED1hue = 0.0, LED1saturation = 1, LED1value = 1;
/*
chosen LED SparkFun sku: COM-09264
has Max Luminosity (RGB): (2800, 6500, 1200)mcd
so we normalize them all to 1200 mcd -
R  250/600  =  107/256
G  250/950  =   67/256
B  250/250  =  256/256
*/
long LED1bright[3] = { 107, 67, 256 };
//long bright[3] = { 256, 256, 256};
long LED1k, LED1temp_value;
int LED1pulsedelay = 0;
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
		EEPROM_Pointer = 13;
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

	server = WiFiServer(port);
	server.begin();
	int i = 0;
	while (WiFi.status() != WL_CONNECTED) {
		i++;
		if (i == 10) {
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
	Serial.begin(115200);
	ssid = "ESP8266";
	password = "1234567890";
	port = 23;

	EEPROM_Clear();

	connected = false;
	OTAUpdateReq = false;
	run = 0;

	analogWriteFreq(200);

loadData();

	if (OTAUpdateReq) {
		
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
						if (!GetVerified(1, false))
							break;

						
							LED1_active = buffer[1];
							ChangeLEDState(LED1_mode);

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
				//	case MusicState:
				//		SoundPulsation(); break;

				
					case OTAUpdate:


						EEPROM.write(0, OTAMode);
						EEPROM.commit();
						ESP.restart();
						break;
			
					default:
	
						break;
					}
					yield();
				}
				yield();
				//client.write((uint8_t)174);
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


	if (LED1_active)
		switch (LED1_mode)
		{
		case SmoothState:
			Smooth(0);

			break;
		
		default:break;

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




//LED1/2_freq, LED1/2_active > LED1/2_mscounter
void Flash()
{
	if (LED1_active) {
		LED1_mscounter++;
		if (LED1_mscounter < LED1_freq / 2) {
			FixColor();

		}
		else
		{
			byte out[] = { 0,0,0 };
			setColor(out);
		}
		if (LED1_mscounter == LED1_freq)
			LED1_mscounter = 0;
	}
	
}

//Spped with LED1_freq, LED1/2_active
void Smooth(bool LEDStripe)
{
	int divider = !LED1_active * 6 ;

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

}



void ChangeLEDState( uint8_t State) {
	
		LED1_mode = State;



	StateWriter( State);

}



void StateWriter( uint8_t State)
{
	switch (State)
	{
	case FixColorState:


		if (LED1_active) {
			setColor(color0);
		}
		else {
			setColor(new byte[3]{ 0,0,0 });
		}
		
		break;


	
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

