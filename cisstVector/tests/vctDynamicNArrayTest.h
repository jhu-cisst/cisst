/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Daniel Li, Anton Deguet
  Created on: 2006-07-10

  (C) Copyright 2006-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstVector/vctForwardDeclarations.h>


class vctDynamicNArrayTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(vctDynamicNArrayTest);

    CPPUNIT_TEST(TestAssignmentDouble);
    CPPUNIT_TEST(TestAssignmentFloat);
    CPPUNIT_TEST(TestAssignmentInt);

#if DANIEL
    CPPUNIT_TEST(TestAccessMethodsDouble);
    CPPUNIT_TEST(TestAccessMethodsFloat);
    CPPUNIT_TEST(TestAccessMethodsInt);
#endif  // DANIEL

    CPPUNIT_TEST(TestSoNiOperationsDouble);
    CPPUNIT_TEST(TestSoNiOperationsFloat);
    CPPUNIT_TEST(TestSoNiOperationsInt);

    CPPUNIT_TEST(TestSoNiNiOperationsDouble);
    CPPUNIT_TEST(TestSoNiNiOperationsFloat);
    CPPUNIT_TEST(TestSoNiNiOperationsInt);

    CPPUNIT_TEST(TestNioNiOperationsDouble);
    CPPUNIT_TEST(TestNioNiOperationsFloat);
    CPPUNIT_TEST(TestNioNiOperationsInt);

    CPPUNIT_TEST(TestNioSiNiOperationsDouble);
    CPPUNIT_TEST(TestNioSiNiOperationsFloat);
    CPPUNIT_TEST(TestNioSiNiOperationsInt);

    CPPUNIT_TEST(TestNioNiNiOperationsDouble);
    CPPUNIT_TEST(TestNioNiNiOperationsFloat);
    CPPUNIT_TEST(TestNioNiNiOperationsInt);

    CPPUNIT_TEST(TestNoNiNiOperationsDouble);
    CPPUNIT_TEST(TestNoNiNiOperationsFloat);
    CPPUNIT_TEST(TestNoNiNiOperationsInt);

    CPPUNIT_TEST(TestNoNiOperationsDouble);
    CPPUNIT_TEST(TestNoNiOperationsFloat);
    CPPUNIT_TEST(TestNoNiOperationsInt);

    CPPUNIT_TEST(TestNoNiSiOperationsDouble);
    CPPUNIT_TEST(TestNoNiSiOperationsFloat);
    CPPUNIT_TEST(TestNoNiSiOperationsInt);

    CPPUNIT_TEST(TestNoSiNiOperationsDouble);
    CPPUNIT_TEST(TestNoSiNiOperationsFloat);
    CPPUNIT_TEST(TestNoSiNiOperationsInt);

    CPPUNIT_TEST(TestNioSiOperationsDouble);
    CPPUNIT_TEST(TestNioSiOperationsFloat);
    CPPUNIT_TEST(TestNioSiOperationsInt);

    CPPUNIT_TEST(TestNioOperationsDouble);
    CPPUNIT_TEST(TestNioOperationsFloat);
    CPPUNIT_TEST(TestNioOperationsInt);

    CPPUNIT_TEST(TestSoNiSiOperationsDouble);
    CPPUNIT_TEST(TestSoNiSiOperationsFloat);
    CPPUNIT_TEST(TestSoNiSiOperationsInt);

    CPPUNIT_TEST(TestMinAndMaxDouble);
    CPPUNIT_TEST(TestMinAndMaxFloat);
    CPPUNIT_TEST(TestMinAndMaxInt);

    CPPUNIT_TEST(TestIteratorsDouble);
    CPPUNIT_TEST(TestIteratorsFloat);
    CPPUNIT_TEST(TestIteratorsInt);

    CPPUNIT_TEST(TestSTLFunctionsDouble);
    CPPUNIT_TEST(TestSTLFunctionsFloat);
    CPPUNIT_TEST(TestSTLFunctionsInt);

    CPPUNIT_TEST(TestIsCompactDouble);
    CPPUNIT_TEST(TestIsCompactFloat);
    CPPUNIT_TEST(TestIsCompactInt);

    CPPUNIT_TEST(TestFastCopyOfDouble);
    CPPUNIT_TEST(TestFastCopyOfFloat);
    CPPUNIT_TEST(TestFastCopyOfInt);

    CPPUNIT_TEST_SUITE_END();

 public:

    enum {MIN_DIM = 2, MAX_DIM = 6};
    enum {MIN_SIZE = 3, MAX_SIZE = 8};

    void setUp(void) {
    }

    void tearDown(void) {
    }

    typedef vct::size_type size_type;
    typedef vct::size_type dimension_type;
    typedef vct::stride_type stride_type;

    /*! Test assignments */
    template<class _elementType>
        void TestAssignment(void);
    void TestAssignmentDouble(void);
    void TestAssignmentFloat(void);
    void TestAssignmentInt(void);

