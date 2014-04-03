/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Ankur Kapoor, Anton Deguet
  Created on: 2005-10-20
  
  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "nmrSVDTest.h"


const int numberThread = 25;
const int numberTimes = 100;


template <class _matrixTypeI, class _matrixTypeU, class _vectorType, class _matrixTypeVt>
void nmrSVDTest::GenericTestFixedSize(_matrixTypeI & input, _matrixTypeU &U, _vectorType &S, _matrixTypeVt &Vt) 
{
    _matrixTypeI matrixS;
    matrixS.SetAll(0.0);
    matrixS.Diagonal().Assign(S);
    /* Recompose */
    _matrixTypeI product1, product2;
    product1.ProductOf(matrixS, Vt);
    product2.ProductOf(U, product1);
    /* Compare initial with result */
    double error = (input- product2).LinfNorm();
    CPPUNIT_ASSERT(error < cmnTypeTraits<double>::Tolerance());
    /* Make sure that both U and V are orthonormal */
    CPPUNIT_ASSERT(nmrIsOrthonormal(U));
    CPPUNIT_ASSERT(nmrIsOrthonormal(Vt));
}


void nmrSVDTest::TestDynamicColumnMajor(void) {
    int rows, cols;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(10, 20, rows);
    randomSequence.ExtractRandomValue(10, 20, cols);
    vctDynamicMatrix<double> input(rows, cols , VCT_COL_MAJOR);
    vctDynamicMatrix<double> inputOrig(rows, cols , VCT_COL_MAJOR);
    vctRandom(input, 0.0, 10.0);
    inputOrig.Assign(input);
    nmrSVDDynamicData svdData;
    svdData.Allocate(input);
    nmrSVD(input, svdData);
    nmrSVDTest::GenericTestDynamic(inputOrig, svdData.U(), svdData.S(), svdData.Vt());
}

void nmrSVDTest::TestDynamicRowMajor(void) {
    int rows, cols;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(10, 20, rows);
    randomSequence.ExtractRandomValue(10, 20, cols);
    vctDynamicMatrix<double> input(rows, cols , VCT_ROW_MAJOR);
    vctDynamicMatrix<double> inputOrig(rows, cols , VCT_ROW_MAJOR);
    vctRandom(input, 0.0, 10.0);
    inputOrig.Assign(input);
    nmrSVDDynamicData svdData;
    svdData.Allocate(input);
    nmrSVD(input, svdData);
    nmrSVDTest::GenericTestDynamic(inputOrig, svdData.U(), svdData.S(), svdData.Vt());
}

void nmrSVDTest::TestDynamicColumnMajorUserAlloc(void) {
    int rows, cols;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(10, 20, rows);
    randomSequence.ExtractRandomValue(10, 20, cols);
    vctDynamicMatrix<double> input(rows, cols , VCT_COL_MAJOR);
    vctDynamicMatrix<double> inputOrig(rows, cols , VCT_COL_MAJOR);
    vctDynamicMatrix<double> U(rows, rows, VCT_COL_MAJOR);
    vctDynamicMatrix<double> Vt(cols, cols, VCT_COL_MAJOR);
    vctDynamicVector<double> S((rows< cols)?rows:cols);
    vctRandom(input, 0.0, 10.0);
    inputOrig.Assign(input);
    nmrSVD(input, U, S, Vt);
    nmrSVDTest::GenericTestDynamic(inputOrig, U, S, Vt);
}

void nmrSVDTest::TestDynamicRowMajorUserAlloc(void) {
    int rows, cols;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(10, 20, rows);
    randomSequence.ExtractRandomValue(10, 20, cols);
    vctDynamicMatrix<double> input(rows, cols , VCT_ROW_MAJOR);
    vctDynamicMatrix<double> inputOrig(rows, cols , VCT_ROW_MAJOR);
    vctDynamicMatrix<double> U(rows, rows, VCT_ROW_MAJOR);
    vctDynamicMatrix<double> Vt(cols, cols, VCT_ROW_MAJOR);
    vctDynamicVector<double> S((rows< cols)?rows:cols);
    vctRandom(input, 0.0, 10.0);
    inputOrig.Assign(input);
    nmrSVD(input, U, S, Vt);
    nmrSVDTest::GenericTestDynamic(inputOrig, U, S, Vt);
}

