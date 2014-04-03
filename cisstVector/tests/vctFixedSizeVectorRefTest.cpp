/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2003-08-20
  
  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "vctFixedSizeVectorRefTest.h"
#include "vctGenericContainerTest.h"
#include "vctGenericVectorTest.h"


#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctFixedSizeConstVectorRef.h>
#include <cisstVector/vctRandomFixedSizeVector.h>


// this macro creates a vector##number by reference and the
// associatied storage based on SIZE and STRIDE##number
#define CREATE_STORAGE_AND_REF(number) \
    value_type storage##number[SIZE * STRIDE##number]; \
    vctFixedSizeVectorRef<value_type, SIZE, STRIDE##number> vector##number; \
    vector##number.SetRef(storage##number)


template <class _elementType>
void vctFixedSizeVectorRefTest::TestAssignment(void) {
    enum {SIZE = 7, STRIDE1 = 3, STRIDE2 = 2};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctGenericVectorTest::TestAssignment(vector1, vector2);
}

void vctFixedSizeVectorRefTest::TestAssignmentDouble(void) {
    TestAssignment<double>();
}
void vctFixedSizeVectorRefTest::TestAssignmentFloat(void) {
    TestAssignment<float>();
}
void vctFixedSizeVectorRefTest::TestAssignmentInt(void) {
    TestAssignment<int>();
}


template <class _elementType>
void vctFixedSizeVectorRefTest::TestAccessMethods(void) {
    enum {SIZE = 7, STRIDE1 = 4};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctFixedSizeConstVectorRef<value_type, SIZE, STRIDE1> vector2(vector1.Pointer());
    vctGenericVectorTest::TestAccessMethods(vector1, vector2);
}

void vctFixedSizeVectorRefTest::TestAccessMethodsDouble(void) {
    TestAccessMethods<double>();
}
void vctFixedSizeVectorRefTest::TestAccessMethodsFloat(void) {
    TestAccessMethods<float>();
}
void vctFixedSizeVectorRefTest::TestAccessMethodsInt(void) {
    TestAccessMethods<int>();
}



template <class _elementType>
void vctFixedSizeVectorRefTest::TestSoViOperations(void) {
    enum {SIZE = 6, STRIDE1 = 2, STRIDE2 = 4};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSoCiOperations(vector1, vector2);
}

void vctFixedSizeVectorRefTest::TestSoViOperationsDouble(void) {
    TestSoViOperations<double>();
}
void vctFixedSizeVectorRefTest::TestSoViOperationsFloat(void) {
    TestSoViOperations<float>();
}
void vctFixedSizeVectorRefTest::TestSoViOperationsInt(void) {
    TestSoViOperations<int>();
}



