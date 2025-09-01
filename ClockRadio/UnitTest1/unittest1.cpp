#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\Alarm\Alarm.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest1
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			CAlarm alarm = CAlarm();
			double aap = alarm.SquareRoot(9);
		}

	};
}