#if DANIEL
    /*! Test access methods */
    template<class _elementType>
        void TestAccessMethods(void);
    void TestAccessMethodsDouble(void);
    void TestAccessMethodsFloat(void);
    void TestAccessMethodsInt(void);
#endif  // DANIEL

    /*! Test SoNi operations */
    template<class _elementType>
        void TestSoNiOperations(void);
    void TestSoNiOperationsDouble(void);
    void TestSoNiOperationsFloat(void);
    void TestSoNiOperationsInt(void);

    /*! Test SoNiNi operations */
    template<class _elementType>
        void TestSoNiNiOperations(void);
    void TestSoNiNiOperationsDouble(void);
    void TestSoNiNiOperationsFloat(void);
    void TestSoNiNiOperationsInt(void);

    /*! Test NioNi operations */
    template<class _elementType>
        void TestNioNiOperations(void);
    void TestNioNiOperationsDouble(void);
    void TestNioNiOperationsFloat(void);
    void TestNioNiOperationsInt(void);

    /*! Test NioSiNi operations */
    template<class _elementType>
        void TestNioSiNiOperations(void);
    void TestNioSiNiOperationsDouble(void);
    void TestNioSiNiOperationsFloat(void);
    void TestNioSiNiOperationsInt(void);

    /*! Test NioNiNi operations */
    template<class _elementType>
        void TestNioNiNiOperations(void);
    void TestNioNiNiOperationsDouble(void);
    void TestNioNiNiOperationsFloat(void);
    void TestNioNiNiOperationsInt(void);

    /*! Test NoNiNi operations */
    template<class _elementType>
        void TestNoNiNiOperations(void);
    void TestNoNiNiOperationsDouble(void);
    void TestNoNiNiOperationsFloat(void);
    void TestNoNiNiOperationsInt(void);

    /*! Test NoNi operations */
    template<class _elementType>
        void TestNoNiOperations(void);
    void TestNoNiOperationsDouble(void);
    void TestNoNiOperationsFloat(void);
    void TestNoNiOperationsInt(void);

    /*! Test NoNiSi operations */
    template<class _elementType>
        void TestNoNiSiOperations(void);
    void TestNoNiSiOperationsDouble(void);
    void TestNoNiSiOperationsFloat(void);
    void TestNoNiSiOperationsInt(void);

    /*! Test NoSiNi operations */
    template<class _elementType>
        void TestNoSiNiOperations(void);
    void TestNoSiNiOperationsDouble(void);
    void TestNoSiNiOperationsFloat(void);
    void TestNoSiNiOperationsInt(void);

    /*! Test NioSi operations */
    template<class _elementType>
        void TestNioSiOperations(void);
    void TestNioSiOperationsDouble(void);
    void TestNioSiOperationsFloat(void);
    void TestNioSiOperationsInt(void);

    /*! Test Nio operations */
    template<class _elementType>
        void TestNioOperations(void);
    void TestNioOperationsDouble(void);
    void TestNioOperationsFloat(void);
    void TestNioOperationsInt(void);

    /*! Test SoNiSi operations */
    template<class _elementType>
        void TestSoNiSiOperations(void);
    void TestSoNiSiOperationsDouble(void);
    void TestSoNiSiOperationsFloat(void);
    void TestSoNiSiOperationsInt(void);

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

    /*! Test STL functions */
    template<class _elementType>
        void TestSTLFunctions(void);
    void TestSTLFunctionsDouble(void);
    void TestSTLFunctionsFloat(void);
    void TestSTLFunctionsInt(void);

    /*! Test IsCompact */
    template<class _elementType>
        void TestIsCompact(void);
    void TestIsCompactDouble(void);
    void TestIsCompactFloat(void);
    void TestIsCompactInt(void);

    /*! Test FastCopyOf */
    template<class _elementType>
        void TestFastCopyOf(void);
    void TestFastCopyOfDouble(void);
    void TestFastCopyOfFloat(void);
    void TestFastCopyOfInt(void);

};


