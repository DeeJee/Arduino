// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ALARM_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ALARM_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef ALARM_EXPORTS
#define ALARM_API __declspec(dllexport)
#else
#define ALARM_API __declspec(dllimport)
#endif

// This class is exported from the Alarm.dll
class ALARM_API CAlarm {
public:
	CAlarm(void);
	double SquareRoot(double number);
};

extern ALARM_API int nAlarm;

ALARM_API int fnAlarm(void);
