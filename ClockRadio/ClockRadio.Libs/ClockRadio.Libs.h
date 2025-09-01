// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CLOCKRADIOLIBS_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CLOCKRADIOLIBS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CLOCKRADIOLIBS_EXPORTS
#define CLOCKRADIOLIBS_API __declspec(dllexport)
#else
#define CLOCKRADIOLIBS_API __declspec(dllimport)
#endif

// This class is exported from the ClockRadio.Libs.dll
class CLOCKRADIOLIBS_API CClockRadioLibs {
public:
	CClockRadioLibs(void);
	// TODO: add your methods here.
};

extern CLOCKRADIOLIBS_API int nClockRadioLibs;

CLOCKRADIOLIBS_API int fnClockRadioLibs(void);
