
void initSetTime() {
	Time time = rtc.getTime();
	day = time.dow;
	hours = time.hour;
	minutes = time.min;
}

void writeTimeToClock() {
	rtc.setTime(hours, minutes, 0);
	previousMinutes = -1;//forceren dat de tijd direct wordt getoond
}