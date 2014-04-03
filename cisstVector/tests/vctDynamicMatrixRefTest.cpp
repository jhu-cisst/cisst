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


#include "vctDynamicMatrixRefTest.h"
#include "vctGenericContainerTest.h"
#include "vctGenericMatrixTest.h"


#include <cisstVector/vctDynamicMatrix.h>
#include <cisstVector/vctDynamicMatrixRef.h>
#include <cisstVector/vctDynamicConstMatrixRef.h>
#include <cisstVector/vctRandomDynamicVector.h>
#include <cisstVector/vctRandomDynamicMatrix.h>


// this macro creates a matrix##number by reference and the
// associatied storage based on SIZE and STRIDE##number
#define CREATE_STORAGE_AND_REF(number) \
    vctDynamicMatrix<value_type> storage##number(6, 11); \
    vctDynamicMatrixRef<value_type> matrix##number; \
    matrix##number.SetRef(3, 5, storage##number.row_stride() * 2, 2, storage##number.Pointer())


template <class _elementType>
void vctDynamicMatrixRefTest::TestAssignment(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctGenericMatrixTest::TestAssignment(matrix1, matrix2);
}

void vctDynamicMatrixRefTest::TestAssignmentDouble(void) {
    TestAssignment<double>();
}
void vctDynamicMatrixRefTest::TestAssignmentFloat(void) {
    TestAssignment<float>();
}
void vctDynamicMatrixRefTest::TestAssignmentInt(void) {
    TestAssignment<int>();
}



template <class _elementType>
void vctDynamicMatrixRefTest::TestAccessMethods(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctDynamicConstMatrixRef<value_type> matrix2(matrix1.rows(), matrix1.cols(),
                                                 matrix1.row_stride(), matrix1.col_stride(),
                                                 matrix1.Pointer());
    vctGenericMatrixTest::TestAccessMethods(matrix1, matrix2);
}

void vctDynamicMatrixRefTest::TestAccessMethodsDouble(void) {
    TestAccessMethods<double>();
}
void vctDynamicMatrixRefTest::TestAccessMethodsFloat(void) {
    TestAccessMethods<float>();
}
void vctDynamicMatrixRefTest::TestAccessMethodsInt(void) {
    TestAccessMethods<int>();
}


template <class _elementType>
void vctDynamicMatrixRefTest::TestSoMiOperations(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSoCiOperations(matrix1, matrix2);
}

void vctDynamicMatrixRefTest::TestSoMiOperationsDouble(void) {
    TestSoMiOperations<double>();
}
void vctDynamicMatrixRefTest::TestSoMiOperationsFloat(void) {
    TestSoMiOperations<float>();
}
void vctDynamicMatrixRefTest::TestSoMiOperationsInt(void) {
    TestSoMiOperations<int>();
}



template <class _elementType>
void vctDynamicMatrixRefTest::TestSoMiMiOperations(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    CREATE_STORAGE_AND_REF(3);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctRandom(matrix2, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSoCiCiOperations(matrix1, matrix2, matrix3);
}

void vctDynamicMatrixRefTest::TestSoMiMiOperationsDouble(void) {
    TestSoMiMiOperations<double>();
}
void vctDynamicMatrixRefTest::TestSoMiMiOperationsFloat(void) {
    TestSoMiMiOperations<float>();
}
void vctDynamicMatrixRefTest::TestSoMiMiOperationsInt(void) {
    TestSoMiMiOperations<int>();
}



template <class _elementType>
void vctDynamicMatrixRefTest::TestMioMiOperations(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    CREATE_STORAGE_AND_REF(3);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctRandom(matrix2, value_type(-10), value_type(10));
    RemoveQuasiZero(matrix2);
    vctGenericContainerTest::TestCioCiOperations(matrix1, matrix2, matrix3);
}

void vctDynamicMatrixRefTest::TestMioMiOperationsDouble(void) {
    TestMioMiOperations<double>();
}
void vctDynamicMatrixRefTest::TestMioMiOperationsFloat(void) {
    TestMioMiOperations<float>();
}
void vctDynamicMatrixRefTest::TestMioMiOperationsInt(void) {
    TestMioMiOperations<int>();
}


template <class _elementType>
void vctDynamicMatrixRefTest::TestMioSiMiOperations(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    CREATE_STORAGE_AND_REF(3);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctRandom(matrix2, value_type(-10), value_type(10));
    RemoveQuasiZero(matrix2);

    value_type scalar;
    vctRandom(matrix1, value_type(-10), value_type(10));
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-10),
                                      value_type(10),
                                      scalar);

    vctGenericContainerTest::TestCioSiCiOperations(matrix1, scalar, matrix2, matrix3);
}

