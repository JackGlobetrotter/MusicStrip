

#include "ESP8266WiFi.h"
#include "MusicStripeLibrary.h"

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

bool WifiConnect()
{


	if (ssid == "" | password == "" | port == 0)
		return false;
	WiFi.begin(ssid.c_str(), password.c_str());
	server = WiFiServer(port);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
	}


	connected = true;
	digitalWrite(0, HIGH);	digitalWrite(2, HIGH);
	Serial.println(WiFi.localIP().toString() + ":" + port);//

	return true;

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
			switch (Serial.read())
			{
			case ControlByte::SSID:
				if (Serial.available() <= 0)
					delay(50);
				ssid = Serial.readString();
				Serial.write((uint8_t)1);
				Serial.flush();
				break;
			case ControlByte::PWD:
				if (Serial.available() <= 0)
					delay(50);

				password = Serial.readString();
				Serial.write((uint8_t)1);
				Serial.flush();
				break;
			case ControlByte::Port:

				//logical OR keeps x_high intact in combined and fills in                                                             //rightmost 8 bits
				if (Serial.available() <= 0)
					delay(50);
				port = Serial.read();

				Serial.write((uint8_t)1);
				Serial.flush();
				break;
			case ControlByte::Connect:
				if (Serial.available() <= 0)
					delay(50);
				Serial.write((uint8_t)WifiConnect());
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
		}

		while (!client.connected()) {


			client.stop();                                    // not connected, so terminate any prior client
			client = server.available();


		}

	}


}

