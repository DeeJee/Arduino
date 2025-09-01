#ifndef Alarm_H
#define Alarm_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class Alarm {
public:
	Alarm(uint8_t day, uint8_t hour, uint8_t minute);
	int check();
	void switchOff();
	void snooze();
private:
	Mode mode;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
};

#endif