/*
Example sketch to display Strings and float values on the OLED
1602 display  from Wide.HK. This uses a Lbrary that I've
put together containing some basic functions.
The I2C Address is set to 0x3C in OLedI2C.cpp
Phil Grant 2013 www.gadjet.co.uk  07/11/13
*/

#include "Wire.h"
#include "OLedI2C.h"
OLedI2C OLED;

void setupOled() {

}

//int i = 0;
//void loop()
//{
//	while (i<100) {
//
//
//		// ********************************************************************//
//		// **** Show Data Value *** //
//		sendCommand(0x01);	// **** Clear display
//		send_string(String(i).c_str());
//		delay(10);
//		i++;
//	}
//	// **** Show Data Value *** //
//	// ********************************************************************//
//
//}

