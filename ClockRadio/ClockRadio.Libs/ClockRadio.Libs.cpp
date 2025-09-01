// ClockRadio.Libs.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ClockRadio.Libs.h"


// This is an example of an exported variable
CLOCKRADIOLIBS_API int nClockRadioLibs=0;

// This is an example of an exported function.
CLOCKRADIOLIBS_API int fnClockRadioLibs(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
// see ClockRadio.Libs.h for the class definition
CClockRadioLibs::CClockRadioLibs()
{
    return;
}
