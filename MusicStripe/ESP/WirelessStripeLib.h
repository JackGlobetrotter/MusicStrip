// WirelessStripeLib.h

#ifndef _WIRELESSSTRIPELIB_h
#define _WIRELESSSTRIPELIB_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

enum ControlByte : uint8_t {
	//WIFI

	Port,
	SSID,
	PWD,
	IP,
	//ESP8266
	OTAUpdate,
	OTAStart,
	OTAEnd,
	OTAError,
	//General
	Start,
	Stop,
	GetLightState,
	RGBColor,
	MusicTitle,
	//LED
	LED1SwitchStade,
	LED2SwitchStade,
	LED1Data,
	LED2Data,
	LEDState,

	LightToggle,
	MusicState,
	MicroState,
	FadeState,
	SmoothState,
	FlashState,
	FixColorState,

	LED1Frequency,
	LED2Frequency,
	ClientLeft,
	ClientArrived,
	SaveStartupCFG
};

class WirelessStripeLibClass
{
 protected:


 public:
	void init();
};

extern WirelessStripeLibClass WirelessStripeLib;

#endif

