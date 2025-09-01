/*
Example sketch to display Strings and float values on the OLED
1602 display  from Wide.HK. This uses a Lbrary that I've
put together containing some basic functions.
The I2C Address is set to 0x3C in OLedI2C.cpp
Phil Grant 2013 www.gadjet.co.uk  07/11/13
*/

#include "Wire.h"
#include "OLedI2C.h"

#define blinkInterval 500  //interval waarmee tekst knippert in ms


OLedI2C OLED;
void setupOled() {
	OLED.init();
}

void clockSetDay() {
	blinkText(dayPosition, 0, weekDay(day), "  ");
}
void writeDayToClock() {
	rtc.setDOW(day);
}

void clockEditHours() {
	blinkText(hourPosition, 0, Format(hours), "  ");
}

void clockEditMinutes() {
	blinkText(minutePosition, 0, Format(minutes), "  ");
}

String Format(int number) {
	if (number < 10)
		return "0" + String(number);
	else {
		return String(number);
	}
}


void showText(const char *String, int column, int row) {
	OLED.sendString(String, column, row);
}

void  showTime() {

	Time time = rtc.getTime();

	if (time.min != previousMinutes) {
		previousMinutes = time.min;

		displayTimeString(time.dow, time.hour, time.min);
	}
}

void  showAlarm() {
	displayTimeString(day, hours, minutes);
}

void displayTimeString(uint8_t day, uint8_t hours, uint8_t minutes) {
	OLED.sendString(weekDay(day).c_str(), dayPosition, 0);
	OLED.sendString(" ", spacePosition, 0);
	OLED.sendString(Format(hours).c_str(), hourPosition, 0);
	OLED.sendString(":", semicolonPosition, 0);
	OLED.sendString(Format(minutes).c_str(), minutePosition, 0);
}


short previousDay;
void showDay() {
	if (day != previousDay) {
		previousDay = day;
		switch (day)
		{
		case 0:
			OLED.sendString("Un", 10, 0);
			break;
		case 1:
			OLED.sendString("Ma", 10, 0);
			break;
		case 2:
			OLED.sendString("Di", 10, 0);
			break;
		case 3:
			OLED.sendString("Wo", 10, 0);
			break;
		case 4:
			OLED.sendString("Do", 10, 0);
			break;
		case 5:
			OLED.sendString("Vr", 10, 0);
			break;
		case 6:
			OLED.sendString("Za", 10, 0);
			break;
		case 7:
			OLED.sendString("Zo", 10, 0);
			break;
		}
	}
}

String weekDay(int day) {
	switch (day)
	{
	case 0:
		return "Un";
		break;
	case 1:
		return "Ma";
		break;
	case 2:
		return "Di";
		break;
	case 3:
		return "Wo";
		break;
	case 4:
		return "Do";
		break;
	case 5:
		return "Vr";
		break;
	case 6:
		return "Za";
		break;
	case 7:
		return "Zo";
		break;
	}
}


//todo: dit aanpassen. Wanneer de clock 24/7 draait gaat dit mis.
unsigned long previousMillis;

void blinkText(int column, int row, String text, char  alternateText[]) {
	unsigned long currentMillis = millis();
	if (currentMillis - previousMillis >= blinkInterval || knobTurned) {
		//save the last time you blinked the LED
		previousMillis = currentMillis;

		//lcd.setCursor(column, row);
		if (tonen) {
			//lcd.write(text.c_str());
			OLED.sendString(text.c_str(), column, row);
			tonen = false;
		}
		else {
			//lcd.write(alternateText);
			OLED.sendString(alternateText, column, row);
			tonen = true;
		}

		if (knobTurned) {
			knobTurned = false;
			tonen = true;
		}
	}
}