void vctDynamicMatrixRefTest::TestMioSiMiOperationsDouble(void) {
    TestMioSiMiOperations<double>();
}
void vctDynamicMatrixRefTest::TestMioSiMiOperationsFloat(void) {
    TestMioSiMiOperations<float>();
}
void vctDynamicMatrixRefTest::TestMioSiMiOperationsInt(void) {
    TestMioSiMiOperations<int>();
}



template <class _elementType>
void vctDynamicMatrixRefTest::TestMioMiMiOperations(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    CREATE_STORAGE_AND_REF(3);
    CREATE_STORAGE_AND_REF(4);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctRandom(matrix2, value_type(-10), value_type(10));
    vctRandom(matrix3, value_type(-10), value_type(10));
    RemoveQuasiZero(matrix3);

    vctGenericContainerTest::TestCioCiCiOperations(matrix1, matrix2, matrix3, matrix4);
}

void vctDynamicMatrixRefTest::TestMioMiMiOperationsDouble(void) {
    TestMioMiMiOperations<double>();
}
void vctDynamicMatrixRefTest::TestMioMiMiOperationsFloat(void) {
    TestMioMiMiOperations<float>();
}
void vctDynamicMatrixRefTest::TestMioMiMiOperationsInt(void) {
    TestMioMiMiOperations<int>();
}



template <class _elementType>
void vctDynamicMatrixRefTest::TestMoMiMiOperations(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    CREATE_STORAGE_AND_REF(3);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctRandom(matrix2, value_type(-10), value_type(10));
    RemoveQuasiZero(matrix2);
    vctGenericContainerTest::TestCoCiCiOperations(matrix1, matrix2, matrix3);
}

void vctDynamicMatrixRefTest::TestMoMiMiOperationsDouble(void) {
    TestMoMiMiOperations<double>();
}
void vctDynamicMatrixRefTest::TestMoMiMiOperationsFloat(void) {
    TestMoMiMiOperations<float>();
}
void vctDynamicMatrixRefTest::TestMoMiMiOperationsInt(void) {
    TestMoMiMiOperations<int>();
}



template <class _elementType>
void vctDynamicMatrixRefTest::TestProductOperations(void) {
    // result size
    enum {ROWS = 4, COLS = 5, COMSIZE = 7};
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> matrix1(ROWS, COMSIZE);
    vctDynamicMatrix<value_type> matrix2(COMSIZE, COLS);
    vctDynamicMatrix<value_type> matrix3(ROWS, COLS);
    vctDynamicVector<value_type> vector1(COMSIZE);
    vctDynamicVector<value_type> vector2(ROWS);

    vctRandom(matrix1, value_type(-10), value_type(10));
    vctRandom(matrix2, value_type(-10), value_type(10));
    vctRandom(vector1, value_type(-10), value_type(10));

    vctGenericMatrixTest::TestMatrixMatrixProductOperations(matrix1, matrix2, matrix3);
    vctGenericMatrixTest::TestMatrixVectorProductOperations(matrix1, vector1, vector2);

    vctRandom(vector2, value_type(-10), value_type(10));
    vctGenericMatrixTest::TestVectorMatrixProductOperations(matrix1, vector2, vector1);
}

void vctDynamicMatrixRefTest::TestProductOperationsDouble(void) {
    TestProductOperations<double>();
}
void vctDynamicMatrixRefTest::TestProductOperationsFloat(void) {
    TestProductOperations<float>();
}
void vctDynamicMatrixRefTest::TestProductOperationsInt(void) {
    TestProductOperations<int>();
}



template <class _elementType>
void vctDynamicMatrixRefTest::TestMoMiOperations(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCoCiOperations(matrix1, matrix2);
}

void vctDynamicMatrixRefTest::TestMoMiOperationsDouble(void) {
    TestMoMiOperations<double>();
}
void vctDynamicMatrixRefTest::TestMoMiOperationsFloat(void) {
    TestMoMiOperations<float>();
}
void vctDynamicMatrixRefTest::TestMoMiOperationsInt(void) {
    TestMoMiOperations<int>();
}



template <class _elementType>
void vctDynamicMatrixRefTest::TestMoMiSiOperations(void) {
    typedef _elementType value_type;
    value_type scalar;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5),
                                      value_type(5),
                                      scalar);
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    if ((scalar < value_type(1))
        && (scalar > -value_type(1))) {
        scalar = value_type(3);
    }
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCoCiSiOperations(matrix1, scalar, matrix2);
}

