/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2006-01-27
  
  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "nmrInverseTest.h"

#include <cisstNumerical/nmrIsOrthonormal.h>


const int numberThread = 25;
const int numberTimes = 100;

template <class _matrixTypeA, class _matrixTypeAInverse>
void nmrInverseTest::GenericTestFixedSize(const _matrixTypeA & A,
                                          const _matrixTypeAInverse & AInv)
{
    /* Recompose */
    _matrixTypeA product;
    product.ProductOf(A, AInv);
    /* Evaluate error */
    product.Diagonal().Subtract(1.0);
    double error = product.LinfNorm();
    CPPUNIT_ASSERT(error < cmnTypeTraits<double>::Tolerance());
}

void nmrInverseTest::GenericTestDynamic(const vctDynamicMatrix<double> & input,
                                        const vctDynamicMatrix<double> & inverse) 
{
    /* Recompose */
    vctDynamicMatrix<double> product(input.rows(), input.cols(), input.StorageOrder());
    product = input * inverse;
    /* Compare initial with result */
    product.Diagonal().Subtract(1.0);
    double error = product.LinfNorm();
    CPPUNIT_ASSERT(error < cmnTypeTraits<double>::Tolerance());
}

void nmrInverseTest::TestDynamicColumnMajor(void) {
    int size;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(10, 20, size);
    vctDynamicMatrix<double> input(size, size , VCT_COL_MAJOR);
    vctDynamicMatrix<double> inputOrig(size, size , VCT_COL_MAJOR);
    vctRandom(input, 0.0, 10.0);
    inputOrig.Assign(input);
    nmrInverseDynamicData invData;
    invData.Allocate(input);
    nmrInverse(input, invData);
    nmrInverseTest::GenericTestDynamic(inputOrig, input);
}

void nmrInverseTest::TestDynamicRowMajor(void) {
    int size;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(10, 20, size);
    vctDynamicMatrix<double> input(size, size , VCT_ROW_MAJOR);
    vctDynamicMatrix<double> inputOrig(size, size , VCT_ROW_MAJOR);
    vctRandom(input, 0.0, 10.0);
    inputOrig.Assign(input);
    nmrInverseDynamicData invData;
    invData.Allocate(input);
    nmrInverse(input, invData);
    nmrInverseTest::GenericTestDynamic(inputOrig, input);
}

void nmrInverseTest::TestDynamicColumnMajorUserAlloc(void) {
    int size;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(10, 20, size);
    vctDynamicMatrix<double> input(size, size , VCT_COL_MAJOR);
    vctDynamicMatrix<double> inputOrig(size, size , VCT_COL_MAJOR);
    vctDynamicVector<CISSTNETLIB_INTEGER> pivotIndices(nmrInverseDynamicData::PivotIndicesSize(input));
    vctDynamicVector<double> workspace(nmrInverseDynamicData::WorkspaceSize(input));
    vctRandom(input, 0.0, 10.0);
    inputOrig.Assign(input);
    nmrInverse(input, pivotIndices, workspace);
    nmrInverseTest::GenericTestDynamic(inputOrig, input);
}

void nmrInverseTest::TestDynamicRowMajorUserAlloc(void) {
    int size;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(10, 20, size);
    vctDynamicMatrix<double> input(size, size , VCT_ROW_MAJOR);
    vctDynamicMatrix<double> inputOrig(size, size , VCT_ROW_MAJOR);
    vctDynamicVector<CISSTNETLIB_INTEGER> pivotIndices(nmrInverseDynamicData::PivotIndicesSize(input));
    vctDynamicVector<double> workspace(nmrInverseDynamicData::WorkspaceSize(input));
    vctRandom(input, 0.0, 10.0);
    inputOrig.Assign(input);
    nmrInverse(input, pivotIndices, workspace);
    nmrInverseTest::GenericTestDynamic(inputOrig, input);
}


void nmrInverseTest::TestFixedSizeColumnMajor(void) {
    enum {SIZE = 6};
    vctFixedSizeMatrix<double, SIZE, SIZE, VCT_COL_MAJOR> input, inputOrig;
    vctRandom(input, 0.0, 10.0);
    inputOrig.Assign(input);
    nmrInverseFixedSizeData<SIZE, VCT_COL_MAJOR> invData;
    nmrInverse(input, invData);
    nmrInverseTest::GenericTestFixedSize(inputOrig, input);
}

