/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrSVDSolverTest.h,v 1.10 2007/04/26 20:12:05 anton Exp $
  
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


#ifndef _nmrSVDSolverTest_h
#define _nmrSVDSolverTest_h

// The class nmrSVDSolver is obsolete but we still want to test it for a while
#include <cisstCommon/cmnPortability.h>
#ifdef CISST_COMPILER_IS_MSVC
#pragma warning (disable: 4996)
#endif

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstNumerical/nmrSVDSolver.h>

class nmrSVDSolverTest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(nmrSVDSolverTest);

    CPPUNIT_TEST(TestDynamicColumnMajor);
    CPPUNIT_TEST(TestDynamicRowMajor);

    CPPUNIT_TEST(TestFixedSizeColumnMajor);
    CPPUNIT_TEST(TestFixedSizeRowMajor);

    CPPUNIT_TEST_SUITE_END();

public:

    void setUp()
    {}
    
    void tearDown()
    {}
    
    /*! Templated generic test */
    template <class _matrixType>
    void GenericTest(_matrixType & input);

    /*! Test using column major matrices. */
    void TestDynamicColumnMajor(void);

    /*! Test using column major matrices. */
    void TestDynamicRowMajor(void);

    void TestFixedSizeColumnMajor(void);

    void TestFixedSizeRowMajor(void);
};


#endif // _nmrSVDSolverTest_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrSVDSolverTest.h,v $
// Revision 1.10  2007/04/26 20:12:05  anton
// All files in tests: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
//
// Revision 1.9  2006/11/20 20:33:52  anton
// Licensing: Applied new license to tests.
//
// Revision 1.8  2006/09/29 20:17:53  anton
// cisstNumerical Tests: Disable warnings for deprecated code with VC 8.
//
// Revision 1.7  2006/01/20 15:12:30  anton
// cisstNumerical tests: Added compilation pragma to compile tests for
// deprecated SVD, LU and PInverse solvers.
//
// Revision 1.6  2005/12/29 23:40:44  anton
// nmrSVDSolverTest.h: Added pragma to avoid deprecated messages re. nmrSVDSolver.
// These tests will have to be removed when nmrSVDSolver will be.
//
// Revision 1.5  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.4  2005/09/06 15:41:37  anton
// cisstNumerical tests: Added license.
//
// Revision 1.3  2005/08/25 16:55:35  anton
// cisstNumerical tests: Removed #include of cisstXyz.h to avoid useless
// dependencies and long compilations.
//
// Revision 1.2  2005/07/25 19:48:30  anton
// nmrSVDSolverTest: Added tests for different storage orders and fixed size
// matrices.
//
// Revision 1.1  2005/07/22 22:30:10  anton
// cisstNumerical Tests: Added basic test for nmrSVDSolver.
//
//
// ****************************************************************************
