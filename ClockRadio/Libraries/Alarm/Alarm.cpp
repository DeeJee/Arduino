Alarm::Alarm(uint8_t day, uint8_t hour, uint8_t minute) {
	this->day = day;
	this->hour = hour;
	this->minute = minute;
	state = 0;
	bitWrite(state, CURRENT, !mode);
}

enum Mode {
	None,
	Off,
	Snooze,
	On,
};

Mode Alarm::checkAlarm(uint8_t day, uint8_t hour, uint8_t minute) {
	if (this->day == day && this->hour == hour && this->minute == minute) {
		if (mode == None) {
			mode = On;
		}
	}
	else
	{
		mode = None;
	}

	return mode;
}

void Alarm::switchOff() {
	mode = Off;
}

void Alarm::snooze() {
	mode = Snooze;
}


