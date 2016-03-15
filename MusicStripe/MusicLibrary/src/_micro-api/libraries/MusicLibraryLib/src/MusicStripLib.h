/*
 Name:		MusicLib.h
 Created:	10/02/2016 09:10:50
 Author:	Jakob D.

*/

#ifndef _MusicStripLib_h
#define _MusicStripLib_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

enum ControlByte : uint8_t {
	//WIFI
	Connect =1,
	Reconnect,
	Port,
	SSID,
	PWD,
	IP,
	WifiConnectSate,
	//ESP8266
	OTAUpdate,
	OTAStart,
	OTAEnd,
	OTAError,
	//General
	Start,
	Stop,
	GetLightState,
	RGBColor ,
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

#endif

