/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2003-08-20

  (C) Copyright 2003-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstVector/vctFixedSizeVector.h>

/*! This is a temporary flag to disable testing GetSubsequence.  The GetSubsequence
  method has been deprecated, and therefore we should change this test or get rid of it.
*/
#define TEST_SUBSEQUENCE 0

class vctFixedSizeVectorTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(vctFixedSizeVectorTest);

    CPPUNIT_TEST(TestSubsequence);

    CPPUNIT_TEST(TestConcatenationDouble);
    CPPUNIT_TEST(TestConcatenationFloat);
    CPPUNIT_TEST(TestConcatenationInt);
    CPPUNIT_TEST(TestConcatenationChar);

    CPPUNIT_TEST(TestConcatenationOperatorDouble);
    CPPUNIT_TEST(TestConcatenationOperatorFloat);
    CPPUNIT_TEST(TestConcatenationOperatorInt);
    CPPUNIT_TEST(TestConcatenationOperatorChar);

    CPPUNIT_TEST(TestAssignmentDouble);
    CPPUNIT_TEST(TestAssignmentFloat);
    CPPUNIT_TEST(TestAssignmentInt);

    CPPUNIT_TEST(TestSwapElementsDouble);
    CPPUNIT_TEST(TestSwapElementsFloat);
    CPPUNIT_TEST(TestSwapElementsInt);

    CPPUNIT_TEST(TestAccessMethodsDouble);
    CPPUNIT_TEST(TestAccessMethodsFloat);
    CPPUNIT_TEST(TestAccessMethodsInt);

    CPPUNIT_TEST(TestXYZWMethodsDouble);
    CPPUNIT_TEST(TestXYZWMethodsFloat);
    CPPUNIT_TEST(TestXYZWMethodsInt);

    CPPUNIT_TEST(TestSelectDouble);
    CPPUNIT_TEST(TestSelectFloat);
    CPPUNIT_TEST(TestSelectInt);

    CPPUNIT_TEST(TestSoViOperationsDouble);
    CPPUNIT_TEST(TestSoViOperationsFloat);
    CPPUNIT_TEST(TestSoViOperationsInt);

    CPPUNIT_TEST(TestSoViViOperationsDouble);
    CPPUNIT_TEST(TestSoViViOperationsFloat);
    CPPUNIT_TEST(TestSoViViOperationsInt);

    CPPUNIT_TEST(TestVioViOperationsDouble);
    CPPUNIT_TEST(TestVioViOperationsFloat);
    CPPUNIT_TEST(TestVioViOperationsInt);

    CPPUNIT_TEST(TestVoViViOperationsDouble);
    CPPUNIT_TEST(TestVoViViOperationsFloat);
    CPPUNIT_TEST(TestVoViViOperationsInt);

    CPPUNIT_TEST(TestCrossProductDouble);
    CPPUNIT_TEST(TestCrossProductFloat);
    CPPUNIT_TEST(TestCrossProductInt);

    CPPUNIT_TEST(TestDotProductDouble);
    CPPUNIT_TEST(TestDotProductFloat);
    CPPUNIT_TEST(TestDotProductInt);

    CPPUNIT_TEST(TestVoViOperationsDouble);
    CPPUNIT_TEST(TestVoViOperationsFloat);
    CPPUNIT_TEST(TestVoViOperationsInt);

    CPPUNIT_TEST(TestVoViSiOperationsDouble);
    CPPUNIT_TEST(TestVoViSiOperationsFloat);
    CPPUNIT_TEST(TestVoViSiOperationsInt);

    CPPUNIT_TEST(TestVoSiViOperationsDouble);
    CPPUNIT_TEST(TestVoSiViOperationsFloat);
    CPPUNIT_TEST(TestVoSiViOperationsInt);

    CPPUNIT_TEST(TestVioSiOperationsDouble);
    CPPUNIT_TEST(TestVioSiOperationsFloat);
    CPPUNIT_TEST(TestVioSiOperationsInt);

    CPPUNIT_TEST(TestVioSiViOperationsDouble);
    CPPUNIT_TEST(TestVioSiViOperationsFloat);
    CPPUNIT_TEST(TestVioSiViOperationsInt);

    CPPUNIT_TEST(TestVioViViOperationsDouble);
    CPPUNIT_TEST(TestVioViViOperationsFloat);
    CPPUNIT_TEST(TestVioViViOperationsInt);

    CPPUNIT_TEST(TestVioOperationsDouble);
    CPPUNIT_TEST(TestVioOperationsFloat);
    CPPUNIT_TEST(TestVioOperationsInt);

    CPPUNIT_TEST(TestSoViSiOperationsDouble);
    CPPUNIT_TEST(TestSoViSiOperationsFloat);
    CPPUNIT_TEST(TestSoViSiOperationsInt);

    CPPUNIT_TEST(TestMinAndMaxDouble);
    CPPUNIT_TEST(TestMinAndMaxFloat);
    CPPUNIT_TEST(TestMinAndMaxInt);

    CPPUNIT_TEST(TestIteratorsDouble);
    CPPUNIT_TEST(TestIteratorsFloat);
    CPPUNIT_TEST(TestIteratorsInt);

    CPPUNIT_TEST(TestSTLFunctionsDouble);
    CPPUNIT_TEST(TestSTLFunctionsFloat);
    CPPUNIT_TEST(TestSTLFunctionsInt);

    CPPUNIT_TEST(TestFastCopyOfDouble);
    CPPUNIT_TEST(TestFastCopyOfFloat);
    CPPUNIT_TEST(TestFastCopyOfInt);

    CPPUNIT_TEST(TestZerosDouble);
    CPPUNIT_TEST(TestZerosFloat);
    CPPUNIT_TEST(TestZerosInt);

    CPPUNIT_TEST(TestNormalizationDouble);
    CPPUNIT_TEST(TestNormalizationFloat);

    CPPUNIT_TEST_SUITE_END();

 public:
    void setUp(void) {
    }

    void tearDown(void) {
    }

    /*! Test accessing subsequences with various strides. */
    void TestSubsequence(void);

    /* Test the Concatenation method. */
    template <class _elementType>
        void TestConcatenation(void);
    void TestConcatenationDouble(void);
    void TestConcatenationFloat(void);
    void TestConcatenationInt(void);
    void TestConcatenationChar(void);

    /* Test the concatenation operator */
    template <class _elementType>
        void TestConcatenationOperator(void);
    void TestConcatenationOperatorDouble(void);
    void TestConcatenationOperatorFloat(void);
    void TestConcatenationOperatorInt(void);
    void TestConcatenationOperatorChar(void);

    /*! Test assignment methods */
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

    /*! Test named subvector methods, such as X(), XY(), XYZ() ... */
    template<class _elementType>
    void TestXYZWMethods(void);
    void TestXYZWMethodsDouble(void);
    void TestXYZWMethodsFloat(void);
    void TestXYZWMethodsInt(void);

    /*! Test SelectFrom method */
    template<class _elementType>
        void TestSelect(void);
    void TestSelectDouble(void);
    void TestSelectFloat(void);
    void TestSelectInt(void);

    /*! Test the SwapElementsWith methods: First swap elements of two vectors, then swap one
    half of a vector with the other half.  Finally swap the alternating sequence starting at
    0 with the alternating sequence starting at 1. */
    template<class _elementType>
        void TestSwapElements(void);
    void TestSwapElementsDouble(void);
    void TestSwapElementsFloat(void);
    void TestSwapElementsInt(void);

    /*! Test SoVi operations */
    template<class _elementType>
        void TestSoViOperations(void);
    void TestSoViOperationsDouble(void);
    void TestSoViOperationsFloat(void);
    void TestSoViOperationsInt(void);

    /*! Test SoViVi operations */
    template<class _elementType>
        void TestSoViViOperations(void);
    void TestSoViViOperationsDouble(void);
    void TestSoViViOperationsFloat(void);
    void TestSoViViOperationsInt(void);

    /*! Test VioVi operations */
    template<class _elementType>
        void TestVioViOperations(void);
    void TestVioViOperationsDouble(void);
    void TestVioViOperationsFloat(void);
    void TestVioViOperationsInt(void);

    /*! Test VoViVi operations */
    template<class _elementType>
        void TestVoViViOperations(void);
    void TestVoViViOperationsDouble(void);
    void TestVoViViOperationsFloat(void);
    void TestVoViViOperationsInt(void);

    /*! Test cross product */
    template<class _elementType>
        void TestCrossProduct(void);
    void TestCrossProductDouble(void);
    void TestCrossProductFloat(void);
    void TestCrossProductInt(void);

    /*! Test dor product */
    template<class _elementType>
        void TestDotProduct(void);
    void TestDotProductDouble(void);
    void TestDotProductFloat(void);
    void TestDotProductInt(void);

    /*! Test VoVi operations */
    template<class _elementType>
        void TestVoViOperations(void);
    void TestVoViOperationsDouble(void);
    void TestVoViOperationsFloat(void);
    void TestVoViOperationsInt(void);

    /*! Test VoViSi operations */
    template<class _elementType>
        void TestVoViSiOperations(void);
    void TestVoViSiOperationsDouble(void);
    void TestVoViSiOperationsFloat(void);
    void TestVoViSiOperationsInt(void);

    /*! Test VoSiVi operations */
    template<class _elementType>
        void TestVoSiViOperations(void);
    void TestVoSiViOperationsDouble(void);
    void TestVoSiViOperationsFloat(void);
    void TestVoSiViOperationsInt(void);

    /*! Test VioSi operations */
    template<class _elementType>
        void TestVioSiOperations(void);
    void TestVioSiOperationsDouble(void);
    void TestVioSiOperationsFloat(void);
    void TestVioSiOperationsInt(void);

    /*! Test VioSiVi operations */
    template<class _elementType>
        void TestVioSiViOperations(void);
    void TestVioSiViOperationsDouble(void);
    void TestVioSiViOperationsFloat(void);
    void TestVioSiViOperationsInt(void);

    /*! Test VioViVi operations */
    template<class _elementType>
        void TestVioViViOperations(void);
    void TestVioViViOperationsDouble(void);
    void TestVioViViOperationsFloat(void);
    void TestVioViViOperationsInt(void);

    /*! Test Vio operations */
    template<class _elementType>
        void TestVioOperations(void);
    void TestVioOperationsDouble(void);
    void TestVioOperationsFloat(void);
    void TestVioOperationsInt(void);

    /*! Test SoViSi operations */
    template<class _elementType>
        void TestSoViSiOperations(void);
    void TestSoViSiOperationsDouble(void);
    void TestSoViSiOperationsFloat(void);
    void TestSoViSiOperationsInt(void);

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

    /*! Test FastCopyOf */
    template<class _elementType>
        void TestFastCopyOf(void);
    void TestFastCopyOfDouble(void);
    void TestFastCopyOfFloat(void);
    void TestFastCopyOfInt(void);

    /*! Test Zeros */
    template<class _elementType>
        void TestZeros(void);
    void TestZerosDouble(void);
    void TestZerosFloat(void);
    void TestZerosInt(void);

    /*! Test Normalization */
    template<class _elementType>
        void TestNormalization(void);
    void TestNormalizationDouble(void);
    void TestNormalizationFloat(void);
};

