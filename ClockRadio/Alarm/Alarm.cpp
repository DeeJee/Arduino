// Alarm.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Alarm.h"


// This is an example of an exported variable
ALARM_API int nAlarm=0;

// This is an example of an exported function.
ALARM_API int fnAlarm(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
// see Alarm.h for the class definition
CAlarm::CAlarm()
{
    return;
}

// Find the square root of a number.
double CAlarm::SquareRoot(double v)
{
	return 0.0;
}
