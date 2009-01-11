/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrIsOrthonormalTest.h,v 1.11 2007/04/26 20:12:05 anton Exp $
  
  Author(s):  Anton Deguet
  Created on: 2005-07-27
  
  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _nmrIsOrthonormalTest_h
#define _nmrIsOrthonormalTest_h

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstVector/vctRandomFixedSizeMatrix.h>
#include <cisstVector/vctRandomDynamicMatrix.h>

#include <cisstNumerical/nmrNetlib.h>

#ifdef CISST_HAS_NETLIB
#include <cisstNumerical/nmrSVD.h>
#endif // CISST_HAS_NETLIB

#include <cisstNumerical/nmrIsOrthonormal.h>

class nmrIsOrthonormalTest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(nmrIsOrthonormalTest);

    CPPUNIT_TEST(TestRotationMatrix);

#ifdef CISST_HAS_NETLIB
    CPPUNIT_TEST(TestFunctionDynamic);
    CPPUNIT_TEST(TestFunctionWithReferenceDynamic);
    CPPUNIT_TEST(TestDynamicData);
    CPPUNIT_TEST(TestDynamicDataWithReferenceDynamic);

    CPPUNIT_TEST(TestFunctionFixedSize);
    CPPUNIT_TEST(TestFixedSizeData);
#endif // CISST_HAS_NETLIB

    CPPUNIT_TEST_SUITE_END();

protected:
#ifdef CISST_HAS_NETLIB
    vctDynamicMatrix<double> UDynamic, VtDynamic;
    enum {SIZE = 12};
    vctFixedSizeMatrix<double, SIZE, SIZE> UFixedSize, VtFixedSize;
#endif // CISST_HAS_NETLIB

public:

    void setUp()
    {
#ifdef CISST_HAS_NETLIB
        // create a random matrix (size and content) to decompose
        // using SVD and then use U and Vt to test nmrIsOrthonormal
        int rows, cols;
        bool storageOrder = VCT_ROW_MAJOR;
        vctDynamicMatrix<double> ADynamic;
        vctDynamicVector<double> SDynamic;
        cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
        randomSequence.ExtractRandomValue(storageOrder);
        randomSequence.ExtractRandomValue(10, 20, rows);
        randomSequence.ExtractRandomValue(10, 20, cols);
        ADynamic.SetSize(rows, cols, storageOrder);
        vctRandom(ADynamic, -10.0, 10.0);
        UDynamic.SetSize(rows, rows, storageOrder);
        SDynamic.SetSize(std::min(rows, cols));
        VtDynamic.SetSize(cols, cols, storageOrder);
        // Decompose, after this U and Vt can be used
        nmrSVD(ADynamic, UDynamic, SDynamic, VtDynamic);

        // create a fixed size matrix, decompose and copy results to
        // data members
        vctFixedSizeMatrix<double, SIZE, SIZE, VCT_ROW_MAJOR> A;
        vctRandom(A, -10.0, 10.0);
        nmrSVDFixedSizeData<SIZE, SIZE, VCT_ROW_MAJOR> svdData;
        nmrSVD(A, svdData);
        UFixedSize.Assign(svdData.U());
        VtFixedSize.Assign(svdData.Vt());
#endif // CISST_HAS_NETLIB
    }
    
    void tearDown()
    {}

    /* Test if a rotation matrix is orthonormal */
    void TestRotationMatrix(void);

#ifdef CISST_HAS_NETLIB
    /* Test the function without a data object */
    void TestFunctionDynamic(void);

    /* Test the function with a workspace reference */
    void TestFunctionWithReferenceDynamic(void);

    /* Test the function with a data object */
    void TestDynamicData(void);

    /* Test the function with a data object using an existing workspace */
    void TestDynamicDataWithReferenceDynamic(void);

    /* Test the function without a data object */
    void TestFunctionFixedSize(void);

    /* Test the function with a data object */
    void TestFixedSizeData(void);
#endif // CISST_HAS_NETLIB

};


#endif // _nmrIsOrthonormalTest_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrIsOrthonormalTest.h,v $
// Revision 1.11  2007/04/26 20:12:05  anton
// All files in tests: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
//
// Revision 1.10  2007/02/12 03:25:03  anton
// cisstVector and cisstNumerical tests: Used more refined #include for vctRandom
// (see ticket #264).
//
// Revision 1.9  2006/11/20 20:33:52  anton
// Licensing: Applied new license to tests.
//
// Revision 1.8  2006/01/27 01:00:42  anton
// cisstNumerical tests: Renamed "solution" to "data" (see #205).
//
// Revision 1.7  2006/01/11 20:53:17  anton
// cisstNumerical Code: Introduced CISST_HAS_NETLIB to indicate that any
// netlib distribution is available (either cnetlib or cisstNetlib).
//
// Revision 1.6  2006/01/04 22:36:36  anton
// nmrIsOrthonormalTest: Added test for fixed size functions and solution.
//
// Revision 1.5  2006/01/03 22:42:35  anton
// nmrIsOrthonormalTest: Added tests for new signatures using nmrIsOrthonormalSolutionDynamic.
// Also use setUp() to create a couple of orthonormal matrices using SVD.
//
// Revision 1.4  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.3  2005/09/06 15:41:37  anton
// cisstNumerical tests: Added license.
//
// Revision 1.2  2005/08/04 19:25:46  anton
// nmrIsOrthonormalTest: More subtile includes
//
// Revision 1.1  2005/07/27 22:40:33  anton
// cisstNumerical tests: Added some basic tests for nmrIsOthonormal.
//
// ****************************************************************************
