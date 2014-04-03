/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Ofri Sadowsky
  Created on: 2006-01-04
  
  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _nmrGaussJordanInverseTest_h
#define _nmrGaussJordanInverseTest_h

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstNumerical/nmrGaussJordanInverse.h>

class nmrGaussJordanInverseTest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(nmrGaussJordanInverseTest);

    CPPUNIT_TEST(TestInverse4x4Double);
    CPPUNIT_TEST(TestInverse4x4Float);
    CPPUNIT_TEST(TestInverse3x3Double);
    CPPUNIT_TEST(TestInverse3x3Float);
    CPPUNIT_TEST(TestInverse2x2Double);
    CPPUNIT_TEST(TestInverse2x2Float);

    CPPUNIT_TEST(TestSingular4x4Double);
    CPPUNIT_TEST(TestSingular4x4Float);
    CPPUNIT_TEST(TestSingular3x3Double);
    CPPUNIT_TEST(TestSingular3x3Float);
    CPPUNIT_TEST(TestSingular2x2Double);
    CPPUNIT_TEST(TestSingular2x2Float);

    CPPUNIT_TEST_SUITE_END();

public:

    void setUp()
    {}
    
    void tearDown()
    {}

    //: Test if m1 and m2 are an inverse pair, by computing m1*m2 and m2*m1 and comparing
    // the result to the identity matrix.  The test passes if the absolute maximum
    // difference is less than the given tolerance.
    template <class _elementType, vct::size_type _size, bool _rowMajor1, bool _rowMajor2>
    void TestMatrixInverse(
        const vctFixedSizeMatrix<_elementType, _size, _size, _rowMajor1> & m1,
        const vctFixedSizeMatrix<_elementType, _size, _size, _rowMajor2> & m2,
        const _elementType tolerance);

    //: This method creates a singular matrix from a given matrix m, by selecting
    // a random row and random coefficients, then setting the selected row to be
    // a linear combination of the other rows with corresponding coefficients.
    template <class _elementType, vct::size_type _size, bool _rowMajor>
    void MakeSingularMatrix(vctFixedSizeMatrix<_elementType, _size, _size, _rowMajor> & m);

    //: Test that the inverse computed by nmrGaussJordanInverseNxN is an inverse, by creating
    // a random matrix, trying to compute its inverse, and then if it's nonsingular verify
    // the inverse property: A * A^{-1} = I, and A^{-1}^{-1} = A.
    template <class _elementType>
    void TestInverse4x4(void);

    template <class _elementType>
    void TestInverse3x3(void);

    template <class _elementType>
    void TestInverse2x2(void);

    void TestInverse4x4Double(void);
    void TestInverse4x4Float(void);
    void TestInverse3x3Double(void);
    void TestInverse3x3Float(void);
    void TestInverse2x2Double(void);
    void TestInverse2x2Float(void);

    //: Test that the singularity detection in nmrGaussJordanInverseNxN works correctly.
    // Create a random matrix.  Make it singular using MakeSingularMatrix.  Test that
    // nmrGaussJordanInverse4x4 detects that it's singular.  Then add the identity,
    // and test that the result is nonsingular.
    // The result is singular very very rarely, because the identity matrix is not
    // a linear combination of A when A is singular.  One counterexample is:
    // A = diag([1, 0, -1]) , and then A+I = diag([2, 1, 0]).  But we expect these
    // cases to be rare enough.  We could probably figure out a better way of
    // solving this, but we don't think it's necessary.  The test is meant to verify
    // that a singular matrix is identified as such, and that a nonsingular matrix
    // is also identified, so as long as the input it correct most of the time, it's
    // good enough for us, and if an incorrect input is generated, we ignore that error.
    // Note that there are still some numerical stability issues that cause incorrect
    // detection, and these may shadow the other rare cases.
    template <class _elementType>
    void TestSingular4x4(void);

    template <class _elementType>
    void TestSingular3x3(void);

    template <class _elementType>
    void TestSingular2x2(void);

    void TestSingular4x4Double(void);
    void TestSingular4x4Float(void);
    void TestSingular3x3Double(void);
    void TestSingular3x3Float(void);
    void TestSingular2x2Double(void);
    void TestSingular2x2Float(void);

    static const double ToleranceScale;
    static const double RandomElementRange;

};


#endif // _nmrGaussJordanInverseTest_h

