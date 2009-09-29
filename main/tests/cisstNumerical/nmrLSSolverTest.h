/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrLSSolverTest.h 75 2009-02-24 16:47:20Z adeguet1 $
  
  Author(s):  Ankur Kapoor
  Created on: 2005-07-28
  
  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _nmrLSSolverTest_h
#define _nmrLSSolverTest_h

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstNumerical/nmrLSSolver.h>

class nmrLSSolverTest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(nmrLSSolverTest);
    CPPUNIT_TEST(TestSolveBookExample);
    CPPUNIT_TEST_SUITE_END();

protected:
    vctDynamicMatrix<double> InputMatrix;  // A
    vctDynamicMatrix<double> InputMatrixRS; // B
    vctDynamicMatrix<double> OutputX; // X = A^+ * B;

public:

    void setUp() {
        InputMatrix.SetSize(15, 15, VCT_COL_MAJOR);
	InputMatrix.Assign(-120.2457115,50.77407853,57.78573463,94.08899407,56.89606457,147.2479934,93.1217515,49.77696642,153.5152266,-7.866191936,-38.97995485,55.11847118,-212.0426688,-7.832119627,-80.76491309,
		-1.978955777,169.242987,4.031403162,-99.20917355,-82.17142917,5.574383184,1.124489638,148.8490471,-60.64828593,-68.165686,-138.1265624,-109.9840455,-64.46789155,88.91726184,68.04385837,
		-15.67172988,59.12825869,67.70891876,21.20351522,-26.56068513,-121.7317454,-64.51458157,-54.64758948,-134.7362674,-102.4553057,31.55426328,8.599059329,-70.43017284,230.9287486,-236.4589848,
		-160.4085562,-64.35952027,56.89002052,23.78820729,-118.7777016,-4.122713369,80.57287931,-84.67581639,46.93831199,-123.4353478,155.3242569,-200.4563322,-101.8137216,52.46386798,99.0114872,
		25.73042347,38.03372517,-25.56454156,-100.7763392,-220.2320717,-112.8343864,23.16260108,-24.63365281,-90.35669426,28.88070187,70.78938846,-49.30879177,-18.20818684,-1.178732395,21.88991209,
		-105.6472928,-100.9115524,-37.74689555,-74.20447521,98.6337391,-134.9277543,-98.97596717,66.30241459,3.587963873,-42.93030046,195.7384755,46.20480118,152.1013239,91.31408178,26.16624602,
		141.5141486,-1.951066953,-29.588711,108.2294953,-51.86350663,-26.11016231,133.9585701,-85.41973745,-62.753122,5.580119018,50.45423536,-32.10046922,-3.843876389,5.594067889,121.3444495,
		-80.50904042,-4.822078915,-147.5134506,-13.14997029,32.73675641,95.34654455,28.95020345,-120.1314815,53.53979542,-36.78735667,186.452902,123.6555652,122.7447989,-110.7069895,-27.46669865,
		52.8743011,0.004319184163,-23.40040477,38.98804897,23.40570128,12.864443,147.8917058,-11.98694281,55.28835174,-46.49733672,-33.98117774,-63.12796567,-69.62048,48.54977073,-13.31344508,
		21.93206727,-31.78594512,11.84448371,8.798710658,2.146613888,65.64675139,113.8028013,-6.529401484,-20.36904796,37.09605838,-113.9779402,-232.5211129,0.7524486523,-0.5005073756,-127.0500204,
		-92.19016244,109.5003739,31.48090434,-63.54652255,-100.3944467,-116.7819365,-68.41385851,48.52955559,-205.4324681,72.82829316,-21.11234834,-123.1636533,-78.28930444,-27.62178594,-166.3606453,
		-217.0674494,-187.3990258,144.3508244,-55.95733022,-94.71460647,-46.06051795,-129.1936045,-59.54909026,13.25607314,211.216017,119.0244936,105.5648388,58.69385592,127.6452474,-70.35542615,
		-5.918782452,42.8183273,-35.09747383,44.36534895,-37.4429195,-26.24399528,-7.292627626,-14.96677438,159.2940704,-135.7297743,-111.6208758,-11.32239894,-25.12073746,186.3400613,28.08804885,
		-101.0633706,89.56384712,62.32338511,-94.99037985,-118.5886214,-121.3152068,-33.05988799,-43.47519312,101.8411789,-102.2610144,63.52741347,37.92236227,48.01358228,-52.25593016,-54.12093299,
		61.44630489,73.09573384,79.90486181,78.11816179,-105.5902924,-131.9436998,-84.36276392,-7.933022302,-158.0402499,103.7834199,-60.14121263,94.41997267,66.81550344,10.34244469,-133.353073);
	InputMatrixRS.SetSize(15, 3, VCT_COL_MAJOR);
	InputMatrixRS.Assign(107.2686268,-40.16667346,213.6308423,
			-71.20854525,17.36656686,-25.76171157,
			-1.128556123,-11.61184934,-140.9528489,
			-0.08170291957,106.4119149,177.0100893,
			-24.94362847,-24.53862968,32.55459848,
			39.65753187,-151.7539131,-111.9039575,
			-26.40133549,0.9734159126,62.03501394,
			-166.4010877,7.137286486,126.9781847,
			-102.89751,31.65358138,-89.60425064,
			24.30947002,49.98256678,13.51754448,
			-125.6590108,127.8084147,-13.904001,
			-34.71831897,-54.78161469,-116.3395294,
			-94.13721934,26.08083989,118.371954,
			-117.4560281,-1.317667187,-1.542966178,
			-102.1141687,-58.02640021,53.62186947);
	OutputX.SetSize(15, 3, VCT_COL_MAJOR);
	OutputX.SetSize(15, 3, VCT_COL_MAJOR);
	OutputX.Assign(3.808716234,-1.604358382,1.314185217,
			1.765037779,-0.671519301,1.550500459,
			1.097770212,-0.4733820163,-0.5192112036,
			0.6452779718,-0.4326171892,2.228564839,
			-0.1777115626,0.4930160896,-1.376984204,
			1.860559974,-0.7484879289,1.335094676,
			-3.247995013,0.7506716309,-2.133893323,
			1.490777879,-1.47847596,1.264077429,
			2.820173362,-1.222342564,1.958289086,
			1.725528877,-0.6506495495,0.9790712927,
			3.994729991,-1.886913892,2.29991698,
			-2.427902554,0.4676176617,-1.62756148,
			-0.5392562507,-0.4029136844,0.1392248457,
			0.6442630721,-0.5196637535,0.04627086023,
			-0.95034165,0.6361314184,-0.5833346816);
    }
    
    void tearDown()
    {}
    
    /*! Test based on the example provided with the Lawson and Hanson book */
    void TestSolveBookExample(void);
};


#endif // _nmrLSSolverTest_h

