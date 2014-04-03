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


#include "vctDynamicVectorRefTest.h"
#include "vctGenericContainerTest.h"
#include "vctGenericVectorTest.h"


#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctDynamicConstVectorRef.h>
#include <cisstVector/vctRandomDynamicVector.h>


// this macro creates a vector##number by reference and the
// associatied storage based on SIZE and STRIDE##number
#define CREATE_STORAGE_AND_REF(number) \
    value_type storage##number[SIZE * STRIDE##number]; \
    vctDynamicVectorRef<value_type> vector##number; \
    vector##number.SetRef(SIZE, storage##number, STRIDE##number)


template <class _elementType>
void vctDynamicVectorRefTest::TestAssignment(void) {
    enum {SIZE = 7, STRIDE1 = 3, STRIDE2 = 2};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctGenericVectorTest::TestAssignment(vector1, vector2);
}

void vctDynamicVectorRefTest::TestAssignmentDouble(void) {
    TestAssignment<double>();
}
void vctDynamicVectorRefTest::TestAssignmentFloat(void) {
    TestAssignment<float>();
}
void vctDynamicVectorRefTest::TestAssignmentInt(void) {
    TestAssignment<int>();
}


template <class _elementType>
void vctDynamicVectorRefTest::TestAccessMethods(void) {
    enum {SIZE = 7, STRIDE1 = 4};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctDynamicConstVectorRef<value_type> vector2(SIZE, vector1.Pointer(), STRIDE1);
    vctGenericVectorTest::TestAccessMethods(vector1, vector2);
}

void vctDynamicVectorRefTest::TestAccessMethodsDouble(void) {
    TestAccessMethods<double>();
}
void vctDynamicVectorRefTest::TestAccessMethodsFloat(void) {
    TestAccessMethods<float>();
}
void vctDynamicVectorRefTest::TestAccessMethodsInt(void) {
    TestAccessMethods<int>();
}



template <class _elementType>
void vctDynamicVectorRefTest::TestSoViOperations(void) {
    enum {SIZE = 6, STRIDE1 = 2, STRIDE2 = 4};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSoCiOperations(vector1, vector2);
}

void vctDynamicVectorRefTest::TestSoViOperationsDouble(void) {
    TestSoViOperations<double>();
}
void vctDynamicVectorRefTest::TestSoViOperationsFloat(void) {
    TestSoViOperations<float>();
}
void vctDynamicVectorRefTest::TestSoViOperationsInt(void) {
    TestSoViOperations<int>();
}



template <class _elementType>
void vctDynamicVectorRefTest::TestSoViViOperations(void) {
    enum {SIZE = 5, STRIDE1 = 1, STRIDE2 = 2, STRIDE3 = 3};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    CREATE_STORAGE_AND_REF(3);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctRandom(vector2, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSoCiCiOperations(vector1, vector2, vector3);
}

void vctDynamicVectorRefTest::TestSoViViOperationsDouble(void) {
    TestSoViViOperations<double>();
}
void vctDynamicVectorRefTest::TestSoViViOperationsFloat(void) {
    TestSoViViOperations<float>();
}
void vctDynamicVectorRefTest::TestSoViViOperationsInt(void) {
    TestSoViViOperations<int>();
}



template <class _elementType>
void vctDynamicVectorRefTest::TestVioViOperations(void) {
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

void vctDynamicVectorRefTest::TestVioViOperationsDouble(void) {
    TestVioViOperations<double>();
}
void vctDynamicVectorRefTest::TestVioViOperationsFloat(void) {
    TestVioViOperations<float>();
}
void vctDynamicVectorRefTest::TestVioViOperationsInt(void) {
    TestVioViOperations<int>();
}



template <class _elementType>
void vctDynamicVectorRefTest::TestVioSiViOperations(void) {
    enum {SIZE = 8, STRIDE1 = 3, STRIDE2 = 2, STRIDE3 = 2};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    CREATE_STORAGE_AND_REF(3);
    value_type scalar;
    vctRandom(vector1, value_type(-10), value_type(10));
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-10),
                                      value_type(10),
                                      scalar);
    vctRandom(vector2, value_type(-10), value_type(10));
    RemoveQuasiZero(vector2);

    vctGenericContainerTest::TestCioSiCiOperations(vector1, scalar, vector2, vector3);
}

void vctDynamicVectorRefTest::TestVioSiViOperationsDouble(void) {
    TestVioSiViOperations<double>();
}
void vctDynamicVectorRefTest::TestVioSiViOperationsFloat(void) {
    TestVioSiViOperations<float>();
}
void vctDynamicVectorRefTest::TestVioSiViOperationsInt(void) {
    TestVioSiViOperations<int>();
}



