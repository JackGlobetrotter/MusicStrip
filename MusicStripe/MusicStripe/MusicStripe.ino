


#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <MusicStripLib.h>
#include <EEPROM.h>


#define WIFIPointer 0

#define SIZE    255
#define DELAY    20
#define HUE_MAX  6.0
#define HUE_DELTA 0.01

//----pinout def--------


#define Reset 13
#define LightPin 4
#define ESPReset 7
#define ESPFactory 8
//-----display

#define DR4 A0
#define DR5 A1
#define DR6 A2
#define DR7 A3

#define Enable A4
#define RS  A5

//----------LED1
#define R1 3
#define G1 5
#define B1 6

//---------LED 2

#define R2 9
#define G2 10
#define B2 11


bool LED2_active = false;
bool LED1_active = false;

byte color0[] = { 128,10,70 };
byte color1[] = { 255,180,125 };

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

uint8_t temp[10];
uint8_t run = 0;
uint8_t ctrl = 0;
//-------Fade-------------


int LED1_freq = 500;
int LED2_freq = 500;
int LED1_counter = 0; //+-5
int LED2_counter = 0; //+-5

int LED1_mscounter = 0; //ms to freq
int LED2_mscounter = 0;

int Display_freq ;
int Display_mscounter;
int Display_counter;
bool Display_scroll = false;

uint8_t Display_max;

uint8_t LED1_mode = 1;
uint8_t LED2_mode = 1;
bool blinking = false;


bool scrollup = true;
bool fadeup = true;
uint8_t scrollcount = 0;
uint8_t fadecount = 0;
LiquidCrystal lcd(RS, Enable, DR4, DR5, DR6, DR7);


String _ssid = "test";
String _pwd = "1234567890";
uint8_t _port = 80;
bool WifiIsConnected = false;
String IPString = "";
bool LightStat = true;
int EEPROM_Pointer = 0;
uint8_t buffer[3];
SoftwareSerial ss(13,12);

void ResetEEPROM()
{

	for (int i = 0; i < EEPROM.length(); i++) {
		EEPROM.write(i, 0);
	}

}

void InitalizeWifi(bool Updated) {
	if (!Updated)

	{
		digitalWrite(ESPReset, LOW);
		delay(50);
		digitalWrite(ESPFactory, HIGH);
		digitalWrite(ESPReset, HIGH);

	}

	String ret;
	int timer;
	int IpPointCounter = 0;
		 while (true)
		 {
			 
			 while (Serial.available() < 5) {
				 delay(50);
				 timer++;
				 if (timer == 5000)
					 ESPHardReset();
			 }
			  ret = Serial.readString();
		
			 for (size_t i = 0; i < ret.length(); i++)
			 {
				 if (ret[i] == '.')
					 IpPointCounter++;
			 }
			 if (IpPointCounter != 3)
			 {
				 IpPointCounter = 0;
			 }
			 else if (ret == "failed")
				 ESPHardReset();
			 else
				 break;
		 }
		 IPString = ret;
		 if (ret.substring(ret.length() - 3, ret.length()) == "OTA")
			 WifiUpdate();

}

void setup()
{
	Serial.begin(115200);

	pinMode(Reset, INPUT);

	if(!digitalRead(Reset))
	ResetEEPROM();
	else
	loadData();

	lcd.begin(16, 2);
	lcd.clear();
	pinMode(ESPReset, OUTPUT);
	pinMode(ESPFactory, OUTPUT);

	InitalizeWifi(false);
	DisplayIP();


	pinMode(LightPin, OUTPUT);
	pinMode(R1, OUTPUT);
	pinMode(R2, OUTPUT);
	pinMode(G1, OUTPUT);
	pinMode(G2, OUTPUT);
	pinMode(B1, OUTPUT);
	pinMode(B2, OUTPUT);


	digitalWrite(R1, LOW);
	digitalWrite(R2, LOW);
	digitalWrite(G1, LOW);
	digitalWrite(G2, LOW);
	digitalWrite(B1, LOW);
	digitalWrite(B2, LOW);





}



