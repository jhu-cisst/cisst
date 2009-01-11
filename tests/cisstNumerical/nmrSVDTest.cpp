/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrSVDTest.cpp,v 1.18 2007/08/28 14:49:26 anton Exp $
  
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


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrSVDTest.cpp,v $
// Revision 1.18  2007/08/28 14:49:26  anton
// cisstNumericalTests: Removed sprintf and code to print to cout or cerr.
// Also removed commented code to cleanup.
//
// Revision 1.17  2007/05/18 21:56:36  anton
// cisstNumerical tests: Modified signature of test methods to avoid implicit
// conversion between containers and containersRef as these constructors are
// now "explicit".   See [2349] by Ofri.
//
// Revision 1.16  2007/04/26 20:12:05  anton
// All files in tests: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
//
// Revision 1.15  2007/02/12 03:25:03  anton
// cisstVector and cisstNumerical tests: Used more refined #include for vctRandom
// (see ticket #264).
//
// Revision 1.14  2006/11/20 20:33:52  anton
// Licensing: Applied new license to tests.
//
// Revision 1.13  2006/10/06 14:32:51  anton
// All tests: Re-ordered #include to load cmnPortability.h before any system
// include.
//
// Revision 1.12  2006/01/27 01:00:42  anton
// cisstNumerical tests: Renamed "solution" to "data" (see #205).
//
// Revision 1.11  2006/01/25 03:17:16  anton
// cisstVector and cisstNumerical: Test new type MatrixSizeType and test it
// with nmrSVD (see ticket #206).
//
// Revision 1.10  2006/01/09 22:54:57  anton
// cisstNumerical tests: Reduced number of threads for tests.  Was quite time
// consuming.
//
// Revision 1.9  2006/01/09 22:46:01  anton
// nmrSVD: Added method UpdateMatrixS for dynamic containers.
//
// Revision 1.8  2006/01/05 03:54:40  anton
// cisstNumerical tests: Compile multithreaded tests only if cisstOSAbstraction
// has been compiled instead of requiring cisstOSAbstraction when cisstNetlib
// is used.  See ticket #200.
//
// Revision 1.7  2005/12/28 21:59:01  anton
// cisstNumerical: Removed nmrCommon.h since it has become obsolete.
//
// Revision 1.6  2005/12/21 17:09:32  anton
// nmrSVDTest.cpp: Use StorageOrder() instead of IsRowMajor().
//
// Revision 1.5  2005/12/20 16:39:51  anton
// nmrSVDTest: Updated to reflect the new nmrSVD API.
//
// Revision 1.4  2005/11/30 06:04:45  kapoor
// nmrSVDTest.cpp: Fixed memory leak (added delete[] corresponding to each new[])
//
// Revision 1.3  2005/11/29 03:10:36  anton
// nmrSVDTest.cpp: Changed from 1000 iterations to 100 per thread.
//
// Revision 1.2  2005/11/20 21:23:41  kapoor
// nmrSVD: Changes to the netlib wrappers. See wiki pages SvdNotes and ticket #184.
//
// Revision 1.1  2005/10/21 23:14:31  anton
// nmrSVD: Added tests from Ankur.
//
//
// ****************************************************************************
