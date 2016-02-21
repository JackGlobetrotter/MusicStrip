


#include <WiFiServer.h> 
#include <WiFiClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "..\MusicLibrary\src\_micro-api\libraries\MusicLibraryLib\src\MusicStripLib.h"

String ssid = "";
String password = "";
uint8_t port = 0;

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


	if (ssid == "" | password == "" | port == 0)
		return "";
	char* ssidbuffer;
	ssid.toCharArray(ssidbuffer, ssid.length(), 0);
	WiFi.begin(ssidbuffer, password.c_str());
	server = WiFiServer(port);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
	}


	connected = true;
	server =  WiFiServer(port);
	digitalWrite(0, HIGH);	digitalWrite(2, HIGH);
	return (WiFi.localIP().toString() + ":" + port);//

	//return true;

}

bool WifiDeconnect()
{
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
				Serial.print((String)WifiConnect());
				Serial.flush();
				break;



			}

		}
	}
	else
	{
		// Check if a client has connected
		while (client.available()) {
			byte req = client.read();
			// Read the first line of the request
			if (req == ControlByte::Disconnect)
				WifiDeconnect();
			else
				Serial.write(req);
			while (client.available())
			{
				req = client.read();
				if (req == ControlByte::Stop)
					client.flush();
				Serial.write(req);
			}
		}

		while (!client.connected()) {


			client.stop();                                    // not connected, so terminate any prior client
			client = server.available();


		}

	}


}

