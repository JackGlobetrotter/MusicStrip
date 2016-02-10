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
	RGBColor = 1,
	MusicTitle,
	
	SwitchStade,
	Start,
	Stop,
	LightToggle,
	IP,
	Port,
	SSID,
	PWD,
	Connect,
	Disconnect,
	MusicState,
	MicroState,
	FadeState,
	SmoothState,
	FlashState,
	FixColorState,
	WifiConnectSate

};

#endif

