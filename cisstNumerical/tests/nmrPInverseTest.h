/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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


#ifndef _nmrPInverseTest_h
#define _nmrPInverseTest_h

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstNumerical/nmrPInverse.h>

class nmrPInverseTest: public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(nmrPInverseTest);

    CPPUNIT_TEST(TestDynamicColumnMajorUsingDataObject);
    CPPUNIT_TEST(TestDynamicRowMajorUsingDataObject);

    CPPUNIT_TEST(TestDynamicColumnMajor);
    CPPUNIT_TEST(TestDynamicRowMajor);

    CPPUNIT_TEST(TestFixedSizeColumnMajorRLeMLeqN);
    CPPUNIT_TEST(TestFixedSizeRowMajorRLeMLeqN);
    CPPUNIT_TEST(TestFixedSizeColumnMajorMGeqNGeR);
    CPPUNIT_TEST(TestFixedSizeRowMajorMGeqNGeR);

    CPPUNIT_TEST(TestFixedSizeColumnMajorRLeMLeqNUsingDataObject);
    CPPUNIT_TEST(TestFixedSizeRowMajorRLeMLeqNUsingDataObject);
    CPPUNIT_TEST(TestFixedSizeColumnMajorMGeqNGeRUsingDataObject);
    CPPUNIT_TEST(TestFixedSizeRowMajorMGeqNGeRUsingDataObject);

    CPPUNIT_TEST(TestCompareWithMatlab);

    CPPUNIT_TEST_SUITE_END();

public:

    void setUp()
    {}

    void tearDown()
    {}

    /*! Templated generic test */
    template <vct::size_type _rows, vct::size_type _cols, bool _storageOrder, vct::size_type _minmn>
        void GenericTestFixedSize(void);
    template <vct::size_type _rows, vct::size_type _cols, bool _storageOrder, vct::size_type _minmn>
        void GenericTestFixedSizeUsingDataObject(void);

    template <vct::size_type _rows, vct::size_type _cols>
        void GenericTestCompareWithMatlab(double input_ptr[], double pinverse_ptr[], int caseNo, double tolerance = cmnTypeTraits<double>::Tolerance());

    void GenericTestDynamic(vct::size_type rows, vct::size_type cols, bool storageOrder);
    void GenericTestDynamicUsingDataObject(vct::size_type rows, vct::size_type cols, bool storageOrder);

    /*! Test using column major matrices. */
    void TestDynamicColumnMajorUsingDataObject(void);

    /*! Test using column major matrices. */
    void TestDynamicRowMajorUsingDataObject(void);

    /*! Test using column major matrices. */
    void TestDynamicColumnMajor(void);

    /*! Test using column major matrices. */
    void TestDynamicRowMajor(void);

    /* these are using nmrPInverse(A, dataobj)  but with rank <= minmn */
    void TestFixedSizeColumnMajorRLeMLeqNUsingDataObject(void) {
        GenericTestFixedSizeUsingDataObject<5, 7, VCT_COL_MAJOR, 5>();
    }

    void TestFixedSizeRowMajorRLeMLeqNUsingDataObject(void) {
        GenericTestFixedSizeUsingDataObject<4, 6, VCT_ROW_MAJOR, 4>();
    }

    void TestFixedSizeColumnMajorMGeqNGeRUsingDataObject(void) {
        GenericTestFixedSizeUsingDataObject<7, 4, VCT_COL_MAJOR, 4>();
    }

    void TestFixedSizeRowMajorMGeqNGeRUsingDataObject(void) {
        GenericTestFixedSizeUsingDataObject<8, 5, VCT_ROW_MAJOR, 5>();
    }

    /* these are using nmrPInverse(A, A^+) but with rank <= minmn */
    void TestFixedSizeColumnMajorRLeMLeqN(void) {
        GenericTestFixedSize<5, 7, VCT_COL_MAJOR, 5>();
    }

    void TestFixedSizeRowMajorRLeMLeqN(void) {
        GenericTestFixedSize<4, 6, VCT_ROW_MAJOR, 4>();
    }

    void TestFixedSizeColumnMajorMGeqNGeR(void) {
        GenericTestFixedSize<7, 4, VCT_COL_MAJOR, 4>();
    }

    void TestFixedSizeRowMajorMGeqNGeR(void) {
        GenericTestFixedSize<8, 5, VCT_ROW_MAJOR, 5>();
    }

    void TestCompareWithMatlab(void);

};


#endif // _nmrPInverseTest_h

