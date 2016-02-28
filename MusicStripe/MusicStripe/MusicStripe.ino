


//#include <IRremoteInt.h>
//#include <IRremote.h>
#include <LiquidCrystal.h>
#include <..\MusicLibrary\src\_micro-api\libraries\MusicLibraryLib\src\MusicStripLib.h>
#include <EEPROM\EEPROM.h>

//get working 2 led stripes controled seperate
//fix fading, flash

#define WIFIPointer 0

#define SIZE    255
#define DELAY    20
#define HUE_MAX  6.0
#define HUE_DELTA 0.01

//----pinout def--------

#define LightPin 4

//-----display

#define DR4 A0
#define DR5 A1
#define DR6 A2
#define DR7 A3

#define Enable A4
#define RS  A5

// ----------IR

#define IRPin 2


//----------LED1
#define R1 3
#define G1 5
#define B1 6

//---------LED 2

#define R2 7
#define G2 7
#define B2 8


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
String _pwd = "test";
uint8_t _port = 80;
bool WifiIsConnected = false;

bool LightStat = true;
int EEPROM_Pointer = 0;
void setup()
{
	loadData();
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


	//attachInterrupt(0, CHECK_IR, CHANGE);
	//irrecv.enableIRIn();

	lcd.begin(16, 2);
	Serial.begin(115200);
	lcd.print("Welcome");
	lcd.print(".");
	delay(500);
	lcd.print(".");
	Serial.flush();
	delay(500);
	lcd.print(".");

	//Display_scroll = true;

	LED1_active = true;
	connectWifi(_ssid,_pwd,_port);
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

	// WIFI-DATA

	uint8_t go = EEPROM.read(EEPROM_Pointer);
	EEPROM_Pointer++;
		byte temp[go];
	for (int i = 0; i < go; i++)
	{
		go[i] = EEPROM.read(EEPROM_Pointer);
		EEPROM_Pointer++;
	}
	if (go != 0)
		_ssid = String(go);

	uint8_t go = EEPROM.read(EEPROM_Pointer);
	EEPROM_Pointer++;
	byte temp[go];
	for (int i = 0; i < go; i++)
	{
		go[i] = EEPROM.read(EEPROM_Pointer);
		EEPROM_Pointer++;
	}
	if (go != 0)
		_pwd = String(go);

	go = EEPROM.read(EEPROM_Pointer);
	if (go != 0)
		_port = go;
	EEPROM_Pointer++;
}

void storeData_WIFI()
{
	EEPROM_Pointer = 13;
	EEPROM.write(EEPROM_Pointer
	, _ssid.length());
	EEPROM_Pointer ++;

	for (uint8_t i = 0; i < _ssid.length(); i++)
	{		
		EEPROM.write(EEPROM_Pointer, _ssid.charAt(i));
		EEPROM_Pointer++;
	}
	EEPROM.write(EEPROM_Pointer
		, _pwd.length());
	EEPROM_Pointer++;

	for (uint8_t i = 0; i < _pwd.length(); i++)
	{
		EEPROM.write(EEPROM_Pointer, _pwd.charAt(i));
		EEPROM_Pointer++;
	}
	EEPROM.write(EEPROM_Pointer
		, _port);

}

void ToggleLightSwitch(bool OnOff)
{
	LightStat = OnOff;
	digitalWrite(LightPin, !LightStat);


}