void loadData()
{
	LightStat = EEPROM.read(EEPROM_Pointer);
	EEPROM_Pointer++;
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



void ToggleLightSwitch(bool OnOff)
{
	LightStat = OnOff;
	digitalWrite(LightPin, !LightStat);


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

void ESPHardReset()
{
	lcd.clear();
	lcd.print("ESP Hard Reset");

	digitalWrite(ESPReset, LOW);
	delay(50);
	digitalWrite(ESPFactory, HIGH);
	digitalWrite(ESPReset, HIGH);
	delay(50);
	digitalWrite(ESPFactory, LOW);

	delay(200);
	
	delay(300);
	Serial.flush();
	digitalWrite(ESPFactory, HIGH);


	String ret;
	int IpPointCounter;
	while (true)
	{
		while (Serial.available() < 5) {
			delay(50);

		}
		ret = Serial.readString();

		for (size_t i = 0; i < ret.length(); i++)
		{
			if (ret[i] == '.')
				IpPointCounter++;
		}
		if (IpPointCounter == 3)
			break;
		else
			IpPointCounter = 0;
		
	
	}
	IPString = ret;
	DisplayIP();
	while (true) { delay(1000); };
}

void WifiUpdate()
{
	lcd.createChar(0, flash);
	lcd.clear();
	lcd.print("ESP Updating");
	lcd.setCursor(0, 1);
	lcd.print("----------------");
	lcd.noCursor();
	lcd.setCursor(0, 1);
	int passing = -1;
	int print = 0;
	bool gotend = false;
	byte combination[3];
	while (!gotend) {
		while  (Serial.available() > 2)
		{
			combination[2] = combination[1];
			combination[1] = combination[0];
			combination[0] = Serial.read();
			
			if (combination[2] == Start&&combination[1] == OTAEnd &&combination[0] == Stop) {
				gotend = true;

				break;
			}
		}
	
		if (print == 3) 
		{
			passing++;
			if (passing == 0) {
				lcd.setCursor(16, 1);

				lcd.print("-");
				lcd.setCursor(0, 1);
				lcd.write(byte(0));
				
			}
			else {
				lcd.setCursor(passing - 1, 1);
				lcd.print("-");
				lcd.write(byte(0));
				if (passing == 16)
					passing = -1;
			}
			print = 0;
		}
		else
		print++;
		delay(50);
		
	}

	InitalizeWifi(true);
	DisplayIP();
}

bool GetVerified(int count)
{
	for (size_t i = 0; i < count; i++)
	{
		while (!Serial.available())
			delay(10);
		buffer[i] = Serial.read();

	}
	Serial.println();
	if (Serial.read() == Stop)
		return true;
	else return false;

}

void loop()
{
	
	while (Serial.available()){
		ctrl = Serial.read();
		switch (ctrl) {
		case OTAUpdate:
			WifiUpdate();
			break;

		case LED1SwitchStade:
			if(GetVerified(1))
			ChangeLEDState(true,buffer[0]);
		
			break;

		case LED2SwitchStade:
			if (GetVerified(1))
				ChangeLEDState(false,buffer[0]);
			break;
		case LightToggle:
			if (Serial.available() <= 0)
				delay(50);
			temp[0] = Serial.read();
			EEPROM.write(0, temp[0]);
			ToggleLightSwitch((bool)temp[0]);

			//	}
			break;
		case GetLightState:

			Serial.write((uint8_t)LightStat);

			break;

		case LED1Data:
			if (!GetVerified(3))
				break;
	
			if (Serial.read() == Stop) {
				color0[0] = buffer[0];
				color0[1] = buffer[1];
				color0[2] = buffer[2];
				ChangeLEDState(true, LED1_mode);
				EEPROM.write(4, color0[0]);
				EEPROM.write(5, color0[1]);
				EEPROM.write(6, color0[2]);

			}
		
			break;
		case LED2Data:

			if (!GetVerified(3))
				break;
			if (Serial.read() == Stop) {
				color0[0] = buffer[0];
				color0[1] = buffer[1];
				color0[2] = buffer[2];
				ChangeLEDState(false,LED2_mode);
				EEPROM.write(10, color0[0]);
				EEPROM.write(11, color0[1]);
				EEPROM.write(12, color0[2]);
			}
			
			break;
		case LEDState:
			if (!GetVerified(2))
				break;

			if (!buffer[0]) {
				LED1_active = buffer[1];
				EEPROM.write(1, LED1_active);
				ChangeLEDState(true, LED1_mode);

			}
			else {
				LED2_active = buffer[1];
				EEPROM.write(7, LED2_active);
				ChangeLEDState(false,LED2_mode);
			}break;

		case LED1Frequency:
			if (!GetVerified(1))
				break;
			LED1_freq = buffer[0];
			EEPROM.write(3, LED1_freq);
			break;
		case LED2Frequency:
			if (!GetVerified(1))
				break;
			LED2_freq = buffer[0];
			EEPROM.write(9, LED1_freq);
			break;
		case ClientLeft:
			DisplayIP();
			break;

		case ClientArrived:
			//DisplayColorStripe();
			break;
		default:
			
				break;
		}
	

	}

}

void 	DisplayIP() {
	lcd.clear();
	if (IPString.length() > 16) {
		lcd.print(IPString.substring(0,15));
		lcd.setCursor(0, 1);
		lcd.print(IPString.substring
			(16, IPString.length()));
	}
	else
	lcd.print(IPString);

}
//LED1/2_freq, LED1/2_active > LED1/2_mscounter
void Flash(bool LEDStripe)
{
	if (LED1_active && !LEDStripe) {
		LED1_mscounter++;
		if (LED1_mscounter < LED1_freq/2) {
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
	if (LED2_active && LEDStripe ) {
		LED2_mscounter++;
		if (LED2_mscounter < LED2_freq/2) {
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
void Smooth(bool LEDStripe )
{
	

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
		delay(LED1_freq);
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
		delay(LED2_freq);
	}
	
}


void DisplayColorStripe()
{

	lcd.clear();
	lcd.print(GetDisplayString()[0]);
	lcd.setCursor(0, 1);
	lcd.print(GetDisplayString()[1]);
}

void ChangeLEDState(bool One, uint8_t State) {
	if (One) {
		LED1_mode = State;
		EEPROM.write(2, State);
		
	}
	else {
		LED2_mode = State;
		EEPROM.write(8, State);
	}
	StateWriter(One, State);
	
}

void StateWriter(bool One, uint8_t State)
{
	switch (State)
	{
	case FixColorState:
		

		if (One&&LED1_active) {
			setColor(color0, 0);
		}
		else if (One&&!LED1_active) {
			setColor(new byte[3]{ 0,0,0 }, 0);
		}
		else if(!One&&LED1_active) {
			setColor(color1, 1);
		}
		else {
			setColor(new byte[3]{ 0,0,0 }, 1);
		}

		break;

	case FadeState:

		Fadeing(One);
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

String* GetDisplayString()
{



	String ret[2];
	if (LED1_active)
	{
		ret[0] = (String)LED1_mode + ": "+ (String)color0[0] + "/" + (String)color0[1] + "/" + (String)color0[2];
	}
	else
	{
		ret[0] = "------------";
	}

	if (LED2_active)
	{
		ret[1] = (String)LED2_mode + ": " + (String)color1[0] + "/" + (String)color1[1] + "/" + (String)color1[2];
	}
	else
	{
		ret[1] = "------------";
	}
	return ret;
}



void Fadeing(bool LEDStripe) //LED1_counter over 100 =  {X} over {color0[1,2,3]}
{
	if (LED1_active && !LEDStripe) {
		if (LED1_mscounter == LED1_freq) {
			if (fadeup)
				LED1_counter = LED1_counter + 5;
			else
				LED1_counter = LED1_counter - 5;
			byte currentcolor[] = { (color0[0] * LED1_counter / 100),(color0[1] * LED1_counter / 100) ,(color0[2] * LED1_counter / 100) };
			setColor(currentcolor, 0);

			if (LED1_counter == 100)
				fadeup = false;
			if (LED1_counter == 0)
				fadeup = true;
		}
		LED1_mscounter++;
	}

	if (LED2_active && LEDStripe) {
		if (LED2_mscounter == LED2_freq) {
			if (fadeup)
				LED2_counter = LED2_counter + 5;
			else
				LED2_counter = LED2_counter - 5;
			byte currentcolor[] = { ((color1[0] * LED2_counter) / 100),((color1[1] * LED2_counter) / 100) ,((color1[2] * LED2_counter) / 100) };
			setColor(currentcolor, 1);

			if (LED2_counter == 100)
				fadeup = false;
			if (LED2_counter == 0)
				fadeup = true;
		}
		LED2_mscounter++;
	}
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

	//	for (short i = 0; i < 3; i++)
	//	{
	//		/*Serial.print(i);
	//		Serial.print(" : ");
	//		Serial.println(((int)((float)255 * mlt[i])));
	//		Serial.println(mlt[i]);*/
	//		analogWrite(rgbPin[i], mlt[i]);
	//
	//	}
	//	delay(pulsedelay);
	//	Serial.println((String)(mlt[0] + ';' + mlt[1] + ';' + mlt[2]));

	if (!stripe)
	{
		analogWrite(R1, mlt[0]);
		analogWrite(G1, mlt[1]);
		analogWrite(B1, mlt[2]);


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

