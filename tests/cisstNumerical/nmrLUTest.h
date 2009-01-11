/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrLUTest.h,v 1.6 2007/05/18 21:56:36 anton Exp $
  
  Author(s):  Anton Deguet
  Created on: 2006-01-10
  
  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _nmrLUTest_h
#define _nmrLUTest_h

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstNumerical/nmrLU.h>

class nmrLUTest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(nmrLUTest);

    CPPUNIT_TEST(TestDynamicDataColumnMajor);
    CPPUNIT_TEST(TestDynamicUserOutputColumnMajor);

    CPPUNIT_TEST(TestFixedSizeDataColumnMajorMLeqN);
    CPPUNIT_TEST(TestFixedSizeDataColumnMajorMGeqN);

    CPPUNIT_TEST(TestFixedSizeUserOutputColumnMajorMLeqN);
    CPPUNIT_TEST(TestFixedSizeUserOutputColumnMajorMGeqN);

    CPPUNIT_TEST_SUITE_END();

public:

    void setUp()
    {}
    
    void tearDown()
    {}

    template <class _pivotIndicesOwner>
    void GenericTestDynamic(const vctDynamicMatrix<double> & input,
                            const vctDynamicMatrix<double> & output,
                            const vctDynamicConstVectorBase<_pivotIndicesOwner, F_INTEGER> & pivotIndices) {
        vctDynamicMatrix<double> P, L, U;
        P.SetSize(nmrLUDynamicData::MatrixPSize(input));
        L.SetSize(nmrLUDynamicData::MatrixLSize(input));
        U.SetSize(nmrLUDynamicData::MatrixUSize(input));
        nmrLUDynamicData::UpdateMatrixP(output, pivotIndices, P);
        nmrLUDynamicData::UpdateMatrixLU(output, L, U);
        
        double error = (input - (P * L * U)).LinfNorm();
        CPPUNIT_ASSERT(error < cmnTypeTraits<double>::Tolerance());
    }

    /*! Generic test for fixed size matrices. */
    template <unsigned int _rows, unsigned int _cols, unsigned int _minmn> 
    void GenericTestFixedSize(const vctFixedSizeMatrix<double, _rows, _cols, VCT_COL_MAJOR> & input,
                              const vctFixedSizeMatrix<double, _rows, _cols, VCT_COL_MAJOR> & output,
                              const vctFixedSizeVector<F_INTEGER, _minmn> & pivotIndices);


    /*! Test using column major dynamic matrices. */
    void TestDynamicDataColumnMajor(void);
    void TestDynamicUserOutputColumnMajor(void);

    /*! Test using column major fixed size matrices. */
    void TestFixedSizeDataColumnMajorMLeqN(void);
    void TestFixedSizeDataColumnMajorMGeqN(void);

    /*! Test using column major fixed size matrices. */
    void TestFixedSizeUserOutputColumnMajorMLeqN(void);
    void TestFixedSizeUserOutputColumnMajorMGeqN(void);
};


#endif // _nmrLUTest_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrLUTest.h,v $
// Revision 1.6  2007/05/18 21:56:36  anton
// cisstNumerical tests: Modified signature of test methods to avoid implicit
// conversion between containers and containersRef as these constructors are
// now "explicit".   See [2349] by Ofri.
//
// Revision 1.5  2007/04/26 20:12:05  anton
// All files in tests: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
//
// Revision 1.4  2006/11/20 20:33:52  anton
// Licensing: Applied new license to tests.
//
// Revision 1.3  2006/01/27 01:00:42  anton
// cisstNumerical tests: Renamed "solution" to "data" (see #205).
//
// Revision 1.2  2006/01/11 04:50:32  anton
// nmrLU: Working version, including tests for fixed/dynamic, solution/user
// allocated pivot indices vector.  Still have to update the Doxygen comments.
//
// Revision 1.1  2006/01/10 23:25:33  anton
// cisstNumerical: Incomplete but somewhat working version of nmrLU.
//
//
// ****************************************************************************
