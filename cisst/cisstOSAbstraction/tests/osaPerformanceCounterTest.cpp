#include "osaPerformanceCounterTest.h"
#include <cisstOSAbstraction/osaPerformanceCounter.h>
#include <unistd.h>
void osaPerformanceCounterTest::TestTimer(void)
{

	osaPerformanceCounter pf;
	pf.Start();
	pf.delay(2.00000000); //sleep for 2 seconds
	pf.Stop();
	osaTimeData diff = pf.GetElapsedTime();

	osaTimeData acceptableRangeUpper(2,005000000,true);
	osaTimeData acceptableRangeLower(1,995000000,true);
	CPPUNIT_ASSERT(diff<=acceptableRangeUpper && diff >= acceptableRangeLower);


}
CPPUNIT_TEST_SUITE_REGISTRATION(osaPerformanceCounterTest);
