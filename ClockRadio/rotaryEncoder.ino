//--------------------------------------------
// Example for a rotary encoder
// Bart Venneker 2015
//--------------------------------------------
//
// See http://youtu.be/KzT3aUE1-0Q for more info (in DUTCH!!)

#define ROTARYPA 2
#define ROTARYPB 4

void setupRotaryEncoder() {
	pinMode(ROTARYPA, INPUT);
	pinMode(ROTARYPB, INPUT);

	digitalWrite(ROTARYPA, HIGH);   // enable pull-up
	digitalWrite(ROTARYPB, HIGH);   // enable pull-up

	attachInterrupt(0, rotaryTurn, RISING);    // interrupt 0 is pin 2, For rotary encoder
}

void rotaryTurn()
{
	knobTurned = true;
	// Interrupt Service Routine for a change to Rotary Encoder pin A
	if (mode == ClockSetHoursMode) {
		hours = getNewValue(hours, 0, 23);
	}
	if (mode == ClockSetMinutesMode) {
		minutes = getNewValue(minutes, 0, 59);
	}
	if (mode == ClockSetDayMode) {
		day = getNewValue(day, 1, 7);
	}
	if (mode == AlarmSetHoursMode) {
		hours = getNewValue(hours, 0, 23);
	}
	if (mode == AlarmSetMinutesMode) {
		minutes = getNewValue(minutes, 0, 59);
	}
	if (mode == AlarmSetDayMode) {
		day = getNewValue(day, 1, 7);
	}
}

int getNewValue(int counter, int minValue, int maxValue) {
	if (digitalRead(ROTARYPB)) {
		if (counter > minValue) {
			counter--;   // Turn it Down!       
		}
		else {
			counter = maxValue;
		}
	}
	else {
		if (counter < maxValue) {
			counter++;   // Turn it Up!      
		}
		else {
			counter = minValue;
		}
	}
	return counter;
}

void rotaryTurnInLoop()
{
	// Interrupt Service Routine for a change to Rotary Encoder pin A
	//if (mode == ClockSetHours) {
	//	if (digitalRead(ROTARYPB)) {
	//		if (hours > 0) {
	//			hours--;   // Turn it Down!       
	//		}
	//		else {
	//			hours = 23;
	//		}
	//	}
	//	else {
	//		if (hours < 23) {
	//			hours++;   // Turn it Up!      
	//		}
	//		else {
	//			hours = 0;
	//		}
	//	}

	//}
	//if (mode == ClockSetMinutes) {
	//	if (digitalRead(ROTARYPB)) {
	//		if (minutes > 0) {
	//			minutes--;   // Turn it Down!       
	//		}
	//		else {
	//			minutes = 59;
	//		}
	//	}
	//	else {
	//		if (minutes < 59) {
	//			minutes++;   // Turn it Up!      
	//		}
	//		else {
	//			minutes = 0;
	//		}
	//	}
	//}
}




