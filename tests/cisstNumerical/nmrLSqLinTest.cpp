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

