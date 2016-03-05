#include <WiFiClient.h>

#include <ESP8266WiFi.h>

#include "..\MusicLibrary\src\_micro-api\libraries\MusicLibraryLib\src\MusicStripLib.h"

String ssid = "";
String password = "";
uint8_t port = 0;
enum ControlByte;
WiFiServer server(23);
WiFiClient client;
bool connected = false;

void setup() {

	Serial.begin(115200);
	Serial.flush();

	pinMode(0, OUTPUT); pinMode(2, OUTPUT);
	digitalWrite(0, LOW);	digitalWrite(2, LOW);
	
}

String WifiConnect()
{

	if (ssid == "" || password == "" || port == 0)
		return "False data";

	char ssidbuffer[ssid.length() + 1];
	ssid.toCharArray(ssidbuffer, ssid.length(), 0);

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
	Serial.write(ControlByte::Disconnect);
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


void loop() {
	if (!connected) {
		if (Serial.available()> 0)
		{
			switch ((uint8_t)Serial.read())
			{
			case ControlByte::SSID:
				while (Serial.available() <= 3)
					delay(50);
				ssid = Serial.readString();
				Serial.write((uint8_t)1);
				Serial.flush();
				break;
			case ControlByte::PWD:
				while (Serial.available() <= 3)
					delay(50);

				password = Serial.readString();
				Serial.write((uint8_t)1);
				Serial.flush();
				break;
			case ControlByte::Port:

				//logical OR keeps x_high intact in combined and fills in                                                             //rightmost 8 bits
				while (Serial.available() <= 0)
					delay(50);
				port = Serial.read();

				Serial.write((uint8_t)1);
				Serial.flush();
				break;
			case ControlByte::Connect:		

				
				Serial.print(WifiConnect());
				Serial.flush();
				break;



			}

		}
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
			case Disconnect:
				WifiDeconnect();
				break;
			case GetLightState:
				Serial.write(req);
				while (Serial.available() <= 0)
					delay(50);
				client.write(Serial.read());
				break;
			case SSID:
				Serial.write(req);
				while (client.available() <= 0)
					delay(100);
				go = client.read();
				Serial.write(go);
				for (int i = 0; i < go; i++)
				{		
					while (client.available() <= 0)
						delay(100);
					Serial.write(client.read());
				}
				break;
			case PWD:
				Serial.write(req);
				go = (byte)client.read();
				Serial.write(go);
				for (int i = 0; i < go; i++)
				{
					Serial.write(client.read());
				}
				break;
			case Port:
				Serial.write(req);
				Serial.write(client.read());
				
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

			
		}
		
		}

		while (!client.connected()) {


			client.stop();                                    // not connected, so terminate any prior client
			client = server.available();


		}

	}


}