void nmrInverseTest::TestFixedSizeRowMajor(void) {
    enum {SIZE = 8};
    vctFixedSizeMatrix<double, SIZE, SIZE, VCT_ROW_MAJOR> input, inputOrig;
    vctRandom(input, 0.0, 10.0);
    inputOrig.Assign(input);
    nmrInverseFixedSizeData<SIZE, VCT_ROW_MAJOR> invData;
    nmrInverse(input, invData);
    nmrInverseTest::GenericTestFixedSize(inputOrig, input);
}

void nmrInverseTest::TestFixedSizeColumnMajorUserAlloc(void) {
    enum {SIZE = 7};
    vctFixedSizeMatrix<double, SIZE, SIZE, VCT_COL_MAJOR> input, inputOrig;
    typedef nmrInverseFixedSizeData<SIZE, VCT_COL_MAJOR> DataType;
    DataType::VectorTypePivotIndices pivotIndices;
    DataType::VectorTypeWorkspace workspace;
    vctRandom(input, 0.0, 10.0);
    inputOrig.Assign(input);
    nmrInverse(input, pivotIndices, workspace);
    nmrInverseTest::GenericTestFixedSize(inputOrig, input);
}

void nmrInverseTest::TestFixedSizeRowMajorUserAlloc(void) {
    enum {SIZE = 5};
    vctFixedSizeMatrix<double, SIZE, SIZE, VCT_ROW_MAJOR> input, inputOrig;
    typedef nmrInverseFixedSizeData<SIZE, VCT_ROW_MAJOR> DataType;
    DataType::VectorTypePivotIndices pivotIndices;
    DataType::VectorTypeWorkspace workspace;
    vctRandom(input, 0.0, 10.0);
    inputOrig.Assign(input);
    nmrInverse(input, pivotIndices, workspace);
    nmrInverseTest::GenericTestFixedSize(inputOrig, input);
}



#ifdef BUILD_MULTITHREADED_TESTS

void nmrInverseTest::TestThreadSafetyFixedSize(void)
{
    int ii;
    char name[10];
    osaThread *thread = new osaThread[numberThread];
    nmrInverseFixedSizeTestObject<5, VCT_COL_MAJOR> *threadobj = new nmrInverseFixedSizeTestObject<5, VCT_COL_MAJOR>[numberThread];
    for (ii = 0; ii < numberThread; ii++) {
        argument arg;
        arg.times = numberTimes; arg.number = ii;
        sprintf(name, "T%d", ii);
        thread[ii].Create<nmrInverseFixedSizeTestObject<5, VCT_COL_MAJOR>, argument>(&threadobj[ii],
                                                                                     &nmrInverseFixedSizeTestObject<5, VCT_COL_MAJOR>::RunThread,
                                                                                     arg, name);
    }
    for (ii = 0; ii < numberThread; ii++) {
        thread[ii].Wait();
    }
    delete[] threadobj;
    delete[] thread;
}


void nmrInverseTest::TestThreadSafetyDynamic(void)
{
    int ii;
    char name[10];
    osaThread *thread = new osaThread[numberThread];
    nmrInverseDynamicTestObject *threadobj = new nmrInverseDynamicTestObject[numberThread];
    for (ii = 0; ii < numberThread; ii++) {
        argument arg;
        arg.times = numberTimes; arg.number = ii;
        sprintf(name, "T%d", ii);
        thread[ii].Create<nmrInverseDynamicTestObject, argument>(&threadobj[ii],
                                                                 &nmrInverseDynamicTestObject::RunThread,
                                                                 arg, name);
    }
    for (ii = 0; ii < numberThread; ii++) {
        thread[ii].Wait();
    }
    delete[] threadobj;
    delete[] thread;
}
#endif // BUILD_MULTITHREADED_TESTS


CPPUNIT_TEST_SUITE_REGISTRATION(nmrInverseTest);