template <class _elementType>
void vctFixedSizeVectorRefTest::TestSoViViOperations(void) {
    enum {SIZE = 5, STRIDE1 = 1, STRIDE2 = 2, STRIDE3 = 3};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    CREATE_STORAGE_AND_REF(3);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctRandom(vector2, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSoCiCiOperations(vector1, vector2, vector3);
}

void vctFixedSizeVectorRefTest::TestSoViViOperationsDouble(void) {
    TestSoViViOperations<double>();
}
void vctFixedSizeVectorRefTest::TestSoViViOperationsFloat(void) {
    TestSoViViOperations<float>();
}
void vctFixedSizeVectorRefTest::TestSoViViOperationsInt(void) {
    TestSoViViOperations<int>();
}



template <class _elementType>
void vctFixedSizeVectorRefTest::TestVioViOperations(void) {
    enum {SIZE = 8, STRIDE1 = 3, STRIDE2 = 2, STRIDE3 = 2};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    CREATE_STORAGE_AND_REF(3);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctRandom(vector2, value_type(-10), value_type(10));
    RemoveQuasiZero(vector2);

    vctGenericContainerTest::TestCioCiOperations(vector1, vector2, vector3);
}

void vctFixedSizeVectorRefTest::TestVioViOperationsDouble(void) {
    TestVioViOperations<double>();
}
void vctFixedSizeVectorRefTest::TestVioViOperationsFloat(void) {
    TestVioViOperations<float>();
}
void vctFixedSizeVectorRefTest::TestVioViOperationsInt(void) {
    TestVioViOperations<int>();
}



template <class _elementType>
void vctFixedSizeVectorRefTest::TestVoViViOperations(void) {
    enum {SIZE = 4, STRIDE1 = 4, STRIDE2 = 1, STRIDE3 = 2};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    CREATE_STORAGE_AND_REF(3);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctRandom(vector2, value_type(-10), value_type(10));
    RemoveQuasiZero(vector2);

    vctGenericContainerTest::TestCoCiCiOperations(vector1, vector2, vector3);
}

void vctFixedSizeVectorRefTest::TestVoViViOperationsDouble(void) {
    TestVoViViOperations<double>();
}
void vctFixedSizeVectorRefTest::TestVoViViOperationsFloat(void) {
    TestVoViViOperations<float>();
}
void vctFixedSizeVectorRefTest::TestVoViViOperationsInt(void) {
    TestVoViViOperations<int>();
}



template <class _elementType>
void vctFixedSizeVectorRefTest::TestCrossProduct(void) {
    enum {SIZE = 3, STRIDE1 = 3, STRIDE2 = 1, STRIDE3 = 2};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    CREATE_STORAGE_AND_REF(3);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctRandom(vector2, value_type(-10), value_type(10));

    vctGenericVectorTest::TestCrossProduct(vector1, vector2, vector3);
}

void vctFixedSizeVectorRefTest::TestCrossProductDouble(void) {
    TestCrossProduct<double>();
}
void vctFixedSizeVectorRefTest::TestCrossProductFloat(void) {
    TestCrossProduct<float>();
}
void vctFixedSizeVectorRefTest::TestCrossProductInt(void) {
    TestCrossProduct<int>();
}



template <class _elementType>
void vctFixedSizeVectorRefTest::TestDotProduct(void) {
    enum {SIZE = 8, STRIDE1 = 3, STRIDE2 = 1};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctRandom(vector2, value_type(-10), value_type(10));

    vctGenericVectorTest::TestDotProduct(vector1, vector2);
}

void vctFixedSizeVectorRefTest::TestDotProductDouble(void) {
    TestDotProduct<double>();
}
void vctFixedSizeVectorRefTest::TestDotProductFloat(void) {
    TestDotProduct<float>();
}
void vctFixedSizeVectorRefTest::TestDotProductInt(void) {
    TestDotProduct<int>();
}



template <class _elementType>
void vctFixedSizeVectorRefTest::TestVoViOperations(void) {
    enum {SIZE = 4, STRIDE1 = 1, STRIDE2 = 3};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCoCiOperations(vector1, vector2);
}

void vctFixedSizeVectorRefTest::TestVoViOperationsDouble(void) {
    TestVoViOperations<double>();
}
void vctFixedSizeVectorRefTest::TestVoViOperationsFloat(void) {
    TestVoViOperations<float>();
}
void vctFixedSizeVectorRefTest::TestVoViOperationsInt(void) {
    TestVoViOperations<int>();
}



template <class _elementType>
void vctFixedSizeVectorRefTest::TestVoViSiOperations(void) {
    enum {SIZE = 8, STRIDE1 = 3, STRIDE2 = 4};
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
    vctRandom(vector1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCoCiSiOperations(vector1, scalar, vector2);
}

void vctFixedSizeVectorRefTest::TestVoViSiOperationsDouble(void) {
    TestVoViSiOperations<double>();
}
void vctFixedSizeVectorRefTest::TestVoViSiOperationsFloat(void) {
    TestVoViSiOperations<float>();
}
void vctFixedSizeVectorRefTest::TestVoViSiOperationsInt(void) {
    TestVoViSiOperations<int>();
}




template <class _elementType>
void vctFixedSizeVectorRefTest::TestVoSiViOperations(void) {
    enum {SIZE = 10, STRIDE1 = 3, STRIDE2 = 1};
    typedef _elementType value_type;
    value_type scalar;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5),
                                      value_type(5),
                                       scalar);
    vctRandom(vector1, value_type(-10), value_type(10));
    RemoveQuasiZero(vector1);

    vctGenericContainerTest::TestCoSiCiOperations(scalar, vector1, vector2);
}

