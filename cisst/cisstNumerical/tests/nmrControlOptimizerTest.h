#ifndef _nmrControlOptimizerTest_h
#define _nmrControlOptimizerTest_h

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
 $Id: $
 
 Author(s):  Paul Wilkening
 Created on:
 
 (C) Copyright 2013 Johns Hopkins University (JHU), All Rights Reserved.
 
 --- begin cisst license - do not edit ---
 
 This software is provided "as is" under an open source license, with
 no warranty.  The complete license can be found in license.txt and
 http://www.cisst.org/cisst/license.txt.
 
 --- end cisst license ---
 */

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstNumerical/nmrControlOptimizer.h>

class nmrControlOptimizerTest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(nmrControlOptimizerTest);
    
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestResetIndices);
	CPPUNIT_TEST(TestAllocate);
	CPPUNIT_TEST(TestReserveSpace);
	CPPUNIT_TEST(TestGetObjectiveSpace);
	CPPUNIT_TEST(TestGetIneqConstraintSpace);
	CPPUNIT_TEST(TestGetEqConstraintSpace);
	CPPUNIT_TEST(TestSlacks);
    CPPUNIT_TEST(TestSolve);

    CPPUNIT_TEST_SUITE_END();

public:
    void setUp()
    {}
    
    void tearDown()
    {}

    /*! Test of constructor. */
    void TestConstructor(void);

    /*! Test of reset_indices. */
    void TestResetIndices(void);

    /*! Test of allocate. */
    void TestAllocate(void);

    /*! Test of Reserve Space. */
    void TestReserveSpace(void);

    /*! Test of GetObjectiveSpace. */
    void TestGetObjectiveSpace(void);

    /*! Test of GetIneqConstraintSpace. */
    void TestGetIneqConstraintSpace(void);

    /*! Test of GetEqConstraintSpace. */
    void TestGetEqConstraintSpace(void);

    /*! Test of Slacks. */
    void TestSlacks(void);

    /*! Test of Solve. */
    void TestSolve(void);
};


#endif