void vctDynamicMatrixRefTest::TestMoMiSiOperationsDouble(void) {
    TestMoMiSiOperations<double>();
}
void vctDynamicMatrixRefTest::TestMoMiSiOperationsFloat(void) {
    TestMoMiSiOperations<float>();
}
void vctDynamicMatrixRefTest::TestMoMiSiOperationsInt(void) {
    TestMoMiSiOperations<int>();
}




template <class _elementType>
void vctDynamicMatrixRefTest::TestMoSiMiOperations(void) {
    typedef _elementType value_type;
    value_type scalar;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5),
                                      value_type(5),
                                       scalar);
    vctRandom(matrix1, value_type(-10), value_type(10));
    RemoveQuasiZero(matrix1);
    vctGenericContainerTest::TestCoSiCiOperations(scalar, matrix1, matrix2);
}

void vctDynamicMatrixRefTest::TestMoSiMiOperationsDouble(void) {
    TestMoSiMiOperations<double>();
}
void vctDynamicMatrixRefTest::TestMoSiMiOperationsFloat(void) {
    TestMoSiMiOperations<float>();
}
void vctDynamicMatrixRefTest::TestMoSiMiOperationsInt(void) {
    TestMoSiMiOperations<int>();
}



template <class _elementType>
void vctDynamicMatrixRefTest::TestMioSiOperations(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    value_type scalar;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5),
                                      value_type(5),
                                      scalar);
    if ((scalar < value_type(1))
        && (scalar > -value_type(1))) {
        scalar = value_type(3);
    }
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCioSiOperations(matrix1, scalar, matrix2);
}

void vctDynamicMatrixRefTest::TestMioSiOperationsDouble(void) {
    TestMioSiOperations<double>();
}
void vctDynamicMatrixRefTest::TestMioSiOperationsFloat(void) {
    TestMioSiOperations<float>();
}
void vctDynamicMatrixRefTest::TestMioSiOperationsInt(void) {
    TestMioSiOperations<int>();
}



template <class _elementType>
void vctDynamicMatrixRefTest::TestMioOperations(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCioOperations(matrix1, matrix2);
}

void vctDynamicMatrixRefTest::TestMioOperationsDouble(void) {
    TestMioOperations<double>();
}
void vctDynamicMatrixRefTest::TestMioOperationsFloat(void) {
    TestMioOperations<float>();
}
void vctDynamicMatrixRefTest::TestMioOperationsInt(void) {
    TestMioOperations<int>();
}



template <class _elementType>
void vctDynamicMatrixRefTest::TestSoMiSiOperations(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    value_type scalar;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5),
                                      value_type(5),
                                      scalar);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSoCiSiOperations(matrix1, scalar, matrix2);
}

void vctDynamicMatrixRefTest::TestSoMiSiOperationsDouble(void) {
    TestSoMiSiOperations<double>();
}
void vctDynamicMatrixRefTest::TestSoMiSiOperationsFloat(void) {
    TestSoMiSiOperations<float>();
}
void vctDynamicMatrixRefTest::TestSoMiSiOperationsInt(void) {
    TestSoMiSiOperations<int>();
}



template<class _elementType>
void vctDynamicMatrixRefTest::TestMinAndMax(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    vctRandom(matrix1, value_type(-50), value_type(50));
    vctGenericContainerTest::TestMinAndMax(matrix1);
}

void vctDynamicMatrixRefTest::TestMinAndMaxDouble(void) {
    TestMinAndMax<double>();
}

void vctDynamicMatrixRefTest::TestMinAndMaxFloat(void) {
    TestMinAndMax<float>();
}

void vctDynamicMatrixRefTest::TestMinAndMaxInt(void) {
    TestMinAndMax<int>();
}



template <class _elementType>
void vctDynamicMatrixRefTest::TestIterators(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestIterators(matrix1);
}

void vctDynamicMatrixRefTest::TestIteratorsDouble(void) {
    TestIterators<double>();
}
void vctDynamicMatrixRefTest::TestIteratorsFloat(void) {
    TestIterators<float>();
}
void vctDynamicMatrixRefTest::TestIteratorsInt(void) {
    TestIterators<int>();
}



template <class _elementType>
void vctDynamicMatrixRefTest::TestSTLFunctions(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSTLFunctions(matrix1, matrix2);
}

void vctDynamicMatrixRefTest::TestSTLFunctionsDouble(void) {
    TestSTLFunctions<double>();
}
void vctDynamicMatrixRefTest::TestSTLFunctionsFloat(void) {
    TestSTLFunctions<float>();
}
void vctDynamicMatrixRefTest::TestSTLFunctionsInt(void) {
    TestSTLFunctions<int>();
}



CPPUNIT_TEST_SUITE_REGISTRATION(vctDynamicMatrixRefTest);

