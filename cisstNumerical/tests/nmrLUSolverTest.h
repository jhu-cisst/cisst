/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2005-07-22
  
  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _nmrLUSolverTest_h
#define _nmrLUSolverTest_h

// The class nmrLUSolver is obsolete but we still want to test it for a while
#include <cisstCommon/cmnPortability.h>
#ifdef CISST_COMPILER_IS_MSVC
#pragma warning (disable: 4996)
#endif

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstNumerical/nmrLUSolver.h>

class nmrLUSolverTest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(nmrLUSolverTest);

    CPPUNIT_TEST(TestDynamicColumnMajor);
    // There is some extra work needed for the row major.
    // CPPUNIT_TEST(TestDynamicRowMajor);

    CPPUNIT_TEST(TestFixedSizeColumnMajor);
    // CPPUNIT_TEST(TestFixedSizeRowMajor);

    CPPUNIT_TEST_SUITE_END();

public:

    void setUp()
    {}
    
    void tearDown()
    {}
    
    /*! Templated generic test */
    template <class _matrixType>
    void GenericTest(_matrixType & input);

    /*! Test using column major dynamic matrices. */
    void TestDynamicColumnMajor(void);

    /*! Test using column major dynamic matrices. */
    // void TestDynamicRowMajor(void);

    /*! Test using column major fixed size matrices. */
    void TestFixedSizeColumnMajor(void);

    /*! Test using row major fixed size matrices. */
    // void TestFixedSizeRowMajor(void);
};


#endif // _nmrLUSolverTest_h

