/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Ankur Kapoor
  Created on: 2005-11-04
  
  (C) Copyright 2005-2015 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "nmrLSqLinTest.h"

#include <cisstCommon/cmnConstants.h>
#include <cisstVector/vctRandom.h>
#include <cisstNumerical/nmrIsOrthonormal.h>
#include <cisstNumerical/nmrLSqLin.h>


const int numberThread = 25;
const int numberTimes = 100;


void nmrLSqLinTest::TestDynamicLS(void) {
    CISSTNETLIB_INTEGER ret;
    nmrLSqLinSolutionDynamic solution(InputA);
    ret = nmrLSqLin(InputA, Inputb, solution);
    CPPUNIT_ASSERT(ret == 0);
    double error = (solution.GetX() - OutputXLS).LinfNorm();
    CPPUNIT_ASSERT(error < Inputb.size()*cmnTypeTraits<double>::Tolerance());
}

// Tests an underconstrained system of equations. This could occur, for example,
// if we wish to solve for the 7 joint angle velocities of a robot to achieve a desired
// Cartesian velocity objective.
void nmrLSqLinTest::TestDynamicLS_Underconstrained(void) {
    CISSTNETLIB_INTEGER ret;
    vctDynamicMatrix<double> testA(3,7, VCT_COL_MAJOR);  // 3 x 7 matrix (e.g., position Jacobian of robot)
    vctRandom(testA, -10.0, 10.0);
    vctDynamicVector<double> testx(7);    // 7 unknowns (e.g., robot joint angle velocities)
    vctRandom(testx, -cmnPI_2, cmnPI_2);
    vctDynamicVector<double> testb(3);    // 3 objectives (e.g., robot Cartesian translational velocity)
    testb.ProductOf(testA, testx);
    nmrLSqLinSolutionDynamic solution(testA);
    // We know that the underconstrained equation has a solution, but it may not be a unique solution
    // so we check (A*x' - A*x) = A*(x' - x), where x' is the computed result and x is the original value.
    vctDynamicMatrix<double> savedA(testA);
    ret = nmrLSqLin(testA, testb, solution);
    CPPUNIT_ASSERT(ret == 0);
    double error = (savedA*(solution.GetX() - testx)).LinfNorm();
    CPPUNIT_ASSERT(error < testb.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestDynamicLSUserAlloc(void) {
    CISSTNETLIB_INTEGER ret;
    vctDynamicVector<double> output(15);
    ret = nmrLSqLin(InputA, Inputb, output);
    CPPUNIT_ASSERT(ret == 0);
    double error = (output - OutputXLS).LinfNorm();
    CPPUNIT_ASSERT(error < Inputb.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestDynamicLSUserAlloc_Underconstrained(void) {
    CISSTNETLIB_INTEGER ret;
    vctDynamicMatrix<double> testA(3,7, VCT_COL_MAJOR);  // 3 x 7 matrix (e.g., position Jacobian of robot)
    vctRandom(testA, -10.0, 10.0);
    vctDynamicVector<double> testx(7);    // 7 unknowns (e.g., robot joint angle velocities)
    vctRandom(testx, -cmnPI_2, cmnPI_2);
    vctDynamicVector<double> testb(3);    // 3 objectives (e.g., robot Cartesian translational velocity)
    testb.ProductOf(testA, testx);
    vctDynamicVector<double> output(7);
    // We know that the underconstrained equation has a solution, but it may not be a unique solution
    // so we check (A*x' - A*x) = A*(x' - x), where x' is the computed result and x is the original value.
    vctDynamicMatrix<double> savedA(testA);
    ret = nmrLSqLin(testA, testb, output);
    CPPUNIT_ASSERT(ret == 0);
    double error = (savedA*(output - testx)).LinfNorm();
    CPPUNIT_ASSERT(error < testb.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestDynamicLSI(void) {
    CISSTNETLIB_INTEGER ret;
    nmrLSqLinSolutionDynamic solution(InputA, InputG);
    ret = nmrLSqLin(InputA, Inputb, InputG, Inputh, solution);
    CPPUNIT_ASSERT(ret == 0);
    double error = (solution.GetX() - OutputXLSI).LinfNorm();
    CPPUNIT_ASSERT(error < Inputb.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestDynamicLSIUserAlloc(void) {
    CISSTNETLIB_INTEGER ret;
    vctDynamicVector<double> output(15);
    ret = nmrLSqLin(InputA, Inputb, InputG, Inputh, output);
    CPPUNIT_ASSERT(ret == 0);
    double error = (output - OutputXLSI).LinfNorm();
    CPPUNIT_ASSERT(error < Inputb.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestDynamicLSEI(void) {
    CISSTNETLIB_INTEGER ret;
    nmrLSqLinSolutionDynamic solution(InputA, InputE, InputG);
    ret = nmrLSqLin(InputA, Inputb, InputE, Inputf, InputG, Inputh, solution);
    CPPUNIT_ASSERT(ret == 0);
    double error = (solution.GetX() - OutputXLSEI).LinfNorm();
    CPPUNIT_ASSERT(error < Inputb.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestDynamicLSEIUserAlloc(void) {
    CISSTNETLIB_INTEGER ret;
    vctDynamicVector<double> output(15);
    ret = nmrLSqLin(InputA, Inputb, InputE, Inputf, InputG, Inputh, output);
    CPPUNIT_ASSERT(ret == 0);
    double error = (output - OutputXLSEI).LinfNorm();
    CPPUNIT_ASSERT(error < Inputb.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestFixedSizeLS(void) {
    CISSTNETLIB_INTEGER ret;
    vctFixedSizeVector<double, 4> x;
    ret = nmrLSqLin(A, b, x);
    CPPUNIT_ASSERT(ret == 0);
    double error = (x - XLS).LinfNorm();
    CPPUNIT_ASSERT(error < b.size()*cmnTypeTraits<double>::Tolerance());
}

// Tests an underconstrained system of equations. This could occur, for example,
// if we wish to solve for the 7 joint angle velocities of a robot to achieve a desired
// Cartesian velocity objective.
void nmrLSqLinTest::TestFixedSizeLS_Underconstrained(void) {
    CISSTNETLIB_INTEGER ret;
    vctFixedSizeMatrix<double, 3, 7, VCT_COL_MAJOR> testA;  // 3 x 7 matrix (e.g., position Jacobian of robot)
    vctRandom(testA, -10.0, 10.0);
    vctFixedSizeVector<double, 7> testx;    // 7 unknowns (e.g., robot joint angle velocities)
    vctRandom(testx, -cmnPI_2, cmnPI_2);
    vctFixedSizeVector<double, 3> testb;    // 3 objectives (e.g., robot Cartesian translational velocity)
    testb.ProductOf(testA, testx);
    vctFixedSizeVector<double, 7> x;        // 7 unknowns (e.g., robot joint angle velocities)
    // We know that the underconstrained equation has a solution, but it may not be a unique solution
    // so we check (A*x' - A*x) = A*(x' - x), where x' is the computed result and x is the original value.
    vctFixedSizeMatrix<double, 3, 7, VCT_COL_MAJOR> savedA(testA);
    ret = nmrLSqLin(testA, testb, x);
    CPPUNIT_ASSERT(ret == 0);
    double error = (savedA*(x - testx)).LinfNorm();
    CPPUNIT_ASSERT(error < testb.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestFixedSizeLSObject(void) {
    CISSTNETLIB_INTEGER ret;
    nmrLSqLinSolutionFixedSize<5, 0, 0, 4> solution;
    ret = nmrLSqLin(A, b, solution);
    CPPUNIT_ASSERT(ret == 0);
    double error = (solution.GetX() - XLS).LinfNorm();
    CPPUNIT_ASSERT(error < b.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestFixedSizeLSObject_Underconstrained(void) {
    CISSTNETLIB_INTEGER ret;
    vctFixedSizeMatrix<double, 3, 7, VCT_COL_MAJOR> testA;  // 3 x 7 matrix (e.g., position Jacobian of robot)
    vctRandom(testA, -10.0, 10.0);
    vctFixedSizeVector<double, 7> testx;    // 7 unknowns (e.g., robot joint angle velocities)
    vctRandom(testx, -cmnPI_2, cmnPI_2);
    vctFixedSizeVector<double, 3> testb;    // 3 objectives (e.g., robot Cartesian translational velocity)
    testb.ProductOf(testA, testx);
    nmrLSqLinSolutionFixedSize<3, 0, 0, 7> solution;
    // We know that the underconstrained equation has a solution, but it may not be a unique solution
    // so we check (A*x' - A*x) = A*(x' - x), where x' is the computed result and x is the original value.
    vctFixedSizeMatrix<double, 3, 7, VCT_COL_MAJOR> savedA(testA);
    ret = nmrLSqLin(testA, testb, solution);
    CPPUNIT_ASSERT(ret == 0);
    double error = (savedA*(solution.GetX() - testx)).LinfNorm();
    CPPUNIT_ASSERT(error < testb.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestFixedSizeLSI(void) {
    CISSTNETLIB_INTEGER ret;
    vctFixedSizeVector<double, 4> x;
    ret = nmrLSqLin(A, b, G, h, x);
    CPPUNIT_ASSERT(ret == 0);
    double error = (x - XLSI).LinfNorm();
    CPPUNIT_ASSERT(error < b.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestFixedSizeLSIObject(void) {
    CISSTNETLIB_INTEGER ret;
    nmrLSqLinSolutionFixedSize<5, 0, 3, 4> solution;
    ret = nmrLSqLin(A, b, G, h, solution);
    CPPUNIT_ASSERT(ret == 0);
    double error = (solution.GetX() - XLSI).LinfNorm();
    CPPUNIT_ASSERT(error < b.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestFixedSizeLSEI(void) {
    CISSTNETLIB_INTEGER ret;
    vctFixedSizeVector<double, 4> x;
    ret = nmrLSqLin(A, b, E, f, G, h, x);
    CPPUNIT_ASSERT(ret == 0);
    double error = (x - XLSEI).LinfNorm();
    CPPUNIT_ASSERT(error < b.size()*cmnTypeTraits<double>::Tolerance());
}

void nmrLSqLinTest::TestFixedSizeLSEIObject(void) {
    CISSTNETLIB_INTEGER ret;
    nmrLSqLinSolutionFixedSize<5, 2, 3, 4> solution;
    ret = nmrLSqLin(A, b, E, f, G, h, solution);
    CPPUNIT_ASSERT(ret == 0);
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

