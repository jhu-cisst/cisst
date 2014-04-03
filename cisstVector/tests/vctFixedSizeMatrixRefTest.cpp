/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2004-11-17
  
  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "vctFixedSizeMatrixRefTest.h"
#include "vctGenericContainerTest.h"
#include "vctGenericMatrixTest.h"


#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctFixedSizeMatrixRef.h>
#include <cisstVector/vctFixedSizeConstMatrixRef.h>
#include <cisstVector/vctRandomFixedSizeVector.h>
#include <cisstVector/vctRandomFixedSizeMatrix.h>


// this macro creates a matrix##number by reference and the
// associatied storage based on SIZE and STRIDE##number
#define CREATE_STORAGE_AND_REF(number) \
    typedef vctFixedSizeMatrix<value_type, 6, 11> StorageType; \
    StorageType storage##number; \
    typedef vctFixedSizeMatrixRef<value_type, 3, 5, StorageType::MatrixTraits::ROWSTRIDE * 2, 2> MatrixType; \
    MatrixType matrix##number; \
    matrix##number.SetRef(storage##number.Pointer())



template <class _elementType>
void vctFixedSizeMatrixRefTest::TestAssignment(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctGenericMatrixTest::TestAssignment(matrix1, matrix2);
}

void vctFixedSizeMatrixRefTest::TestAssignmentDouble(void) {
    TestAssignment<double>();
}
void vctFixedSizeMatrixRefTest::TestAssignmentFloat(void) {
    TestAssignment<float>();
}
void vctFixedSizeMatrixRefTest::TestAssignmentInt(void) {
    TestAssignment<int>();
}



template <class _elementType>
void vctFixedSizeMatrixRefTest::TestAccessMethods(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    vctRandom(matrix1, value_type(-10), value_type(10));
    typename MatrixType::ConstRefType matrix2(matrix1.Pointer());
    vctGenericMatrixTest::TestAccessMethods(matrix1, matrix2);
}

void vctFixedSizeMatrixRefTest::TestAccessMethodsDouble(void) {
    TestAccessMethods<double>();
}
void vctFixedSizeMatrixRefTest::TestAccessMethodsFloat(void) {
    TestAccessMethods<float>();
}
void vctFixedSizeMatrixRefTest::TestAccessMethodsInt(void) {
    TestAccessMethods<int>();
}


template <class _elementType>
void vctFixedSizeMatrixRefTest::TestSoMiOperations(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSoCiOperations(matrix1, matrix2);
}

void vctFixedSizeMatrixRefTest::TestSoMiOperationsDouble(void) {
    TestSoMiOperations<double>();
}
void vctFixedSizeMatrixRefTest::TestSoMiOperationsFloat(void) {
    TestSoMiOperations<float>();
}
void vctFixedSizeMatrixRefTest::TestSoMiOperationsInt(void) {
    TestSoMiOperations<int>();
}



