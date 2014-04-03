/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2004-11-17

  (C) Copyright 2004-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>


class vctDynamicMatrixRefTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(vctDynamicMatrixRefTest);

    CPPUNIT_TEST(TestAssignmentDouble);
    CPPUNIT_TEST(TestAssignmentFloat);
    CPPUNIT_TEST(TestAssignmentInt);

    CPPUNIT_TEST(TestAccessMethodsDouble);
    CPPUNIT_TEST(TestAccessMethodsFloat);
    CPPUNIT_TEST(TestAccessMethodsInt);

    CPPUNIT_TEST(TestSoMiOperationsDouble);
    CPPUNIT_TEST(TestSoMiOperationsFloat);
    CPPUNIT_TEST(TestSoMiOperationsInt);

    CPPUNIT_TEST(TestSoMiMiOperationsDouble);
    CPPUNIT_TEST(TestSoMiMiOperationsFloat);
    CPPUNIT_TEST(TestSoMiMiOperationsInt);

    CPPUNIT_TEST(TestMioMiOperationsDouble);
    CPPUNIT_TEST(TestMioMiOperationsFloat);
    CPPUNIT_TEST(TestMioMiOperationsInt);

    CPPUNIT_TEST(TestMioSiMiOperationsDouble);
    CPPUNIT_TEST(TestMioSiMiOperationsFloat);
    CPPUNIT_TEST(TestMioSiMiOperationsInt);

    CPPUNIT_TEST(TestMioMiMiOperationsDouble);
    CPPUNIT_TEST(TestMioMiMiOperationsFloat);
    CPPUNIT_TEST(TestMioMiMiOperationsInt);

    CPPUNIT_TEST(TestMoMiMiOperationsDouble);
    CPPUNIT_TEST(TestMoMiMiOperationsFloat);
    CPPUNIT_TEST(TestMoMiMiOperationsInt);

    CPPUNIT_TEST(TestProductOperationsDouble);
    CPPUNIT_TEST(TestProductOperationsFloat);
    CPPUNIT_TEST(TestProductOperationsInt);

    CPPUNIT_TEST(TestMoMiOperationsDouble);
    CPPUNIT_TEST(TestMoMiOperationsFloat);
    CPPUNIT_TEST(TestMoMiOperationsInt);

    CPPUNIT_TEST(TestMoMiSiOperationsDouble);
    CPPUNIT_TEST(TestMoMiSiOperationsFloat);
    CPPUNIT_TEST(TestMoMiSiOperationsInt);

    CPPUNIT_TEST(TestMoSiMiOperationsDouble);
    CPPUNIT_TEST(TestMoSiMiOperationsFloat);
    CPPUNIT_TEST(TestMoSiMiOperationsInt);

    CPPUNIT_TEST(TestMioSiOperationsDouble);
    CPPUNIT_TEST(TestMioSiOperationsFloat);
    CPPUNIT_TEST(TestMioSiOperationsInt);

    CPPUNIT_TEST(TestMioOperationsDouble);
    CPPUNIT_TEST(TestMioOperationsFloat);
    CPPUNIT_TEST(TestMioOperationsInt);

    CPPUNIT_TEST(TestSoMiSiOperationsDouble);
    CPPUNIT_TEST(TestSoMiSiOperationsFloat);
    CPPUNIT_TEST(TestSoMiSiOperationsInt);

    CPPUNIT_TEST(TestMinAndMaxDouble);
    CPPUNIT_TEST(TestMinAndMaxFloat);
    CPPUNIT_TEST(TestMinAndMaxInt);

    CPPUNIT_TEST(TestIteratorsDouble);
    CPPUNIT_TEST(TestIteratorsFloat);
    CPPUNIT_TEST(TestIteratorsInt);

    CPPUNIT_TEST(TestSTLFunctionsDouble);
    CPPUNIT_TEST(TestSTLFunctionsFloat);
    CPPUNIT_TEST(TestSTLFunctionsInt);

    CPPUNIT_TEST_SUITE_END();

 public:
    void setUp(void) {
    }

    void tearDown(void) {
    }

    /*! Test SoMi operations */
    template<class _elementType>
        void TestAssignment(void);
    void TestAssignmentDouble(void);
    void TestAssignmentFloat(void);
    void TestAssignmentInt(void);

    /*! Test access methods */
    template<class _elementType>
        void TestAccessMethods(void);
    void TestAccessMethodsDouble(void);
    void TestAccessMethodsFloat(void);
    void TestAccessMethodsInt(void);

    /*! Test SoMi operations */
    template<class _elementType>
        void TestSoMiOperations(void);
    void TestSoMiOperationsDouble(void);
    void TestSoMiOperationsFloat(void);
    void TestSoMiOperationsInt(void);

    /*! Test SoMiMi operations */
    template<class _elementType>
        void TestSoMiMiOperations(void);
    void TestSoMiMiOperationsDouble(void);
    void TestSoMiMiOperationsFloat(void);
    void TestSoMiMiOperationsInt(void);

    /*! Test MioMi operations */
    template<class _elementType>
        void TestMioMiOperations(void);
    void TestMioMiOperationsDouble(void);
    void TestMioMiOperationsFloat(void);
    void TestMioMiOperationsInt(void);

    /*! Test MioSiMi operations */
    template<class _elementType>
        void TestMioSiMiOperations(void);
    void TestMioSiMiOperationsDouble(void);
    void TestMioSiMiOperationsFloat(void);
    void TestMioSiMiOperationsInt(void);

    /*! Test MioMiMi operations */
    template<class _elementType>
        void TestMioMiMiOperations(void);
    void TestMioMiMiOperationsDouble(void);
    void TestMioMiMiOperationsFloat(void);
    void TestMioMiMiOperationsInt(void);

    /*! Test MoMiMi operations */
    template<class _elementType>
        void TestMoMiMiOperations(void);
    void TestMoMiMiOperationsDouble(void);
    void TestMoMiMiOperationsFloat(void);
    void TestMoMiMiOperationsInt(void);

    /*! Test Product operations */
    template<class _elementType>
        void TestProductOperations(void);
    void TestProductOperationsDouble(void);
    void TestProductOperationsFloat(void);
    void TestProductOperationsInt(void);

    /*! Test MoMi operations */
    template<class _elementType>
        void TestMoMiOperations(void);
    void TestMoMiOperationsDouble(void);
    void TestMoMiOperationsFloat(void);
    void TestMoMiOperationsInt(void);

    /*! Test MoMiSi operations */
    template<class _elementType>
        void TestMoMiSiOperations(void);
    void TestMoMiSiOperationsDouble(void);
    void TestMoMiSiOperationsFloat(void);
    void TestMoMiSiOperationsInt(void);

    /*! Test MoSiMi operations */
    template<class _elementType>
        void TestMoSiMiOperations(void);
    void TestMoSiMiOperationsDouble(void);
    void TestMoSiMiOperationsFloat(void);
    void TestMoSiMiOperationsInt(void);

    /*! Test MioSi operations */
    template<class _elementType>
        void TestMioSiOperations(void);
    void TestMioSiOperationsDouble(void);
    void TestMioSiOperationsFloat(void);
    void TestMioSiOperationsInt(void);

    /*! Test Mio operations */
    template<class _elementType>
        void TestMioOperations(void);
    void TestMioOperationsDouble(void);
    void TestMioOperationsFloat(void);
    void TestMioOperationsInt(void);

    /*! Test SoMiSi operations */
    template<class _elementType>
        void TestSoMiSiOperations(void);
    void TestSoMiSiOperationsDouble(void);
    void TestSoMiSiOperationsFloat(void);
    void TestSoMiSiOperationsInt(void);

    /*! Test MinAndMax */
    template<class _elementType>
        void TestMinAndMax(void);
    void TestMinAndMaxDouble(void);
    void TestMinAndMaxFloat();
    void TestMinAndMaxInt(void);

    /*! Test iterators */
    template<class _elementType>
        void TestIterators(void);
    void TestIteratorsDouble(void);
    void TestIteratorsFloat(void);
    void TestIteratorsInt(void);

    /*! Test STL Functions */
    template<class _elementType>
        void TestSTLFunctions(void);
    void TestSTLFunctionsDouble(void);
    void TestSTLFunctionsFloat(void);
    void TestSTLFunctionsInt(void);
};