void vctFixedSizeVectorRefTest::TestVoSiViOperationsDouble(void) {
    TestVoSiViOperations<double>();
}
void vctFixedSizeVectorRefTest::TestVoSiViOperationsFloat(void) {
    TestVoSiViOperations<float>();
}
void vctFixedSizeVectorRefTest::TestVoSiViOperationsInt(void) {
    TestVoSiViOperations<int>();
}



template <class _elementType>
void vctFixedSizeVectorRefTest::TestVioSiOperations(void) {
    enum {SIZE = 9, STRIDE1 = 1, STRIDE2 = 2};
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
    vctRandom(vector1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCioSiOperations(vector1, scalar, vector2);
}

void vctFixedSizeVectorRefTest::TestVioSiOperationsDouble(void) {
    TestVioSiOperations<double>();
}
void vctFixedSizeVectorRefTest::TestVioSiOperationsFloat(void) {
    TestVioSiOperations<float>();
}
void vctFixedSizeVectorRefTest::TestVioSiOperationsInt(void) {
    TestVioSiOperations<int>();
}



template <class _elementType>
void vctFixedSizeVectorRefTest::TestVioOperations(void) {
    enum {SIZE = 9, STRIDE1 = 2, STRIDE2 = 4};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCioOperations(vector1, vector2);
}

void vctFixedSizeVectorRefTest::TestVioOperationsDouble(void) {
    TestVioOperations<double>();
}
void vctFixedSizeVectorRefTest::TestVioOperationsFloat(void) {
    TestVioOperations<float>();
}
void vctFixedSizeVectorRefTest::TestVioOperationsInt(void) {
    TestVioOperations<int>();
}



template <class _elementType>
void vctFixedSizeVectorRefTest::TestSoViSiOperations(void) {
    enum {SIZE = 9, STRIDE1 = 1, STRIDE2 = 2};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    value_type scalar;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5),
                                      value_type(5),
                                      scalar);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSoCiSiOperations(vector1, scalar, vector2);
}

void vctFixedSizeVectorRefTest::TestSoViSiOperationsDouble(void) {
    TestSoViSiOperations<double>();
}
void vctFixedSizeVectorRefTest::TestSoViSiOperationsFloat(void) {
    TestSoViSiOperations<float>();
}
void vctFixedSizeVectorRefTest::TestSoViSiOperationsInt(void) {
    TestSoViSiOperations<int>();
}



template <class _elementType>
void vctFixedSizeVectorRefTest::TestIterators(void) {
    enum {SIZE = 9, STRIDE1 = 5};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestIterators(vector1);
}

void vctFixedSizeVectorRefTest::TestIteratorsDouble(void) {
    TestIterators<double>();
}
void vctFixedSizeVectorRefTest::TestIteratorsFloat(void) {
    TestIterators<float>();
}
void vctFixedSizeVectorRefTest::TestIteratorsInt(void) {
    TestIterators<int>();
}


template <class _elementType>
void vctFixedSizeVectorRefTest::TestSTLFunctions(void) {
    enum {SIZE = 9, STRIDE1 = 5, STRIDE2 = 1};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSTLFunctions(vector1, vector2);
}

void vctFixedSizeVectorRefTest::TestSTLFunctionsDouble(void) {
    TestSTLFunctions<double>();
}
void vctFixedSizeVectorRefTest::TestSTLFunctionsFloat(void) {
    TestSTLFunctions<float>();
}
void vctFixedSizeVectorRefTest::TestSTLFunctionsInt(void) {
    TestSTLFunctions<int>();
}


template <class _elementType>
void vctFixedSizeVectorRefTest::TestNormalization(void) {
    enum {SIZE = 9, STRIDE1 = 5};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctGenericVectorTest::TestNormalization(vector1);
}

void vctFixedSizeVectorRefTest::TestNormalizationDouble(void) {
    TestNormalization<double>();
}
void vctFixedSizeVectorRefTest::TestNormalizationFloat(void) {
    TestNormalization<float>();
}

CPPUNIT_TEST_SUITE_REGISTRATION(vctFixedSizeVectorRefTest);

