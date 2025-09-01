////***************************************//
//// --- WIDE.HK---//
//// --- Revised Date : 06/30/2013
//// --- I2C Arduino - Arduino UNO Demo ---//
//// --- SSD131x PMOLED Controller      ---//
//// --- SCL, SDA, GND, VCC(3.3v -5v)   ---//
////***************************************//
//
//#include <Wire.h>          // *** I2C Mode 
//#define OLED_Address 0x3c
//#define OLED_Command_Mode 0x80
//#define OLED_Data_Mode 0x40

#include <Alarm.h>
#include <Button.h>
#include <EEPROM.h>

#include <Fsm.h>
#include <DS3231.h>

//Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

#define dayPosition 0
#define spacePosition 2
#define hourPosition 3
#define semicolonPosition  5
#define minutePosition 6

#define eeprom_alarm1uren 0
#define eeprom_alarm1minuten 1
#define eeprom_alarm1dag 2

short previousMinutes;
const int setClockButtonPressed = 1, setAlarmButtonPressed = 2, rotaryButtonPressed = 3;

Button rotaryButton = Button(8, PULLUP);		//Knop voor het maken van instellingen
Button alarm1SetButton = Button(7, PULLDOWN);	//knop voor het instellen van alarm1
Button clockSetButton = Button(6, PULLDOWN);	//knop voor het instellen van de klok
Button alarmOffButton = Button(5, PULLDOWN);	//alarm uit
Button snoozeButton = Button(9, PULLDOWN);		//snooze knop

enum ClockMode {
	DisplayMode,
	ClockSetHoursMode,
	ClockSetMinutesMode,
	ClockSetDayMode,
	AlarmSetHoursMode,
	AlarmSetMinutesMode,
	AlarmSetDayMode
};
ClockMode mode;

//AlarmMode alarmMode;

State DisplayClock(enterDisplayClock, timeKeeping, NULL);
State ClockSetHours(enterClockSetHours, clockEditHours, NULL);
State ClockSetMinutes(enterClockSetMinutes, clockEditMinutes, writeTimeToClock);
State ClockSetDay(enterClockSetDay, clockSetDay, writeDayToClock);
State AlarmSetHours(getAlarmFromEeprom, clockEditHours, NULL);
State AlarmSetMinutes(enterAlarmSetMinutes, clockEditMinutes, NULL);
State AlarmSetDay(enterAlarmSetDay, clockSetDay, storeAlarm1);
State AlarmSounding(enterAlarm, soundAlarm, exitAlarm);
Alarm alarm1;

void enterAlarm() {
	Serial.println("AlarmAlarmAlarmAlarm");
}

void soundAlarm() {
	showTime();
	blinkText(10, 0, "ALARM", "     ");
	if (alarmOffButton.uniquePress()) {
		//alarmMode = Off;
		Serial.println("alarm off");
		alarm1.switchOff();
		showText("     ", 10, 0);
	}

	if (snoozeButton.uniquePress()) {
		alarm1.snooze();
		Serial.println("snoozing");
		showText("     ", 10, 0);
	}
}

void exitAlarm() {

}

Fsm sm(&DisplayClock);

void setup() {
	Serial.begin(9600);
	rtc.begin();
	setupOled();
	setupRotaryEncoder();

	//enter timesetmode
	sm.add_transition(&DisplayClock, &ClockSetHours, setClockButtonPressed, &initSetTime);
	//sm.add_transition(&ClockSetHours, &DisplayClock, setClockButtonPressed, &initSetTime);	//
	//12234
	//cycle timesetmode
	sm.add_transition(&ClockSetHours, &ClockSetMinutes, rotaryButtonPressed, forceDisplayTime);
	sm.add_transition(&ClockSetMinutes, &ClockSetDay, rotaryButtonPressed, forceDisplayTime);
	sm.add_transition(&ClockSetDay, &DisplayClock, rotaryButtonPressed, forceDisplayTime);

	//enter alarm1setmode
	sm.add_transition(&DisplayClock, &AlarmSetHours, setAlarmButtonPressed, showAlarm);

	//cycle alarm1 setmode
	sm.add_transition(&AlarmSetHours, &AlarmSetMinutes, rotaryButtonPressed, showAlarm);
	sm.add_transition(&AlarmSetMinutes, &AlarmSetDay, rotaryButtonPressed, showAlarm);
	sm.add_transition(&AlarmSetDay, &DisplayClock, rotaryButtonPressed, forceDisplayTime);

	sm.add_transition(&DisplayClock, &AlarmSounding, 4, NULL);
	sm.add_transition(&AlarmSounding, &DisplayClock, 5, NULL);
	alarm1 =Alarm(EEPROM.read(eeprom_alarm1dag),
		EEPROM.read(eeprom_alarm1minuten),
		EEPROM.read(eeprom_alarm1uren));
}