template <class _elementType>
void vctDynamicVectorRefTest::TestVioViViOperations(void) {
    enum {SIZE = 8, STRIDE1 = 3, STRIDE2 = 2, STRIDE3 = 2, STRIDE4 = 1};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    CREATE_STORAGE_AND_REF(3);
    CREATE_STORAGE_AND_REF(4);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctRandom(vector2, value_type(-10), value_type(10));
    vctRandom(vector3, value_type(-10), value_type(10));
    RemoveQuasiZero(vector3);

    vctGenericContainerTest::TestCioCiCiOperations(vector1, vector2, vector3, vector4);
}

void vctDynamicVectorRefTest::TestVioViViOperationsDouble(void) {
    TestVioViViOperations<double>();
}
void vctDynamicVectorRefTest::TestVioViViOperationsFloat(void) {
    TestVioViViOperations<float>();
}
void vctDynamicVectorRefTest::TestVioViViOperationsInt(void) {
    TestVioViViOperations<int>();
}



template <class _elementType>
void vctDynamicVectorRefTest::TestVoViViOperations(void) {
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

void vctDynamicVectorRefTest::TestVoViViOperationsDouble(void) {
    TestVoViViOperations<double>();
}
void vctDynamicVectorRefTest::TestVoViViOperationsFloat(void) {
    TestVoViViOperations<float>();
}
void vctDynamicVectorRefTest::TestVoViViOperationsInt(void) {
    TestVoViViOperations<int>();
}



template <class _elementType>
void vctDynamicVectorRefTest::TestCrossProduct(void) {
    enum {SIZE = 3, STRIDE1 = 3, STRIDE2 = 1, STRIDE3 = 2};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    CREATE_STORAGE_AND_REF(3);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctRandom(vector2, value_type(-10), value_type(10));

    vctGenericVectorTest::TestCrossProduct(vector1, vector2, vector3);
}

void vctDynamicVectorRefTest::TestCrossProductDouble(void) {
    TestCrossProduct<double>();
}
void vctDynamicVectorRefTest::TestCrossProductFloat(void) {
    TestCrossProduct<float>();
}
void vctDynamicVectorRefTest::TestCrossProductInt(void) {
    TestCrossProduct<int>();
}



template <class _elementType>
void vctDynamicVectorRefTest::TestDotProduct(void) {
    enum {SIZE = 8, STRIDE1 = 3, STRIDE2 = 1};
    typedef _elementType value_type;
    vctDynamicVectorRef<value_type> vector1;
    value_type storage1[SIZE * STRIDE1];
    vector1.SetRef(SIZE, storage1, STRIDE1);
    vctDynamicVectorRef<value_type> vector2;
    value_type storage2[SIZE * STRIDE2];
    vector2.SetRef(SIZE, storage2, STRIDE2);

    vctRandom(vector1, value_type(-10), value_type(10));
    vctRandom(vector2, value_type(-10), value_type(10));

    vctGenericVectorTest::TestDotProduct(vector1, vector2);
}

void vctDynamicVectorRefTest::TestDotProductDouble(void) {
    TestDotProduct<double>();
}
void vctDynamicVectorRefTest::TestDotProductFloat(void) {
    TestDotProduct<float>();
}
void vctDynamicVectorRefTest::TestDotProductInt(void) {
    TestDotProduct<int>();
}



template <class _elementType>
void vctDynamicVectorRefTest::TestVoViOperations(void) {
    enum {SIZE = 4, STRIDE1 = 1, STRIDE2 = 3};
    typedef _elementType value_type;
    vctDynamicVectorRef<value_type> vector1;
    value_type storage1[SIZE * STRIDE1];
    vector1.SetRef(SIZE, storage1, STRIDE1);
    vctDynamicVectorRef<value_type> vector2;
    value_type storage2[SIZE * STRIDE2];
    vector2.SetRef(SIZE, storage2, STRIDE2);

    vctRandom(vector1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCoCiOperations(vector1, vector2);
}

void vctDynamicVectorRefTest::TestVoViOperationsDouble(void) {
    TestVoViOperations<double>();
}
void vctDynamicVectorRefTest::TestVoViOperationsFloat(void) {
    TestVoViOperations<float>();
}
void vctDynamicVectorRefTest::TestVoViOperationsInt(void) {
    TestVoViOperations<int>();
}



template <class _elementType>
void vctDynamicVectorRefTest::TestVoViSiOperations(void) {
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

void vctDynamicVectorRefTest::TestVoViSiOperationsDouble(void) {
    TestVoViSiOperations<double>();
}
void vctDynamicVectorRefTest::TestVoViSiOperationsFloat(void) {
    TestVoViSiOperations<float>();
}
void vctDynamicVectorRefTest::TestVoViSiOperationsInt(void) {
    TestVoViSiOperations<int>();
}




template <class _elementType>
void vctDynamicVectorRefTest::TestVoSiViOperations(void) {
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

void vctDynamicVectorRefTest::TestVoSiViOperationsDouble(void) {
    TestVoSiViOperations<double>();
}
void vctDynamicVectorRefTest::TestVoSiViOperationsFloat(void) {
    TestVoSiViOperations<float>();
}
void vctDynamicVectorRefTest::TestVoSiViOperationsInt(void) {
    TestVoSiViOperations<int>();
}



template <class _elementType>
void vctDynamicVectorRefTest::TestVioSiOperations(void) {
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

void vctDynamicVectorRefTest::TestVioSiOperationsDouble(void) {
    TestVioSiOperations<double>();
}
void vctDynamicVectorRefTest::TestVioSiOperationsFloat(void) {
    TestVioSiOperations<float>();
}
void vctDynamicVectorRefTest::TestVioSiOperationsInt(void) {
    TestVioSiOperations<int>();
}



template <class _elementType>
void vctDynamicVectorRefTest::TestVioOperations(void) {
    enum {SIZE = 9, STRIDE1 = 2, STRIDE2 = 4};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCioOperations(vector1, vector2);
}

void vctDynamicVectorRefTest::TestVioOperationsDouble(void) {
    TestVioOperations<double>();
}
void vctDynamicVectorRefTest::TestVioOperationsFloat(void) {
    TestVioOperations<float>();
}
void vctDynamicVectorRefTest::TestVioOperationsInt(void) {
    TestVioOperations<int>();
}



template <class _elementType>
void vctDynamicVectorRefTest::TestSoViSiOperations(void) {
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

void vctDynamicVectorRefTest::TestSoViSiOperationsDouble(void) {
    TestSoViSiOperations<double>();
}
void vctDynamicVectorRefTest::TestSoViSiOperationsFloat(void) {
    TestSoViSiOperations<float>();
}
void vctDynamicVectorRefTest::TestSoViSiOperationsInt(void) {
    TestSoViSiOperations<int>();
}



template<class _elementType>
void vctDynamicVectorRefTest::TestMinAndMax(void) {
    enum {SIZE = 12, STRIDE1 = 2};
    typedef _elementType value_type;

    CREATE_STORAGE_AND_REF(1);
    vctRandom(vector1, value_type(-50), value_type(50));
    vctGenericContainerTest::TestMinAndMax(vector1);
}

void vctDynamicVectorRefTest::TestMinAndMaxDouble(void) {
    TestMinAndMax<double>();
}

void vctDynamicVectorRefTest::TestMinAndMaxFloat(void) {
    TestMinAndMax<float>();
}

void vctDynamicVectorRefTest::TestMinAndMaxInt(void) {
    TestMinAndMax<int>();
}



template <class _elementType>
void vctDynamicVectorRefTest::TestIterators(void) {
    enum {SIZE = 9, STRIDE1 = 5};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestIterators(vector1);
}

void vctDynamicVectorRefTest::TestIteratorsDouble(void) {
    TestIterators<double>();
}
void vctDynamicVectorRefTest::TestIteratorsFloat(void) {
    TestIterators<float>();
}
void vctDynamicVectorRefTest::TestIteratorsInt(void) {
    TestIterators<int>();
}



template <class _elementType>
void vctDynamicVectorRefTest::TestSTLFunctions(void) {
    enum {SIZE = 9, STRIDE1 = 5, STRIDE2 = 1};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    CREATE_STORAGE_AND_REF(2);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSTLFunctions(vector1, vector2);
}

void vctDynamicVectorRefTest::TestSTLFunctionsDouble(void) {
    TestSTLFunctions<double>();
}
void vctDynamicVectorRefTest::TestSTLFunctionsFloat(void) {
    TestSTLFunctions<float>();
}
void vctDynamicVectorRefTest::TestSTLFunctionsInt(void) {
    TestSTLFunctions<int>();
}



template <class _elementType>
void vctDynamicVectorRefTest::TestNormalization(void) {
    enum {SIZE = 4, STRIDE1 = 1};
    typedef _elementType value_type;
    CREATE_STORAGE_AND_REF(1);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctGenericVectorTest::TestNormalization(vector1);
}

void vctDynamicVectorRefTest::TestNormalizationDouble(void) {
    TestNormalization<double>();
}
void vctDynamicVectorRefTest::TestNormalizationFloat(void) {
    TestNormalization<float>();
}


CPPUNIT_TEST_SUITE_REGISTRATION(vctDynamicVectorRefTest);