void nmrSVDTest::TestFixedSizeColumnMajorMLeqN(void) {
    vctFixedSizeMatrix<double, 5, 7, VCT_COL_MAJOR> input;
    vctFixedSizeMatrix<double, 5, 7, VCT_COL_MAJOR> inputOrig;
    vctRandom(input, 0.0, 10.0);
    inputOrig.Assign(input);
    nmrSVDFixedSizeData<5, 7, VCT_COL_MAJOR> svdData;
    nmrSVD(input, svdData);
    nmrSVDTest::GenericTestFixedSize(inputOrig, svdData.U(), svdData.S(), svdData.Vt());
}

void nmrSVDTest::TestFixedSizeRowMajorMLeqN(void) {
    vctFixedSizeMatrix<double, 4, 6, VCT_ROW_MAJOR> input;
    vctFixedSizeMatrix<double, 4, 6, VCT_ROW_MAJOR> inputOrig;
    vctRandom(input, 0.0, 10.0);
    inputOrig.Assign(input);
    nmrSVDFixedSizeData<4, 6, VCT_ROW_MAJOR> svdData;
    nmrSVD(input, svdData);
    nmrSVDTest::GenericTestFixedSize(inputOrig, svdData.U(), svdData.S(), svdData.Vt());
}

void nmrSVDTest::TestFixedSizeColumnMajorMGeqN(void) {
    vctFixedSizeMatrix<double, 7, 4, VCT_COL_MAJOR> input;
    vctFixedSizeMatrix<double, 7, 4, VCT_COL_MAJOR> inputOrig;
    vctRandom(input, 0.0, 10.0);
    inputOrig.Assign(input);
    nmrSVDFixedSizeData<7, 4, VCT_COL_MAJOR> svdData;
    nmrSVD(input, svdData);
    nmrSVDTest::GenericTestFixedSize(inputOrig, svdData.U(), svdData.S(), svdData.Vt());
}

void nmrSVDTest::TestFixedSizeRowMajorMGeqN(void) {
    vctFixedSizeMatrix<double, 5, 4, VCT_ROW_MAJOR> input;
    vctFixedSizeMatrix<double, 5, 4, VCT_ROW_MAJOR> inputOrig;
    vctRandom(input, 0.0, 10.0);
    inputOrig.Assign(input);
    nmrSVDFixedSizeData<5, 4, VCT_ROW_MAJOR> svdData;
    nmrSVD(input, svdData);
    nmrSVDTest::GenericTestFixedSize(inputOrig, svdData.U(), svdData.S(), svdData.Vt());
}

void nmrSVDTest::TestFixedSizeColumnMajorMLeqN_T2(void) {
    vctFixedSizeMatrix<double, 5, 7, VCT_COL_MAJOR> input;
    vctFixedSizeMatrix<double, 5, 7, VCT_COL_MAJOR> inputOrig;
    vctRandom(input, 0.0, 10.0);
    inputOrig.Assign(input);
    nmrSVDFixedSizeData<5, 7, VCT_COL_MAJOR>::MatrixTypeU U;
    nmrSVDFixedSizeData<5, 7, VCT_COL_MAJOR>::MatrixTypeVt Vt;
    nmrSVDFixedSizeData<5, 7, VCT_COL_MAJOR>::VectorTypeS S;
    nmrSVD(input, U, S, Vt);
    nmrSVDTest::GenericTestFixedSize(inputOrig, U, S, Vt);
}