template <class _elementType>
void vctFixedSizeMatrixRefTest::TestSoMiMiOperations(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    CREATE_STORAGE_AND_REF(3);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctRandom(matrix2, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSoCiCiOperations(matrix1, matrix2, matrix3);
}

void vctFixedSizeMatrixRefTest::TestSoMiMiOperationsDouble(void) {
    TestSoMiMiOperations<double>();
}
void vctFixedSizeMatrixRefTest::TestSoMiMiOperationsFloat(void) {
    TestSoMiMiOperations<float>();
}
void vctFixedSizeMatrixRefTest::TestSoMiMiOperationsInt(void) {
    TestSoMiMiOperations<int>();
}



template <class _elementType>
void vctFixedSizeMatrixRefTest::TestMioMiOperations(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    CREATE_STORAGE_AND_REF(3);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctRandom(matrix2, value_type(-10), value_type(10));
    RemoveQuasiZero(matrix2);
    vctGenericContainerTest::TestCioCiOperations(matrix1, matrix2, matrix3);
}

void vctFixedSizeMatrixRefTest::TestMioMiOperationsDouble(void) {
    TestMioMiOperations<double>();
}
void vctFixedSizeMatrixRefTest::TestMioMiOperationsFloat(void) {
    TestMioMiOperations<float>();
}
void vctFixedSizeMatrixRefTest::TestMioMiOperationsInt(void) {
    TestMioMiOperations<int>();
}



template <class _elementType>
void vctFixedSizeMatrixRefTest::TestMoMiMiOperations(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    CREATE_STORAGE_AND_REF(3);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctRandom(matrix2, value_type(-10), value_type(10));
    RemoveQuasiZero(matrix2);
    vctGenericContainerTest::TestCoCiCiOperations(matrix1, matrix2, matrix3);
}

void vctFixedSizeMatrixRefTest::TestMoMiMiOperationsDouble(void) {
    TestMoMiMiOperations<double>();
}
void vctFixedSizeMatrixRefTest::TestMoMiMiOperationsFloat(void) {
    TestMoMiMiOperations<float>();
}
void vctFixedSizeMatrixRefTest::TestMoMiMiOperationsInt(void) {
    TestMoMiMiOperations<int>();
}



template <class _elementType>
void vctFixedSizeMatrixRefTest::TestProductOperations(void) {
    // result size
    enum {ROWS = 4, COLS = 5, COMSIZE = 7};
    typedef _elementType value_type;
    vctFixedSizeMatrix<value_type, ROWS, COMSIZE> matrix1;
    vctFixedSizeMatrix<value_type, COMSIZE, COLS> matrix2;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix3;
    vctFixedSizeVector<value_type, COMSIZE> vector1;
    vctFixedSizeVector<value_type, ROWS> vector2;
    
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctRandom(matrix2, value_type(-10), value_type(10));
    vctRandom(vector1, value_type(-10), value_type(10));

    vctGenericMatrixTest::TestMatrixMatrixProductOperations(matrix1, matrix2, matrix3);
    vctGenericMatrixTest::TestMatrixVectorProductOperations(matrix1, vector1, vector2);

    vctRandom(vector2, value_type(-10), value_type(10));
    vctGenericMatrixTest::TestVectorMatrixProductOperations(matrix1, vector2, vector1);
}

void vctFixedSizeMatrixRefTest::TestProductOperationsDouble(void) {
    TestProductOperations<double>();
}
void vctFixedSizeMatrixRefTest::TestProductOperationsFloat(void) {
    TestProductOperations<float>();
}
void vctFixedSizeMatrixRefTest::TestProductOperationsInt(void) {
    TestProductOperations<int>();
}



template <class _elementType>
void vctFixedSizeMatrixRefTest::TestMoMiOperations(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCoCiOperations(matrix1, matrix2);
}

void vctFixedSizeMatrixRefTest::TestMoMiOperationsDouble(void) {
    TestMoMiOperations<double>();
}
void vctFixedSizeMatrixRefTest::TestMoMiOperationsFloat(void) {
    TestMoMiOperations<float>();
}
void vctFixedSizeMatrixRefTest::TestMoMiOperationsInt(void) {
    TestMoMiOperations<int>();
}



template <class _elementType>
void vctFixedSizeMatrixRefTest::TestMoMiSiOperations(void) {
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

void vctFixedSizeMatrixRefTest::TestMoMiSiOperationsDouble(void) {
    TestMoMiSiOperations<double>();
}
void vctFixedSizeMatrixRefTest::TestMoMiSiOperationsFloat(void) {
    TestMoMiSiOperations<float>();
}
void vctFixedSizeMatrixRefTest::TestMoMiSiOperationsInt(void) {
    TestMoMiSiOperations<int>();
}




template <class _elementType>
void vctFixedSizeMatrixRefTest::TestMoSiMiOperations(void) {
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

void vctFixedSizeMatrixRefTest::TestMoSiMiOperationsDouble(void) {
    TestMoSiMiOperations<double>();
}
void vctFixedSizeMatrixRefTest::TestMoSiMiOperationsFloat(void) {
    TestMoSiMiOperations<float>();
}
void vctFixedSizeMatrixRefTest::TestMoSiMiOperationsInt(void) {
    TestMoSiMiOperations<int>();
}



template <class _elementType>
void vctFixedSizeMatrixRefTest::TestMioSiOperations(void) {
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

void vctFixedSizeMatrixRefTest::TestMioSiOperationsDouble(void) {
    TestMioSiOperations<double>();
}
void vctFixedSizeMatrixRefTest::TestMioSiOperationsFloat(void) {
    TestMioSiOperations<float>();
}
void vctFixedSizeMatrixRefTest::TestMioSiOperationsInt(void) {
    TestMioSiOperations<int>();
}



template <class _elementType>
void vctFixedSizeMatrixRefTest::TestMioOperations(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCioOperations(matrix1, matrix2);
}

void vctFixedSizeMatrixRefTest::TestMioOperationsDouble(void) {
    TestMioOperations<double>();
}
void vctFixedSizeMatrixRefTest::TestMioOperationsFloat(void) {
    TestMioOperations<float>();
}
void vctFixedSizeMatrixRefTest::TestMioOperationsInt(void) {
    TestMioOperations<int>();
}



template <class _elementType>
void vctFixedSizeMatrixRefTest::TestSoMiSiOperations(void) {
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

void vctFixedSizeMatrixRefTest::TestSoMiSiOperationsDouble(void) {
    TestSoMiSiOperations<double>();
}
void vctFixedSizeMatrixRefTest::TestSoMiSiOperationsFloat(void) {
    TestSoMiSiOperations<float>();
}
void vctFixedSizeMatrixRefTest::TestSoMiSiOperationsInt(void) {
    TestSoMiSiOperations<int>();
}



template <class _elementType>
void vctFixedSizeMatrixRefTest::TestIterators(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestIterators(matrix1);
}

void vctFixedSizeMatrixRefTest::TestIteratorsDouble(void) {
    TestIterators<double>();
}
void vctFixedSizeMatrixRefTest::TestIteratorsFloat(void) {
    TestIterators<float>();
}
void vctFixedSizeMatrixRefTest::TestIteratorsInt(void) {
    TestIterators<int>();
}


template <class _elementType>
void vctFixedSizeMatrixRefTest::TestSTLFunctions(void) {
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSTLFunctions(matrix1, matrix2);
}

void vctFixedSizeMatrixRefTest::TestSTLFunctionsDouble(void) {
    TestSTLFunctions<double>();
}
void vctFixedSizeMatrixRefTest::TestSTLFunctionsFloat(void) {
    TestSTLFunctions<float>();
}
void vctFixedSizeMatrixRefTest::TestSTLFunctionsInt(void) {
    TestSTLFunctions<int>();
}


CPPUNIT_TEST_SUITE_REGISTRATION(vctFixedSizeMatrixRefTest);

