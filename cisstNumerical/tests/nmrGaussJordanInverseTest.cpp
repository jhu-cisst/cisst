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

#include "nmrGaussJordanInverseTest.h"

#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctRandomFixedSizeVector.h>
#include <cisstVector/vctRandomFixedSizeMatrix.h>


const double nmrGaussJordanInverseTest::ToleranceScale = 50;
const double nmrGaussJordanInverseTest::RandomElementRange = 10;

template <class _elementType, vct::size_type _size, bool _rowMajor1, bool _rowMajor2>
void nmrGaussJordanInverseTest::TestMatrixInverse(
    const vctFixedSizeMatrix<_elementType, _size, _size, _rowMajor1> & m1,
    const vctFixedSizeMatrix<_elementType, _size, _size, _rowMajor2> & m2,
    const _elementType tolerance)
{
    typedef _elementType value_type;
    static bool identityInitialized = false;
    static vctFixedSizeMatrix<_elementType, _size, _size, VCT_ROW_MAJOR> identity( value_type(0) );
    if (!identityInitialized) {
        identity.Diagonal().SetAll( value_type(1) );
        identityInitialized = true;
    }

    vctFixedSizeMatrix<value_type, _size, _size, VCT_ROW_MAJOR> result, difference;
    value_type maxDiff;
    result.ProductOf(m1, m2);
    difference.DifferenceOf(result, identity);
    maxDiff = difference.LinfNorm();
    CPPUNIT_ASSERT(maxDiff < tolerance);

    result.ProductOf(m2, m1);
    difference.DifferenceOf(result, identity);
    maxDiff = difference.LinfNorm();
    CPPUNIT_ASSERT(maxDiff < tolerance);
}


template <class _elementType, vct::size_type _size, bool _rowMajor>
void nmrGaussJordanInverseTest::MakeSingularMatrix(vctFixedSizeMatrix<_elementType, _size, _size, _rowMajor> & m)
{
    typedef _elementType value_type;
    vctFixedSizeVector<value_type, _size> combinationCoefficients;
    vctRandom(combinationCoefficients, value_type(0), value_type(1));
    vct::size_type selectedRow = cmnRandomSequence::GetInstance().ExtractRandomUnsignedInt(0, _size);
    combinationCoefficients[selectedRow] = value_type(0);
    vctFixedSizeVector<value_type, _size> combination = combinationCoefficients * m;
    m.Row(selectedRow).Assign( combination );
}

template <class _elementType>
void nmrGaussJordanInverseTest::TestInverse4x4(void)
{
    typedef _elementType value_type;
    enum {SIZE = 4};
    vctFixedSizeMatrix<value_type, SIZE, SIZE, VCT_ROW_MAJOR> A, Ainv, Ainvinv, Adiff;
    vctRandom(A, value_type(-RandomElementRange), value_type(RandomElementRange));
    value_type tolerance = value_type(ToleranceScale) * cmnTypeTraits<value_type>::Tolerance();
    bool nonsingular;

    nmrGaussJordanInverse4x4(A, nonsingular, Ainv, tolerance);

    if (!nonsingular)
        return;

    TestMatrixInverse(A, Ainv, tolerance);

    nmrGaussJordanInverse4x4(Ainv, nonsingular, Ainvinv, tolerance);

    CPPUNIT_ASSERT(nonsingular);

    Adiff.DifferenceOf(A, Ainvinv);
    value_type maxDiff = Adiff.LinfNorm();
    CPPUNIT_ASSERT(maxDiff < tolerance);

}

template <class _elementType>
void nmrGaussJordanInverseTest::TestInverse3x3(void)
{
    typedef _elementType value_type;
    enum {SIZE = 3};
    vctFixedSizeMatrix<value_type, SIZE, SIZE, VCT_ROW_MAJOR> A, Ainv, Ainvinv, Adiff;
    vctRandom(A, value_type(-RandomElementRange), value_type(RandomElementRange));
    value_type tolerance = value_type(ToleranceScale) * cmnTypeTraits<value_type>::Tolerance();
    bool nonsingular;

    nmrGaussJordanInverse3x3(A, nonsingular, Ainv, tolerance);

    if (!nonsingular)
        return;

    TestMatrixInverse(A, Ainv, tolerance);

    nmrGaussJordanInverse3x3(Ainv, nonsingular, Ainvinv, tolerance);

    CPPUNIT_ASSERT(nonsingular);

    Adiff.DifferenceOf(A, Ainvinv);
    value_type maxDiff = Adiff.LinfNorm();
    CPPUNIT_ASSERT(maxDiff < tolerance);

}