void forceDisplayTime() {
	previousMinutes = -1;  //forceren dat de tijd direct wordt getoond
	showTime();
}

boolean tonen;
int hours, minutes;
short day;
boolean knobTurned;

void enterAlarmSetDay() {
	mode = AlarmSetDayMode;
}

int previousMin;
void timeKeeping() {
	showTime();
	Time time = rtc.getTime();
	if (time.min != previousMin) {
		/*Serial.print("Current time ");
		Serial.print("day: ");
		Serial.println(time.dow);
		Serial.print("hour: ");
		Serial.println(time.hour);
		Serial.print("min: ");
		Serial.println(time.min);
		Serial.print("Alarm time ");
		Serial.print("day: ");
		Serial.println(alarm1.getDay());
		Serial.print("hour: ");
		Serial.println(alarm1.getHour());
		Serial.print("min: ");
		Serial.println(alarm1.getMinute());*/
		alarm1.check(time.dow, time.hour, time.min);
		previousMin = time.min;
	}
}

void enterDisplayClock() {
	Serial.println("DisplayClock");
	showTime();
	mode = DisplayMode;
}

void enterClockSetDay() {
	Serial.println("ClockSetDay");
	mode = ClockSetDayMode;
}

void enterClockSetHours() {
	Serial.println("ClockSetHours");
	mode = ClockSetHoursMode;
}

void enterClockSetMinutes() {
	Serial.println("ClockSetMinutes");
	mode = ClockSetMinutesMode;
}

void enterAlarmSetMinutes() {
	Serial.println("AlarmSetMinutes");
	mode = AlarmSetMinutesMode;
}

void getAlarmFromEeprom() {
	Serial.println("getting alarm from eeprom");
	day = EEPROM.read(eeprom_alarm1dag);
	hours = EEPROM.read(eeprom_alarm1uren);
	minutes = EEPROM.read(eeprom_alarm1minuten);
	Serial.print("read ");
	Serial.print(weekDay(day));
	Serial.print(" ");
	Serial.print(hours);
	Serial.print(":");
	Serial.println(minutes);

	displayTimeString(day, hours, minutes);
	mode = AlarmSetHoursMode;
}

void storeAlarm1() {
	EEPROM.update(eeprom_alarm1dag, day);
	EEPROM.update(eeprom_alarm1uren, hours);
	EEPROM.update(eeprom_alarm1minuten, minutes);
	alarm1.setDay(day);
	alarm1.setHour(hours);
	alarm1.setMinute(minutes);
	Serial.print("alarm ");
	Serial.print(hours);
	Serial.print(":");
	Serial.print(minutes);
}

void loop() {
	alarmOffButton.tick();
	if (alarmOffButton.wasLongPressed()) {
		Serial.println("longpress");
	}


	if (clockSetButton.uniquePress())
	{
		sm.trigger(setClockButtonPressed);
	}

	if (alarm1SetButton.uniquePress()) {
		sm.trigger(setAlarmButtonPressed);
	}

	if (rotaryButton.uniquePress()) {
		sm.trigger(rotaryButtonPressed);
	}

	/*boolean sounding = alarm1.isSounding();
	Serial.print("sounding: ");
	Serial.println(sounding);*/

	if (alarm1.isSounding()) {
		sm.trigger(4);
	}
	else {
		sm.trigger(5);
	}

	sm.run_machine();


	/*if (timeelapsed == 10) {
		doeIets();
	}

	if (timeelapsed == 10) {
		doeNogIets();
	}*/
}