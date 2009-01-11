/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrLSqLinTest.cpp,v 1.10 2007/04/26 20:12:05 anton Exp $
  
  Author(s):  Ankur Kapoor
  Created on: 2005-11-04
  
  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "nmrLSqLinTest.h"

#include <cisstVector/vctRandomDynamicMatrix.h>
#include <cisstNumerical/nmrIsOrthonormal.h>
#include <cisstNumerical/nmrLSqLin.h>


const int numberThread = 25;
const int numberTimes = 100;


void nmrLSqLinTest::TestDynamicLS(void) {
    nmrLSqLinSolutionDynamic solution(InputA);
    nmrLSqLin(InputA, Inputb, solution);
    double error = (solution.GetX() - OutputXLS).LinfNorm();
    CPPUNIT_ASSERT(error < Inputb.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestDynamicLSUserAlloc(void) {
    vctDynamicVector<double> output(15);
    nmrLSqLin(InputA, Inputb, output);
    double error = (output - OutputXLS).LinfNorm();
    CPPUNIT_ASSERT(error < Inputb.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestDynamicLSI(void) {
    nmrLSqLinSolutionDynamic solution(InputA, InputG);
    nmrLSqLin(InputA, Inputb, InputG, Inputh, solution);
    double error = (solution.GetX() - OutputXLSI).LinfNorm();
    CPPUNIT_ASSERT(error < Inputb.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestDynamicLSIUserAlloc(void) {
    vctDynamicVector<double> output(15);
    nmrLSqLin(InputA, Inputb, InputG, Inputh, output);
    double error = (output - OutputXLSI).LinfNorm();
    CPPUNIT_ASSERT(error < Inputb.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestDynamicLSEI(void) {
    nmrLSqLinSolutionDynamic solution(InputA, InputE, InputG);
    nmrLSqLin(InputA, Inputb, InputE, Inputf, InputG, Inputh, solution);
    double error = (solution.GetX() - OutputXLSEI).LinfNorm();
    CPPUNIT_ASSERT(error < Inputb.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestDynamicLSEIUserAlloc(void) {
    vctDynamicVector<double> output(15);
    nmrLSqLin(InputA, Inputb, InputE, Inputf, InputG, Inputh, output);
    double error = (output - OutputXLSEI).LinfNorm();
    CPPUNIT_ASSERT(error < Inputb.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestFixedSizeLS(void) {
    vctFixedSizeVector<double, 4> x;
    nmrLSqLin(A, b, x);
    double error = (x - XLS).LinfNorm();
    CPPUNIT_ASSERT(error < b.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestFixedSizeLSObject(void) {
    nmrLSqLinSolutionFixedSize<5, 0, 0, 4> solution;
    nmrLSqLin(A, b, solution);
    double error = (solution.GetX() - XLS).LinfNorm();
    CPPUNIT_ASSERT(error < b.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestFixedSizeLSI(void) {
    vctFixedSizeVector<double, 4> x;
    nmrLSqLin(A, b, G, h, x);
    double error = (x - XLSI).LinfNorm();
    CPPUNIT_ASSERT(error < b.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestFixedSizeLSIObject(void) {
    nmrLSqLinSolutionFixedSize<5, 0, 3, 4> solution;
    nmrLSqLin(A, b, G, h, solution);
    double error = (solution.GetX() - XLSI).LinfNorm();
    CPPUNIT_ASSERT(error < b.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestFixedSizeLSEI(void) {
    vctFixedSizeVector<double, 4> x;
    nmrLSqLin(A, b, E, f, G, h, x);
    double error = (x - XLSEI).LinfNorm();
    CPPUNIT_ASSERT(error < b.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestFixedSizeLSEIObject(void) {
    nmrLSqLinSolutionFixedSize<5, 2, 3, 4> solution;
    nmrLSqLin(A, b, E, f, G, h, solution);
    double error = (solution.GetX() - XLSEI).LinfNorm();
    CPPUNIT_ASSERT(error < b.size()*cmnTypeTraits<double>::Tolerance());
}

#ifdef BUILD_MULTITHREADED_TESTS
void nmrLSqLinTest::TestDynamicLSThreadSafety(void)
{
    int ii;
    char name[10];
    osaThread *thread = new osaThread[numberThread];
    nmrLSqLinDynamicTestObject *threadobj = new nmrLSqLinDynamicTestObject[numberThread];
    for (ii = 0; ii < numberThread; ii++) {
        argument arg;
        arg.times = numberTimes; arg.number = ii;
        sprintf(name, "T%d", ii);
        thread[ii].Create<nmrLSqLinDynamicTestObject, argument>(&threadobj[ii],
                                                             &nmrLSqLinDynamicTestObject::RunThreadLS,
                                                             arg, name);
    }
    for (ii = 0; ii < numberThread; ii++) {
        thread[ii].Wait();
    }
    delete[] threadobj;
    delete[] thread;
}

void nmrLSqLinTest::TestDynamicLSIThreadSafety(void)
{
    int ii;
    char name[10];
    osaThread *thread = new osaThread[numberThread];
    nmrLSqLinDynamicTestObject *threadobj = new nmrLSqLinDynamicTestObject[numberThread];
    for (ii = 0; ii < numberThread; ii++) {
        argument arg;
        arg.times = numberTimes; arg.number = ii;
        sprintf(name, "T%d", ii);
        thread[ii].Create<nmrLSqLinDynamicTestObject, argument>(&threadobj[ii],
                                                             &nmrLSqLinDynamicTestObject::RunThreadLSI,
                                                             arg, name);
    }
    for (ii = 0; ii < numberThread; ii++) {
        thread[ii].Wait();
    }
    delete[] threadobj;
    delete[] thread;
}

void nmrLSqLinTest::TestDynamicLSEIThreadSafety(void)
{
    int ii;
    char name[10];
    osaThread *thread = new osaThread[numberThread];
    nmrLSqLinDynamicTestObject *threadobj = new nmrLSqLinDynamicTestObject[numberThread];
    for (ii = 0; ii < numberThread; ii++) {
        argument arg;
        arg.times = numberTimes; arg.number = ii;
        sprintf(name, "T%d", ii);
        thread[ii].Create<nmrLSqLinDynamicTestObject, argument>(&threadobj[ii],
                                                             &nmrLSqLinDynamicTestObject::RunThreadLSEI,
                                                             arg, name);
    }
    for (ii = 0; ii < numberThread; ii++) {
        thread[ii].Wait();
    }
    delete[] threadobj;
    delete[] thread;
}
#endif // BUILD_MULTITHREADED_TESTS


CPPUNIT_TEST_SUITE_REGISTRATION(nmrLSqLinTest);


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrLSqLinTest.cpp,v $
// Revision 1.10  2007/04/26 20:12:05  anton
// All files in tests: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
//
// Revision 1.9  2007/02/12 03:25:03  anton
// cisstVector and cisstNumerical tests: Used more refined #include for vctRandom
// (see ticket #264).
//
// Revision 1.8  2006/11/20 20:33:52  anton
// Licensing: Applied new license to tests.
//
// Revision 1.7  2006/10/06 14:32:51  anton
// All tests: Re-ordered #include to load cmnPortability.h before any system
// include.
//
// Revision 1.6  2006/01/09 22:54:57  anton
// cisstNumerical tests: Reduced number of threads for tests.  Was quite time
// consuming.
//
// Revision 1.5  2006/01/05 03:54:40  anton
// cisstNumerical tests: Compile multithreaded tests only if cisstOSAbstraction
// has been compiled instead of requiring cisstOSAbstraction when cisstNetlib
// is used.  See ticket #200.
//
// Revision 1.4  2005/12/28 21:59:01  anton
// cisstNumerical: Removed nmrCommon.h since it has become obsolete.
//
// Revision 1.3  2005/12/01 03:16:19  anton
// nmrLSqLinTest: Better #if around thread tests to allow compilation with
// cnetlib as well (was working with cisstNetlib only).
//
// Revision 1.2  2005/11/30 06:03:32  kapoor
// cisstNumerical : Added LSqLin tests when cisstNetlib is used.
//
// Revision 1.1  2005/11/28 22:04:44  kapoor
// cisstNumerical: Changes to the netlib wrappers. See wiki pages SvdNotes and ticket #184.
//
//
// ****************************************************************************