template <class _elementType>
void nmrGaussJordanInverseTest::TestInverse2x2(void)
{
    typedef _elementType value_type;
    enum {SIZE = 2};
    vctFixedSizeMatrix<value_type, SIZE, SIZE, VCT_ROW_MAJOR> A, Ainv, Ainvinv, Adiff;
    vctRandom(A, value_type(-RandomElementRange), value_type(RandomElementRange));
    value_type tolerance = value_type(ToleranceScale) * cmnTypeTraits<value_type>::Tolerance();
    bool nonsingular;

    nmrGaussJordanInverse2x2(A, nonsingular, Ainv, tolerance);

    if (!nonsingular)
        return;

    TestMatrixInverse(A, Ainv, tolerance);

    nmrGaussJordanInverse2x2(Ainv, nonsingular, Ainvinv, tolerance);

    CPPUNIT_ASSERT(nonsingular);

    Adiff.DifferenceOf(A, Ainvinv);
    value_type maxDiff = Adiff.LinfNorm();
    CPPUNIT_ASSERT(maxDiff < tolerance);

}

template <class _elementType>
void nmrGaussJordanInverseTest::TestSingular4x4(void)
{
    typedef _elementType value_type;
    enum {SIZE = 4};
    vctFixedSizeMatrix<value_type, SIZE, SIZE, VCT_ROW_MAJOR> A, Ainv;
    vctRandom(A, value_type(-RandomElementRange), value_type(RandomElementRange));
    MakeSingularMatrix(A);
    value_type tolerance = value_type(ToleranceScale) * cmnTypeTraits<value_type>::Tolerance();
    bool nonsingular;

    nmrGaussJordanInverse4x4(A, nonsingular, Ainv, tolerance);

    CPPUNIT_ASSERT(!nonsingular);

    A.Diagonal().Add( value_type(1) );

    nmrGaussJordanInverse4x4(A, nonsingular, Ainv, tolerance);

    CPPUNIT_ASSERT(nonsingular);
}


template <class _elementType>
void nmrGaussJordanInverseTest::TestSingular3x3(void)
{
    typedef _elementType value_type;
    enum {SIZE = 3};
    vctFixedSizeMatrix<value_type, SIZE, SIZE, VCT_ROW_MAJOR> A, Ainv;
    vctRandom(A, value_type(-RandomElementRange), value_type(RandomElementRange));
    MakeSingularMatrix(A);
    value_type tolerance = value_type(ToleranceScale) * cmnTypeTraits<value_type>::Tolerance();
    bool nonsingular;

    nmrGaussJordanInverse3x3(A, nonsingular, Ainv, tolerance);

    CPPUNIT_ASSERT(!nonsingular);

    A.Diagonal().Add( value_type(1) );

    nmrGaussJordanInverse3x3(A, nonsingular, Ainv, tolerance);

    CPPUNIT_ASSERT(nonsingular);
}

template <class _elementType>
void nmrGaussJordanInverseTest::TestSingular2x2(void)
{
    typedef _elementType value_type;
    enum {SIZE = 2};
    vctFixedSizeMatrix<value_type, SIZE, SIZE, VCT_ROW_MAJOR> A, Ainv;
    vctRandom(A, value_type(-RandomElementRange), value_type(RandomElementRange));
    MakeSingularMatrix(A);
    value_type tolerance = value_type(ToleranceScale) * cmnTypeTraits<value_type>::Tolerance();
    bool nonsingular;

    nmrGaussJordanInverse2x2(A, nonsingular, Ainv, tolerance);

    CPPUNIT_ASSERT(!nonsingular);

    A.Diagonal().Add( value_type(1) );

    nmrGaussJordanInverse2x2(A, nonsingular, Ainv, tolerance);

    CPPUNIT_ASSERT(nonsingular);
}


void nmrGaussJordanInverseTest::TestInverse4x4Double(void)
{
    TestInverse4x4<double>();
}

void nmrGaussJordanInverseTest::TestInverse4x4Float()
{
    TestInverse4x4<float>();
}

void nmrGaussJordanInverseTest::TestInverse3x3Double(void)
{
    TestInverse3x3<double>();
}

void nmrGaussJordanInverseTest::TestInverse3x3Float()
{
    TestInverse3x3<float>();
}
void nmrGaussJordanInverseTest::TestInverse2x2Double(void)
{
    TestInverse2x2<double>();
}

void nmrGaussJordanInverseTest::TestInverse2x2Float()
{
    TestInverse2x2<float>();
}

void nmrGaussJordanInverseTest::TestSingular4x4Double(void)
{
    TestSingular4x4<double>();
}

void nmrGaussJordanInverseTest::TestSingular4x4Float(void)
{
    TestSingular4x4<float>();
}

void nmrGaussJordanInverseTest::TestSingular3x3Double(void)
{
    TestSingular3x3<double>();
}

void nmrGaussJordanInverseTest::TestSingular3x3Float(void)
{
    TestSingular3x3<float>();
}

void nmrGaussJordanInverseTest::TestSingular2x2Double(void)
{
    TestSingular2x2<double>();
}

void nmrGaussJordanInverseTest::TestSingular2x2Float(void)
{
    TestSingular2x2<float>();
}

CPPUNIT_TEST_SUITE_REGISTRATION(nmrGaussJordanInverseTest);