bool connectWifi(String ssid, String password, uint8_t port) {

	lcd.clear();
	lcd.print("Connecting to");
	lcd.setCursor(0, 1);
	lcd.print(ssid);
	lcd.setCursor((uint8_t)ssid.length(), 1);

	int ProgressInt = ssid.length();

	Serial.write(ControlByte::Port);
	Serial.write(port);
	while (Serial.available() <= 0)
		delay(100);
	if (Serial.read() != 1)
		return "";
	else
		Serial.flush();
	delay(100);

	lcd.print(".");

	Serial.write(ControlByte::SSID);

	Serial.print(ssid);
	while (Serial.available() <= 0)
		delay(100);
	// Console.WriteLine(myPort.ReadLine());
	if (Serial.read() != 1)
		return "";
	else
		Serial.flush();
	delay(100);

	lcd.print(".");

	Serial.write(ControlByte::PWD);

	Serial.print(password);
	while (Serial.available() <= 0)
		delay(100);
	if (Serial.read() != 1)
		return "";
	else
		Serial.flush();
	delay(100);

	lcd.print(".");
	Serial.write(ControlByte::Connect);

	while (Serial.available() <= 5)
	{
		lcd.print(".");
		delay(100);
	}
	String IP = Serial.readString();;
	if (IP != "")
		WifiIsConnected = true;

	lcd.clear();
	lcd.print("Connected to:");
	lcd.setCursor(0, 1);
	lcd.print(IP);
	return WifiIsConnected;
}
void CHECK_IR() {
	//while (irrecv.decode(&results)) {
	//	LED1_mscounter = 0;


	//	switch (results.value)
	//	{
	//	case 16195807: //RED
	//		if (LED1_active) {
	//			color0[0] = 255;
	//			color0[1] = 0;
	//			color0[2] = 0;
	//		}
	//		if (LED2_active) {
	//			color1[0] = 255;
	//			color1[1] = 0;
	//			color1[2] = 0;
	//		}
	//		break;
	//	case 16228447:  //GREEN
	//		if (LED1_active) {
	//			color0[0] = 0;
	//			color0[1] = 255;
	//			color0[2] = 0;
	//		}
	//		if (LED2_active) {
	//			color1[0] = 0;
	//			color1[1] = 255;
	//			color1[2] = 0;
	//		}
	//		break;
	//	case 16212127:  //BLUE
	//		if (LED1_active) {
	//			color0[0] = 0;
	//			color0[1] = 0;
	//			color0[2] = 255;
	//		}
	//		if (LED2_active) {
	//			color1[0] = 0;
	//			color1[1] = 0;
	//			color1[2] = 255;
	//		}
	//		break;
	//	case 16244767:  //WHITE
	//		if (LED1_active) {
	//			color0[0] = 255;
	//			color0[1] = 255;
	//			color0[2] = 255;
	//		}
	//		if (LED2_active) {
	//			color1[0] = 255;
	//			color1[1] = 255;
	//			color1[2] = 255;
	//		}
	//		break;
	//	case 16240687: //FLASH
	//		LED1_mode = FlashState;
	//		break;
	//	case 16248847: //STROBE
	//		LED1_mode = FixColorState;
	//		break;
	//	case 16238647: //FADE
	//		LED1_mode = FadeState;
	//		break;
	//	case 16246807: //SMOOTH
	//		LED1_mode = SmoothState;
	//		//	fadeup = true;
	//		break;

	//	}


	//	//		delay(100);

	//	irrecv.resume(); // Receive the next value
	//}
}

void ScrollDisplay() {
	
	if (Display_mscounter == Display_freq) {
	//	Serial.println(Display_counter >= Display_max * 2);
		if (Display_counter >= Display_max+16 )
		{

			for (int i = 0; i < Display_max +16; i++)
			{
				lcd.scrollDisplayRight();
			}
			Display_counter = 0;
		}
		else
		{
			//Serial.println(Display_counter);
			lcd.scrollDisplayLeft();
			Display_counter++;
		}
		Display_mscounter = 0;
	}
	else {
		delay(10);
		Display_mscounter++;
	}


}

void loop()
{
	uint8_t ctrl = 0;

	if (Serial.available() > 0) {
	
	
		ctrl = (uint8_t)Serial.read();
		//if (bitRead(ctrl, 0) {

		switch (ctrl) {
		case LED1SwitchStade:
			if (Serial.available() <= 0)
				delay(50);
		
			LED1ChanceState(Serial.read());

			break;		
		case LED2SwitchStade:
				if (Serial.available() <= 0)
					delay(50);

				LED2ChanceState(Serial.read());

				break;
		case LightToggle:
			if (Serial.available() <= 0)
				delay(50);
			uint8_t temp = Serial.read();
			EEPROM.write(0, temp);
			ToggleLightSwitch((bool)temp);

			//	}
			break;
		case GetLightState:

			Serial.write((uint8_t)LightStat);

			break;

		case LED1Data:

			if (Serial.available() <= 0)
				delay(50);
			Serial.readBytes(color0, 3); 
			LED1ChanceState(LED1_mode);
			EEPROM.write(4, color0[0] );
			EEPROM.write(5, color0[1]);
			EEPROM.write(6, color0[2]);
			break;
		case LED2Data:
			if (Serial.available() <= 0)
				delay(50);
			Serial.readBytes(color1, 3); LED2ChanceState(LED2_mode);
			EEPROM.write(10, color0[0]);
			EEPROM.write(11, color0[1]);
			EEPROM.write(12, color0[2]);
			break;
		case LEDState:
			if (Serial.available() <= 0)
				delay(50);
			if (!(bool)Serial.read()) {
				LED1_active = (bool)Serial.read();
				EEPROM.write(1, LED1_active);
				LED1ChanceState(LED1_mode);
			}
			else {
				LED2_active = (bool)Serial.read();
				EEPROM.write(7, LED2_active);
				LED2ChanceState(LED2_mode);
			}
	
		case LED1Frequency:
			LED1_freq = Serial.read();
			EEPROM.write(3, LED1_freq);
			break;
		case LED2Frequency:
			LED2_freq = Serial.read();
			EEPROM.write(9, LED2_freq);
			break;
		case SSID:
			_ssid = Serial.readString();
			storeData_WIFI();
			break;

		case PWD:
			_pwd = Serial.readString();
			storeData_WIFI();
			break;

		case Port:
			_port = Serial.read();
			storeData_WIFI();
			break;
		}
	

	}
	if (LED1_active) {
		switch (LED1_mode) {
		case FlashState:
			break;

		case FadeState:
			Fadeing(false);
	
			break;

		case SmoothState:
			Smooth(false);
			break;
		}
	}
	if (LED2_active)
	{
		switch (LED2_mode) {
		case FlashState:
			break;
		case FadeState:
			Fadeing(true);
			
			break;

		case SmoothState:
			Smooth(true);
			break;
		}
		
	}
	if (Display_scroll)
	{
		ScrollDisplay();
	}
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
		LED1Smooth[0] = (rgbval & 0x00FF0000) >> 16; // there must be better ways
		LED1Smooth[1] = (rgbval & 0x0000FF00) >> 8;
		LED1Smooth[2] = rgbval & 0x000000FF;

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
		LED2rgbval[0] = (rgbval & 0x00FF0000) >> 16; // there must be better ways
		LED2rgbval[1] = (rgbval & 0x0000FF00) >> 8;
		LED2Smooth[2] = rgbval & 0x000000FF;
		analogWrite(R2, LED2rgbval[0] * LED2bright[0] / 256);
		analogWrite(G2, LED2rgbval[1] * LED2bright[1] / 256);
		analogWrite(B2, LED2rgbval[2] * LED2bright[2] / 256);
		delay(LED2_freq);
	}
	
}


