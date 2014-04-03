/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):	Ofri Sadowsky
  Created on:	2004-04-20
  
  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _vctDeterminantTest_h
#define _vctDeterminantTest_h

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstVector/vctDeterminant.h>


#include <cisstVector/vctFixedSizeMatrix.h>

class cmnRandomSequence;

class vctDeterminantTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(vctDeterminantTest);
    CPPUNIT_TEST(TestDeterminant2x2ByInverse);
    CPPUNIT_TEST(TestDeterminant2x2ByOrthogonal);
    CPPUNIT_TEST(TestDeterminant2x2ByLinearCombination);
    CPPUNIT_TEST(TestDeterminant3x3ByCrossProduct);
    CPPUNIT_TEST(TestDeterminant3x3ByLinearCombination);
    CPPUNIT_TEST_SUITE_END();

public:

    typedef double ElementType;

    /*! Compute the inverse of the 2x2 matrix using the closed-formula that includes the determinant.
      Test that the product of the input matrix and the computed inverse is the identity, by subtracting
      the identity from the product and comparing the Frobenius norm to zero.

      If the initial determinant of the input matrix is lower than our default tolerance, perform another
      test:  verify that the second row is a scaled version of the first row, and that the
      second column is a scaled version of the first column.
    */
    static void TestDeterminant2x2ByInverse(const vctFixedSizeMatrix<ElementType, 2, 2> & inputMatrix);
    void TestDeterminant2x2ByInverse()
    {
        TestDeterminant2x2ByInverse(Mat2x2);
    }

    /*!  Create a 2x2 orthogonal matrix from the input vector normalized.  
      Test that the determinant is one.  Also perform TestDeterminant2x2ByInverse.
    */
    static void TestDeterminant2x2ByOrthogonal(const vctFixedSizeVector<ElementType, 2> & inputVector);
    void TestDeterminant2x2ByOrthogonal()
    {
        TestDeterminant2x2ByOrthogonal(Vec2D);
    }

    /*!  Create a 2x2 matrix with the input vector and its double.  Test that the determinant is zero.
      Also perform TestDeterminant2x2ByInverse.
    */
    static void TestDeterminant2x2ByLinearCombination(const vctFixedSizeVector<ElementType, 2> & row0);
    void TestDeterminant2x2ByLinearCombination()
    {
        TestDeterminant2x2ByLinearCombination(Vec2D);
    }

    /*! Create a 3x3 matrix that includes the two input vectors and their cross 
      product.  If the two input vectors are linearly dependent, the cross product is zero,
      and then test that the determinant is zero.  Otherwise, test that the determinant is nonzero.
    */
    static void TestDeterminant3x3ByCrossProduct(const vctFixedSizeVector<ElementType, 3> & row0,
        const vctFixedSizeVector<ElementType, 3> & row1);
    /*!  Run TestDeterminant3x3ByCrossProduct for two random vectors and then for two cases
      of linearly dependent vector pairs. */
    void TestDeterminant3x3ByCrossProduct()
    {
        TestDeterminant3x3ByCrossProduct(Vec3D[0], Vec3D[1]);
        TestDeterminant3x3ByCrossProduct(Vec3D[0], 2.0*Vec3D[0]);
        TestDeterminant3x3ByCrossProduct(Vec3D[1], -Vec3D[1]);
    }

    /*!  Create a 3x3 matrix that includes the two input vectors and their sum.  Test that the determinant
      is zero.
    */
    static void TestDeterminant3x3ByLinearCombination(const vctFixedSizeVector<ElementType, 3> & row0,
        const vctFixedSizeVector<ElementType, 3> & row1);
    void TestDeterminant3x3ByLinearCombination()
    {
        TestDeterminant3x3ByLinearCombination(Vec3D[0], Vec3D[1]);
    }

    virtual void setUp(void);

    virtual void tearDown(void)
    {}

    vctDeterminantTest();

private:
    vctFixedSizeVector<ElementType, 2> Vec2D;
    vctFixedSizeMatrix<ElementType, 2, 2> Mat2x2;
    vctFixedSizeVector< vctFixedSizeVector<ElementType, 3>, 2 > Vec3D;

    cmnRandomSequence & RandomSequence;
};

#endif  // _vctDeterminantTest_h


