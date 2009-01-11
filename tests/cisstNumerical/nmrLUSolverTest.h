/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrLUSolverTest.h,v 1.7 2007/04/26 20:12:05 anton Exp $
  
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


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrLUSolverTest.h,v $
// Revision 1.7  2007/04/26 20:12:05  anton
// All files in tests: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
//
// Revision 1.6  2006/11/20 20:33:52  anton
// Licensing: Applied new license to tests.
//
// Revision 1.5  2006/09/29 20:17:53  anton
// cisstNumerical Tests: Disable warnings for deprecated code with VC 8.
//
// Revision 1.4  2006/01/20 15:12:30  anton
// cisstNumerical tests: Added compilation pragma to compile tests for
// deprecated SVD, LU and PInverse solvers.
//
// Revision 1.3  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2005/09/06 15:41:37  anton
// cisstNumerical tests: Added license.
//
// Revision 1.1  2005/08/04 18:56:44  anton
// cisstNumericalTests: Added nmrLUSolverTest.
//
// ****************************************************************************