void DisplayColorStripe()
{

	lcd.print("Stripe 1:");
	if (LED1_active)
	{
		lcd.print((String)color0[0] + "/" + (String)color0[1] + "/" + (String)color0[2]);
		Display_max = ((String)color0[0] + "/" + (String)color0[1] + "/" + (String)color0[2]).length() + 9;
		//Serial.println(Display_max);
	}
	else {
		lcd.print("---");
	}
	lcd.setCursor(0, 1);
	lcd.print("Stripe 2:");


	if (LED2_active)
	{
		lcd.print((String)color1[0] + "/" + (String)color1[1] + "/" + (String)color1[2]);
		if (((String)color1[0] + "/" + (String)color1[1] + "/" + (String)color1[2]).length() + 9> Display_max)
			Display_max = ((String)color1[0] + "/" + (String)color1[1] + "/" + (String)color1[2]).length() + 9;

	}
	else {
		lcd.print("---");
	}
	//Serial.println(Display_max);
	if (Display_max > 16)
		Display_scroll = true;
	else
		Display_scroll = false;
	if(Display_scroll)
	for (size_t i = 0; i < 16; i++)
	{
		lcd.scrollDisplayRight();
	}

}
void LED1ChanceState(uint8_t State)
{
	lcd.clear();
	LED1_mode = State;
	Display_max = 0;
	Display_counter = 0;
	Display_mscounter = 0;
	EEPROM.write(2, State);

	switch (State)
	{
	case FixColorState:


		if (LED1_active) {
			setColor(color0, 0);
		}
		else {
			setColor(new byte[3]{ 0,0,0 }, 0);
		}
	
		Display_freq = 70;
		DisplayColorStripe();

		break;
	case WifiConnectSate:
		connectWifi(_ssid, _pwd, _port);
		break;
	case FadeState:
		Display_freq = 70;
		DisplayColorStripe();
		Fadeing();
		break;
	case FlashState:
		Display_freq = 70;
		DisplayColorStripe();
		LED1_mscounter = 0;
		// = 0;
		fadeup = true;

		break;
	case SmoothState:
		Display_freq = 70;
		LED1_freq = 10;
		DisplayColorStripe();
		break;

	}

}

void LED2ChanceState(uint8_t State)
{
	lcd.clear();
	LED1_mode = State;
	Display_max = 0;
	Display_counter = 0;
	Display_mscounter = 0;
	EEPROM.write(8, State);

	switch (State)
	{
	case FixColorState:


		if (LED2_active) {
			setColor(color1, 0);
		}
		else {
			setColor(new byte[3]{ 0,0,0 }, 0);
		}

		Display_freq = 70;
		DisplayColorStripe();

		break;
	case WifiConnectSate:
		connectWifi(_ssid, _pwd, _port);
		break;
	case FadeState:
		Display_freq = 70;
		DisplayColorStripe();
		Fadeing();
		break;
	case FlashState:
		Display_freq = 70;
		DisplayColorStripe();
		LED2_mscounter = 0;
		// = 0;
		fadeup = true;

		break;
	case SmoothState:
		Display_freq = 70;
		LED2_freq = 10;
		DisplayColorStripe();
		break;

	}

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