void nmrSVDTest::TestFixedSizeRowMajorMLeqN_T2(void) {
    vctFixedSizeMatrix<double, 4, 6, VCT_ROW_MAJOR> input;
    vctFixedSizeMatrix<double, 4, 6, VCT_ROW_MAJOR> inputOrig;
    vctRandom(input, 0.0, 10.0);
    inputOrig.Assign(input);
    nmrSVDFixedSizeData<4, 6, VCT_ROW_MAJOR>::MatrixTypeU U;
    nmrSVDFixedSizeData<4, 6, VCT_ROW_MAJOR>::MatrixTypeVt Vt;
    nmrSVDFixedSizeData<4, 6, VCT_ROW_MAJOR>::VectorTypeS S;
    nmrSVD(input, U, S, Vt);
    nmrSVDTest::GenericTestFixedSize(inputOrig, U, S, Vt);
}

void nmrSVDTest::TestFixedSizeColumnMajorMGeqN_T2(void) {
    vctFixedSizeMatrix<double, 7, 4, VCT_COL_MAJOR> input;
    vctFixedSizeMatrix<double, 7, 4, VCT_COL_MAJOR> inputOrig;
    vctRandom(input, 0.0, 10.0);
    inputOrig.Assign(input);
    nmrSVDFixedSizeData<7, 4, VCT_COL_MAJOR>::MatrixTypeU U;
    nmrSVDFixedSizeData<7, 4, VCT_COL_MAJOR>::MatrixTypeVt Vt;
    nmrSVDFixedSizeData<7, 4, VCT_COL_MAJOR>::VectorTypeS S;
    nmrSVD(input, U, S, Vt);
    nmrSVDTest::GenericTestFixedSize(inputOrig, U, S, Vt);
}

void nmrSVDTest::TestFixedSizeRowMajorMGeqN_T2(void) {
    vctFixedSizeMatrix<double, 5, 4, VCT_ROW_MAJOR> input;
    vctFixedSizeMatrix<double, 5, 4, VCT_ROW_MAJOR> inputOrig;
    vctRandom(input, 0.0, 10.0);
    inputOrig.Assign(input);
    nmrSVDFixedSizeData<5, 4, VCT_ROW_MAJOR>::MatrixTypeU U;
    nmrSVDFixedSizeData<5, 4, VCT_ROW_MAJOR>::MatrixTypeVt Vt;
    nmrSVDFixedSizeData<5, 4, VCT_ROW_MAJOR>::VectorTypeS S;
    nmrSVD(input, U, S, Vt);
    nmrSVDTest::GenericTestFixedSize(inputOrig, U, S, Vt);
}

#ifdef BUILD_MULTITHREADED_TESTS
void nmrSVDTest::TestThreadSafetyFixedSize(void)
{
    int ii;
    char name[10];
    osaThread *thread = new osaThread[numberThread];
    nmrSVDFixedSizeTestObject<5, 4, VCT_COL_MAJOR> *threadobj = new nmrSVDFixedSizeTestObject<5, 4, VCT_COL_MAJOR>[numberThread];
    for (ii = 0; ii < numberThread; ii++) {
        argument arg;
        arg.times = numberTimes; arg.number = ii;
        thread[ii].Create<nmrSVDFixedSizeTestObject<5, 4, VCT_COL_MAJOR>, argument>(&threadobj[ii],
                                                                                    &nmrSVDFixedSizeTestObject<5, 4, VCT_COL_MAJOR>::RunThread,
                                                                                    arg, name);
    }
    for (ii = 0; ii < numberThread; ii++) {
        thread[ii].Wait();
    }
    delete[] threadobj;
    delete[] thread;
}

void nmrSVDTest::TestThreadSafetyDynamic(void)
{
    int ii;
    char name[10];
    osaThread *thread = new osaThread[numberThread];
    nmrSVDDynamicTestObject *threadobj = new nmrSVDDynamicTestObject[numberThread];
    for (ii = 0; ii < numberThread; ii++) {
        argument arg;
        arg.times = numberTimes; arg.number = ii;
        thread[ii].Create<nmrSVDDynamicTestObject, argument>(&threadobj[ii],
                                                             &nmrSVDDynamicTestObject::RunThread,
                                                             arg, name);
    }
    for (ii = 0; ii < numberThread; ii++) {
        thread[ii].Wait();
    }
    delete[] threadobj;
    delete[] thread;
}
#endif

CPPUNIT_TEST_SUITE_REGISTRATION(nmrSVDTest);

