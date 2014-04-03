/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Ankur Kapoor
  Created on: 2005-11-04
  
  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "nmrPInverseTest.h"

#include <cisstVector/vctRandomFixedSizeMatrix.h>
#include <cisstVector/vctRandomDynamicMatrix.h>
#include <cisstNumerical/nmrPInverse.h>

#include <cisstCommon/cmnPrintf.h>
#include <cisstVector/vctPrintf.h>

#include <iostream>

#define MINRANK 2

template <vct::size_type _rows, vct::size_type _cols, bool _storageOrder, vct::size_type _minmn>
void nmrPInverseTest::GenericTestFixedSize(void)
{
    vctFixedSizeMatrix<double, _rows, _cols, _storageOrder> indepVectors;
    vctFixedSizeMatrix<double, _rows, _cols, _storageOrder> input;
    vctFixedSizeMatrix<double, _rows, _cols, _storageOrder> inputOrig;
    vctFixedSizeMatrix<double, _cols, _rows, _storageOrder> pinverse;
    vctFixedSizeMatrix<double, _rows, _cols, _storageOrder> SM;
    vctFixedSizeMatrix<double, _rows, _cols, _storageOrder> P;
    vctFixedSizeMatrix<double, _rows, _rows, _storageOrder> U;
    vctFixedSizeMatrix<double, _cols, _cols, _storageOrder> Vt;
    vctFixedSizeVector<double, _minmn> S;
    vctFixedSizeMatrix<double, _rows, _rows> A_Ap;
    vctFixedSizeMatrix<double, _cols, _cols> Ap_A;
    vctFixedSizeMatrix<double, _rows, _cols> A_Ap_A;
    vctFixedSizeMatrix<double, _cols, _rows> Ap_A_Ap;
    vctFixedSizeMatrix<double, _rows, _cols> E0;
    vctFixedSizeMatrix<double, _cols, _rows> E1;
    vctFixedSizeMatrix<double, _rows, _rows> E2;
    vctFixedSizeMatrix<double, _cols, _cols> E3;
    double error[4];
    vctRandom(indepVectors, 0.0, 10.0);
    nmrSVD(indepVectors, U, S, Vt);
    SM.SetAll(0.);
    for (vct::size_type rank = _minmn; rank >= MINRANK; rank--) {
        if (rank < S.size()) S(rank) = 1e-18; // some number less than eps
        SM.Diagonal().Assign(S);
        P.ProductOf(SM, Vt);
        input.ProductOf(U, P);
        inputOrig.Assign(input);
        nmrPInverse(input, pinverse);
        const double tolerance = S(0)*cmnTypeTraits<double>::Tolerance()*_rows*_cols;
        /* there should be four test conditions
           A A^+ A = A
           A^+ A A^+ = A^+
           (A A^+)^T = A A^+
           (A^+ A)^T = A^+ A
see: strang, g., linear algebra and its application, second ed. pp145
         */
        A_Ap = inputOrig * pinverse;
        Ap_A = pinverse * inputOrig;
        A_Ap_A = inputOrig * pinverse * inputOrig;
        Ap_A_Ap = pinverse * inputOrig * pinverse;
        E0.DifferenceOf(A_Ap_A , inputOrig);
        E1.DifferenceOf(Ap_A_Ap , pinverse);
        E2.DifferenceOf(A_Ap , A_Ap.Transpose());
        E3.DifferenceOf(Ap_A , Ap_A.Transpose());
        error[0] = E0.LinfNorm();
        error[1] = E1.LinfNorm();
        error[2] = E2.LinfNorm();
        error[3] = E3.LinfNorm();

        bool ret_value = true;
        for (vct::size_type ii = 0; ii < 4; ii++) {
            if (!(error[ii] < tolerance)) {
#if !CISST_CMAKE_COMPILER_IS_MSVC_64
                CMN_LOG_INIT_ERROR << cmnPrintf("\nF :\n %.14lf \n") << ii;
                CMN_LOG_INIT_ERROR << cmnPrintf("E%d :\n %.14lf \n") << ii  << error[ii];
#endif
                switch (ii) {
                    case 0:
                        CMN_LOG_INIT_ERROR << cmnPrintf("EM0:\n %.14lf \n") << E0;
                        break;
                    case 1:
                        CMN_LOG_INIT_ERROR << cmnPrintf("EM1:\n %.14lf \n") << E1;
                        break;
                    case 2:
                        CMN_LOG_INIT_ERROR << cmnPrintf("EM2:\n %.14lf \n") << E2;
                        break;
                    case 3:
                        CMN_LOG_INIT_ERROR << cmnPrintf("EM3:\n %.14lf \n") << E3;
                        break;
                }
                ret_value &= false;
            }
        }
        if (!ret_value) {
            CMN_LOG_INIT_ERROR << cmnPrintf("I :\n %.14lf \n") << inputOrig;
            CMN_LOG_INIT_ERROR << cmnPrintf("U :\n %.14lf \n") << U;
            CMN_LOG_INIT_ERROR << cmnPrintf("S :\n %.14lf \n") << S;
            CMN_LOG_INIT_ERROR << cmnPrintf("Vt:\n %.14lf \n") << Vt;
            CMN_LOG_INIT_ERROR << cmnPrintf("SM:\n %.14lf \n") << SM;
            CMN_LOG_INIT_ERROR << cmnPrintf("P :\n %.14lf \n") << P;
            CMN_LOG_INIT_ERROR << cmnPrintf("O :\n %.14lf \n") << pinverse;
        }
        CPPUNIT_ASSERT(error[0] < tolerance);
        CPPUNIT_ASSERT(error[1] < tolerance);
        CPPUNIT_ASSERT(error[2] < tolerance);
        CPPUNIT_ASSERT(error[3] < tolerance);
    }
}

template <vct::size_type _rows, vct::size_type _cols, bool _storageOrder, vct::size_type _minmn>
void nmrPInverseTest::GenericTestFixedSizeUsingDataObject(void)
{
    vctFixedSizeMatrix<double, _rows, _cols, _storageOrder> indepVectors;
    vctFixedSizeMatrix<double, _rows, _cols, _storageOrder> input;
    vctFixedSizeMatrix<double, _rows, _cols, _storageOrder> inputOrig;
    vctFixedSizeMatrix<double, _rows, _cols, _storageOrder> SM;
    vctFixedSizeMatrix<double, _rows, _cols, _storageOrder> P;
    vctFixedSizeMatrix<double, _rows, _rows, _storageOrder> U;
    vctFixedSizeMatrix<double, _cols, _cols, _storageOrder> Vt;
    vctFixedSizeVector<double, _minmn> S;
    vctFixedSizeMatrix<double, _rows, _rows> A_Ap;
    vctFixedSizeMatrix<double, _cols, _cols> Ap_A;
    vctFixedSizeMatrix<double, _rows, _cols> A_Ap_A;
    vctFixedSizeMatrix<double, _cols, _rows> Ap_A_Ap;
    vctFixedSizeMatrix<double, _rows, _cols> E0;
    vctFixedSizeMatrix<double, _cols, _rows> E1;
    vctFixedSizeMatrix<double, _rows, _rows> E2;
    vctFixedSizeMatrix<double, _cols, _cols> E3;
    nmrPInverseFixedSizeData<_rows, _cols, _storageOrder> data;
    double error[4];
    vctRandom(indepVectors, 0.0, 10.0);
    nmrSVD(indepVectors, U, S, Vt);
    SM.SetAll(0.);
    for (vct::size_type rank = _minmn; rank >= MINRANK; rank--) {
        if (rank < S.size()) S(rank) = 1e-18; // some number less than eps
        SM.Diagonal().Assign(S);
        P.ProductOf(SM, Vt);
        input.ProductOf(U, P);
        inputOrig.Assign(input);
        nmrPInverse(input, data);
        const double tolerance = S(0)*cmnTypeTraits<double>::Tolerance()*_rows*_cols;
        /* there should be four test conditions
           A A^+ A = A
           A^+ A A^+ = A^+
           (A A^+)^T = A A^+
           (A^+ A)^T = A^+ A
see: strang, g., linear algebra and its application, second ed. pp145
         */
        A_Ap = inputOrig * data.PInverse();
        Ap_A = data.PInverse() * inputOrig;
        A_Ap_A = inputOrig * data.PInverse() * inputOrig;
        Ap_A_Ap = data.PInverse() * inputOrig * data.PInverse();
        E0.DifferenceOf(A_Ap_A , inputOrig);
        E1.DifferenceOf(Ap_A_Ap , data.PInverse());
        E2.DifferenceOf(A_Ap , A_Ap.Transpose());
        E3.DifferenceOf(Ap_A , Ap_A.Transpose());
        error[0] = E0.LinfNorm();
        error[1] = E1.LinfNorm();
        error[2] = E2.LinfNorm();
        error[3] = E3.LinfNorm();

        bool ret_value = true;
        for (vct::size_type ii = 0; ii < 4; ii++) {
            if (!(error[ii] < tolerance)) {
#if !CISST_CMAKE_COMPILER_IS_MSVC_64
                CMN_LOG_INIT_ERROR << cmnPrintf("\nF :\n %.14lf \n") << ii;
                CMN_LOG_INIT_ERROR << cmnPrintf("E%d :\n %.14lf \n") << ii  << error[ii];
#endif
                switch (ii) {
                    case 0:
                        CMN_LOG_INIT_ERROR << cmnPrintf("EM0:\n %.14lf \n") << E0;
                        break;
                    case 1:
                        CMN_LOG_INIT_ERROR << cmnPrintf("EM1:\n %.14lf \n") << E1;
                        break;
                    case 2:
                        CMN_LOG_INIT_ERROR << cmnPrintf("EM2:\n %.14lf \n") << E2;
                        break;
                    case 3:
                        CMN_LOG_INIT_ERROR << cmnPrintf("EM3:\n %.14lf \n") << E3;
                        break;
                }
                ret_value &= false;
            }
        }
        if (!ret_value) {
            CMN_LOG_INIT_ERROR << cmnPrintf("I :\n %.14lf \n") << inputOrig;
            CMN_LOG_INIT_ERROR << cmnPrintf("U :\n %.14lf \n") << U;
            CMN_LOG_INIT_ERROR << cmnPrintf("S :\n %.14lf \n") << S;
            CMN_LOG_INIT_ERROR << cmnPrintf("Vt:\n %.14lf \n") << Vt;
            CMN_LOG_INIT_ERROR << cmnPrintf("SM:\n %.14lf \n") << SM;
            CMN_LOG_INIT_ERROR << cmnPrintf("P :\n %.14lf \n") << P;
            CMN_LOG_INIT_ERROR << cmnPrintf("O :\n %.14lf \n") << data.PInverse();
        }
        CPPUNIT_ASSERT(error[0] < tolerance);
        CPPUNIT_ASSERT(error[1] < tolerance);
        CPPUNIT_ASSERT(error[2] < tolerance);
        CPPUNIT_ASSERT(error[3] < tolerance);
    }
}

void nmrPInverseTest::GenericTestDynamicUsingDataObject(vct::size_type rows, vct::size_type cols, bool storageOrder)
{
    const vct::size_type minmn = (rows < cols) ? rows : cols;
    vctDynamicMatrix<double> indepVectors(rows, cols, storageOrder);
    vctDynamicMatrix<double> input       (rows, cols, storageOrder);
    vctDynamicMatrix<double> inputOrig   (rows, cols, storageOrder);
    vctDynamicMatrix<double> SM          (rows, cols, storageOrder);
    vctDynamicMatrix<double> P           (rows, cols, storageOrder);
    vctDynamicMatrix<double> U           (rows, rows, storageOrder);
    vctDynamicMatrix<double> Vt          (cols, cols, storageOrder);
    vctDynamicVector<double> S (minmn);
    vctDynamicMatrix<double> A_Ap   (rows, rows);
    vctDynamicMatrix<double> Ap_A   (cols, cols);
    vctDynamicMatrix<double> A_Ap_A (rows, cols);
    vctDynamicMatrix<double> Ap_A_Ap(cols, rows);
    vctDynamicMatrix<double> E0     (rows, cols);
    vctDynamicMatrix<double> E1     (cols, rows);
    vctDynamicMatrix<double> E2     (rows, rows);
    vctDynamicMatrix<double> E3     (cols, cols);
    nmrPInverseDynamicData data;
    data.Allocate(input);
    double error[4];
    vctRandom(indepVectors, 0.0, 10.0);
    nmrSVD(indepVectors, U, S, Vt);
    SM.SetAll(0.);
    for (vct::size_type rank = minmn; rank >= MINRANK; rank--) {
        if (rank < S.size()) S(rank) = 1e-18; // some number less than eps
        SM.Diagonal().Assign(S);
        P.ProductOf(SM, Vt);
        input.ProductOf(U, P);
        inputOrig.Assign(input);
        nmrPInverse(input, data);
        const double tolerance = S(0)*cmnTypeTraits<double>::Tolerance()*rows*cols;
        /* there should be four test conditions
           A A^+ A = A
           A^+ A A^+ = A^+
           (A A^+)^T = A A^+
           (A^+ A)^T = A^+ A
see: strang, g., linear algebra and its application, second ed. pp145
         */
        A_Ap = inputOrig * data.PInverse();
        Ap_A = data.PInverse() * inputOrig;
        A_Ap_A = inputOrig * data.PInverse() * inputOrig;
        Ap_A_Ap = data.PInverse() * inputOrig * data.PInverse();
        E0.DifferenceOf(A_Ap_A , inputOrig);
        E1.DifferenceOf(Ap_A_Ap , data.PInverse());
        E2.DifferenceOf(A_Ap , A_Ap.Transpose());
        E3.DifferenceOf(Ap_A , Ap_A.Transpose());
        error[0] = E0.LinfNorm();
        error[1] = E1.LinfNorm();
        error[2] = E2.LinfNorm();
        error[3] = E3.LinfNorm();

        bool ret_value = true;
        for (vct::size_type ii = 0; ii < 4; ii++) {
            if (!(error[ii] < tolerance)) {
#if !CISST_CMAKE_COMPILER_IS_MSVC_64
                CMN_LOG_INIT_ERROR << cmnPrintf("\nF :\n %.14lf \n") << ii;
                CMN_LOG_INIT_ERROR << cmnPrintf("E%d :\n %.14lf \n") << ii  << error[ii];
#endif
                switch (ii) {
                    case 0:
                        CMN_LOG_INIT_ERROR << cmnPrintf("EM0:\n %.14lf \n") << E0;
                        break;
                    case 1:
                        CMN_LOG_INIT_ERROR << cmnPrintf("EM1:\n %.14lf \n") << E1;
                        break;
                    case 2:
                        CMN_LOG_INIT_ERROR << cmnPrintf("EM2:\n %.14lf \n") << E2;
                        break;
                    case 3:
                        CMN_LOG_INIT_ERROR << cmnPrintf("EM3:\n %.14lf \n") << E3;
                        break;
                }
                ret_value &= false;
            }
        }
        if (!ret_value) {
            CMN_LOG_INIT_ERROR << cmnPrintf("I :\n %.14lf \n") << inputOrig;
            CMN_LOG_INIT_ERROR << cmnPrintf("U :\n %.14lf \n") << U;
            CMN_LOG_INIT_ERROR << cmnPrintf("S :\n %.14lf \n") << S;
            CMN_LOG_INIT_ERROR << cmnPrintf("Vt:\n %.14lf \n") << Vt;
            CMN_LOG_INIT_ERROR << cmnPrintf("SM:\n %.14lf \n") << SM;
            CMN_LOG_INIT_ERROR << cmnPrintf("P :\n %.14lf \n") << P;
            CMN_LOG_INIT_ERROR << cmnPrintf("O :\n %.14lf \n") << data.PInverse();
        }
        CPPUNIT_ASSERT(error[0] < tolerance);
        CPPUNIT_ASSERT(error[1] < tolerance);
        CPPUNIT_ASSERT(error[2] < tolerance);
        CPPUNIT_ASSERT(error[3] < tolerance);
    }
}

void nmrPInverseTest::GenericTestDynamic(vct::size_type rows, vct::size_type cols, bool storageOrder)
{
    const vct::size_type minmn = (rows < cols) ? rows : cols;
    vctDynamicMatrix<double> indepVectors(rows, cols, storageOrder);
    vctDynamicMatrix<double> input       (rows, cols, storageOrder);
    vctDynamicMatrix<double> inputOrig   (rows, cols, storageOrder);
    vctDynamicMatrix<double> pinverse    (cols, rows, storageOrder);
    vctDynamicMatrix<double> SM          (rows, cols, storageOrder);
    vctDynamicMatrix<double> P           (rows, cols, storageOrder);
    vctDynamicMatrix<double> U           (rows, rows, storageOrder);
    vctDynamicMatrix<double> Vt          (cols, cols, storageOrder);
    vctDynamicVector<double> S (minmn);
    vctDynamicMatrix<double> A_Ap   (rows, rows);
    vctDynamicMatrix<double> Ap_A   (cols, cols);
    vctDynamicMatrix<double> A_Ap_A (rows, cols);
    vctDynamicMatrix<double> Ap_A_Ap(cols, rows);
    vctDynamicMatrix<double> E0     (rows, cols);
    vctDynamicMatrix<double> E1     (cols, rows);
    vctDynamicMatrix<double> E2     (rows, rows);
    vctDynamicMatrix<double> E3     (cols, cols);
    double error[4];
    vctRandom(indepVectors, 0.0, 10.0);
    nmrSVD(indepVectors, U, S, Vt);
    SM.SetAll(0.);
    for (vct::size_type rank = minmn; rank >= MINRANK; rank--) {
        if (rank < S.size()) S(rank) = 1e-18; // some number less than eps
        SM.Diagonal().Assign(S);
        P.ProductOf(SM, Vt);
        input.ProductOf(U, P);
        inputOrig.Assign(input);
        nmrPInverse(input, pinverse);
        const double tolerance = S(0)*cmnTypeTraits<double>::Tolerance()*rows*cols;
        /* there should be four test conditions
           A A^+ A = A
           A^+ A A^+ = A^+
           (A A^+)^T = A A^+
           (A^+ A)^T = A^+ A
see: strang, g., linear algebra and its application, second ed. pp145
         */
        A_Ap = inputOrig * pinverse;
        Ap_A = pinverse * inputOrig;
        A_Ap_A = inputOrig * pinverse * inputOrig;
        Ap_A_Ap = pinverse * inputOrig * pinverse;
        E0.DifferenceOf(A_Ap_A , inputOrig);
        E1.DifferenceOf(Ap_A_Ap , pinverse);
        E2.DifferenceOf(A_Ap , A_Ap.Transpose());
        E3.DifferenceOf(Ap_A , Ap_A.Transpose());
        error[0] = E0.LinfNorm();
        error[1] = E1.LinfNorm();
        error[2] = E2.LinfNorm();
        error[3] = E3.LinfNorm();

        bool ret_value = true;
        for (vct::size_type ii = 0; ii < 4; ii++) {
            if (!(error[ii] < tolerance)) {
#if !CISST_CMAKE_COMPILER_IS_MSVC_64
                CMN_LOG_INIT_ERROR << cmnPrintf("\nF :\n %.14lf \n") << ii;
                CMN_LOG_INIT_ERROR << cmnPrintf("E%d :\n %.14lf \n") << ii  << error[ii];
#endif
                switch (ii) {
                    case 0:
                        CMN_LOG_INIT_ERROR << cmnPrintf("EM0:\n %.14lf \n") << E0;
                        break;
                    case 1:
                        CMN_LOG_INIT_ERROR << cmnPrintf("EM1:\n %.14lf \n") << E1;
                        break;
                    case 2:
                        CMN_LOG_INIT_ERROR << cmnPrintf("EM2:\n %.14lf \n") << E2;
                        break;
                    case 3:
                        CMN_LOG_INIT_ERROR << cmnPrintf("EM3:\n %.14lf \n") << E3;
                        break;
                }
                ret_value &= false;
            }
        }
        if (!ret_value) {
            CMN_LOG_INIT_ERROR << cmnPrintf("I :\n %.14lf \n") << inputOrig;
            CMN_LOG_INIT_ERROR << cmnPrintf("U :\n %.14lf \n") << U;
            CMN_LOG_INIT_ERROR << cmnPrintf("S :\n %.14lf \n") << S;
            CMN_LOG_INIT_ERROR << cmnPrintf("Vt:\n %.14lf \n") << Vt;
            CMN_LOG_INIT_ERROR << cmnPrintf("SM:\n %.14lf \n") << SM;
            CMN_LOG_INIT_ERROR << cmnPrintf("P :\n %.14lf \n") << P;
            CMN_LOG_INIT_ERROR << cmnPrintf("O :\n %.14lf \n") << pinverse;
        }
        CPPUNIT_ASSERT(error[0] < tolerance);
        CPPUNIT_ASSERT(error[1] < tolerance);
        CPPUNIT_ASSERT(error[2] < tolerance);
        CPPUNIT_ASSERT(error[3] < tolerance);
    }
}

void nmrPInverseTest::TestDynamicColumnMajorUsingDataObject(void) {
    int rows, cols;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(5, 7, rows);
    randomSequence.ExtractRandomValue(5, 7, cols);
    GenericTestDynamicUsingDataObject(rows, cols, VCT_COL_MAJOR);
}

void nmrPInverseTest::TestDynamicRowMajorUsingDataObject(void) {
    int rows, cols;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(10, 20, rows);
    randomSequence.ExtractRandomValue(10, 20, cols);
    GenericTestDynamicUsingDataObject(rows, cols, VCT_ROW_MAJOR);
}

void nmrPInverseTest::TestDynamicColumnMajor(void) {
    int rows, cols;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(10, 20, rows);
    randomSequence.ExtractRandomValue(10, 20, cols);
    GenericTestDynamic(rows, cols, VCT_COL_MAJOR);
}

void nmrPInverseTest::TestDynamicRowMajor(void) {
    int rows, cols;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(10, 20, rows);
    randomSequence.ExtractRandomValue(10, 20, cols);
    GenericTestDynamic(rows, cols, VCT_ROW_MAJOR);
}

template <vct::size_type _rows, vct::size_type _cols>
void nmrPInverseTest::GenericTestCompareWithMatlab(double input_ptr[], double pinverse_ptr[], int caseNo, double tolerance)
{
    vctFixedSizeMatrix<double, _rows, _cols, VCT_COL_MAJOR> input;
    vctFixedSizeMatrix<double, _rows, _cols, VCT_COL_MAJOR> inputCopy;
    vctFixedSizeMatrix<double, _cols, _rows, VCT_COL_MAJOR> output;
    vctFixedSizeVector<double, _rows*_cols> errorv;
    double error;
    input.Assign(input_ptr, false);
    inputCopy.Assign(input);
    nmrPInverse(input, output);
    double *output_ptr = output.Pointer();
    for (vct::size_type ii = 0; ii < _rows*_cols; ii++) {
        errorv(ii) = output_ptr[ii] - pinverse_ptr[ii];
    }
    error = errorv.LinfNorm();
    if (!(error < tolerance)) {
        CMN_LOG_INIT_ERROR << cmnPrintf("\nCase No: %d\n") << caseNo;
        CMN_LOG_INIT_ERROR << cmnPrintf("I:\n%.14f\n") << inputCopy;
        CMN_LOG_INIT_ERROR << cmnPrintf("O:\n%.14f\n") << output;
        CMN_LOG_INIT_ERROR << cmnPrintf("E:\n%.14f\n") << errorv;
    }
    CPPUNIT_ASSERT(error < tolerance);
}

void nmrPInverseTest::TestCompareWithMatlab(void) {
    // peters matrix
    double M1[] = {0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 59.03510000000000, -3.60091000000000, 1.00000000000000, 0.00000000000000, 0.00000000000000, -58.25410000000000, -9.57080000000000, 1.38779000000000, -0.16212100000000, 0.98677100000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 3.60091000000000, -0.81478700000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 1.00000000000000};
    // ankurs knot matix 1
    double M2[] = {0.38762756430421, 0.00000000000000, -0.35355339059327, 1.16984155512169, 0.00000000000000, -0.09805806756909, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, -0.61237243569579, 0.00000000000000, 0.64644660940673, 0.84920777560845, 0.00000000000000, 0.50970966215454, -0.23223304703363, 0.00000000000000, 0.40223943665878, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, -0.72524512160180, 0.0000000000000, -1.25616139855579};
    // ankurs knot matix 1
    double M3[] = { 0.98610833190000, 0.00000000000000, -0.00802035830000, 0.96760462570000, 0.00000000000000, 0.01870347810000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.86591398050000, 0.00000000000000, 1.49993566970000, -0.86541928550000, 0.00000000000000, 1.49965005740000, 16.70240626230000, 0.00000000000000, -28.92941625500000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 16.71309701790000, 0.00000000000000, 28.94793318680000 };
    // random matlab matrix (uniform random -10, -10, singular values   135.3644   95.1430   67.1745    0.0000, cond 1.6749e+016)
    double M4[] = {-3.37875950216125, 80.26502550855749, 60.73127730014663, 41.37148129078469, 57.92685559525665, 27.12114705151928, -31.03604625888865, -48.63691658149605, -9.07468098913988, -3.56306202808914, 44.61542760737831, -38.04855987473170, 66.62025314842755, 17.33334497939464, -27.61862180722743, 44.18041474603763, 8.89389811598840, -6.16870979172485, 22.17878982314258, 31.91794326100866};
    // random matlab matrix (uniform random -10, -10, singular values  201.2198  158.8004    3.0000    0.0000, cond 9.3386e+015)
    double M5[] = {-27.62210355170828, 43.06224164388668, -30.19569455529031, 38.21262574305315, -20.31541047405453, 102.62372316750051, -58.29586129711694, -32.74264552218162, 27.06835501427010, 60.22221714399455, -26.30043356159047, -112.43070092802198, 10.87664256665879, -42.96039205648545, 23.92471584575928, 11.28474004041303, -48.65035301668470, 28.31086892733934, -26.96175707840265, 101.14820952533759, 16.59703868533411, -11.23645540260741, 9.82476834831108, -32.55792621914553, -6.93680001555010, -48.06475479354379, 23.75117394819212, 55.39529717773785, -3.85959075182361, 44.99505087298729, -25.01151448725986, -24.81382455796729};
    // snake jacobian (singular values 1.4145    1.0000    1.0000    1.0000    0.9999    0.0000, cond 1.4067e+016)
    double M6[] = {1.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 1.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 1.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, -1.00000000000000, -0.02031657669066, -0.00703467920893, 0.00000000000000, -0.32719438181049, 0.94495705537958, 0.00000000000000, -0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, -0.00472478527690, -0.00163597190905, 0.00000000000000, -0.32719438181049, 0.94495705537958, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000};
    // random matlab matrix (singular values 32.2260   10.4982    8.7866    7.4058    3.2680    0.0000, cond 1.8876e+006 Rank < M = N )
    double M7[] = {4.27876101806760, 0.67921138834208, 1.78180468734354, 4.82638439629227, 9.30874330457300, 9.61149367038160, 8.51188653614371, 9.40997010096907, 3.49328758195042, 4.21468315646052, 0.15167900826782, 4.81684950646013, 6.57013676129281, 7.77091971598565, 4.44336262531578, 4.55745559185743, 8.80506069399416, 6.87171153258532, 4.81291497591883, 1.46516374312341, 6.72693426720798, 8.31984641030430, 0.77658376656472, 6.18163043167442, 8.22652976028621, 1.68455753475427, 2.52784089185297, 1.56509181484580, 0.53808848839253, 9.62976448237896, 4.37572035007178, 5.12460481841117, 8.26179133262485, 4.86431351397186, 8.35982499644160, 5.09824129752814};

    double PM1[] = {0.00000000000000, 0.00000000000000, -0.00000000000000, -0.00000000000000, 1.01340635263906, -0.00000000000000, 0.00000000000000, -0.00000000000000, 0.00025329327584, 0.00000545220215, -0.00000552529629, 0.24618707734121, 0.00000000000000, 0.00000000000000, -0.00005731330923, -0.00000123368355, 0.00000125022275, -0.05570537174926, 0.00000000000000, 0.00000000000000, 0.00039417561639, -0.01671496712255, 0.01693905386615, -0.00008820357336, 0.00000000000000, 0.00000000000000, 0.00006476069477, -0.00274616906512, 0.00278298517601, -0.00001449131924, 0.00000000000000, -0.00000000000000, 0.01688059313122, -0.00000125884200, 0.00000127571848, 0.00415471655227, 0.00000000000000, -0.00000000000000, -0.00095878442920, -0.00002063807857, 0.00002091475992, 0.06811447859981};
    double PM2[] = {1.40440413686265, -0.00000000000000, -3.10390335410810, 6.22276041437401, -1.36909372291397, 0.00000000000000, -0.00000000000000, -0.00000000000000, 0.00000000000000, -0.00000000000000, 0.81083310646868, -0.00000000000000, -1.79203943703289, 6.07879384552872, -0.79044662947021, 0.60522953789709, 0.00000000000000, 0.46440895835050, -0.21438634165111, 0.14119689917583, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, -0.34942943662639, -0.00000000000000, -0.26812663711773, 0.12377601206285, -0.87759611430506};
    double PM3[] = {0.51582327058759, 0.00000000000000, 0.28508013404696, 0.01463786101308, -0.01510187707741, -0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, -0.00000000000000, 0.29781070412761, 0.00000000000000, 0.16459109213240, -0.02611571875108, -0.00871907279590, 0.51601979624040, 0.00000000000000, -0.29243103294103, -0.01530505824896, 0.01481600940591, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, -0.29792416826689, -0.00000000000000, 0.16883513558808, 0.00883637950001, 0.02599075355487};
    double PM4[] = {0.00036713893084, 0.00603714392180, 0.00483318341059, 0.00953021137414, 0.00455985528008, -0.00172922108976, -0.00441471014191, 0.00005055843852, 0.00257040363786, -0.00547966962690, 0.00722518392769, -0.00392000448623, 0.00243825439167, 0.00101163365351, 0.00176374265165, 0.00356853133008, 0.00377570057404, 0.00272705966159, -0.00326235168820, 0.00576732431912};
    double PM5[] = {0.12063468476910, -0.09157115578641, 0.12675102489908, 0.17707379993462, 0.06121796537863, 0.00653101715176, -0.00466285128254, 0.04651393423276, -0.00238530273809, 0.00618054801307, -0.00297409002530, -0.00722854781630, -0.00048953649625, -0.00078618182767, -0.00105869217906, -0.00024520987112, -0.07178111904241, 0.05068101818351, -0.07347659637680, -0.10163534430935, -0.03788170146542, -0.00296041780710, 0.00321573627308, -0.02787328150067, 0.04342211944055, -0.03134912409106, 0.04025249534962, 0.06073167689636, 0.02510034180782, 0.00091271305521, -0.00020362480073, 0.01566700015990};
    double PM6[] = {0.99987846456427, -0.00004208203065, -0.00000000000000, 0.00000000000000, -0.00779483463502, -0.00000000000000, 0.00779483463502, 0.00000000000000, -0.00004208203065, 0.99998542896323, -0.00000000000000, 0.00000000000000, -0.00269898625043, 0.00000000000000, 0.00269898625043, -0.00000000000000, 0.00000000000000, 0.00000000000000, 1.00000000000000, 0.00000000000000, 0.00000000000000, -0.00000000000000, -0.00000000000000, -0.00000000000000, -0.00409613888743, -0.00141830109998, 0.00000000000000, 0.00000000000000, -0.16356142933742, -0.00000000000000, -0.16363295247306, -0.00000000000000, 0.01182989548926, 0.00409613888743, 0.00000000000000, 0.00000000000000, 0.47237524613088, 0.00000000000000, 0.47258180924871, 0.00000000000000, 0.00000000000000, 0.00000000000000, 0.00000000000000, -1.00000000000000, 0.00000000000000, 0.00000000000000, -0.00000000000000, 0.00000000000000};
    double PM7[] = {15639.99434605071300, 17531.81781251241800, -28882.36697847368900, -9277.60018779141320, -5163.22567082070600, 13881.41484836513000, -8463.61929788820270, -9487.26703751126840, 15629.76008496284200, 5020.53742779452200, 2794.06745168262610, -7511.95519974410940, -2104.53622833662300, -2359.03982907988070, 3886.19986889874510, 1248.34763506363860, 694.81291243209432, -1867.63941984174270, 2624.77208189867450, 2942.22192907889080, -4847.00093934273170, -1556.82701513163490, -866.62971754441526, 2329.45841298685130, 4889.62884400544770, 5480.98869294407000, -9029.54549917764600, -2900.52481615242640, -1614.24399308530840, 4339.82195548426440, -12027.76610468493200, -13482.67666210826800, 22211.69820014938200, 7134.83305384457620, 3970.83926891883360, -10675.38890021627600};

    GenericTestCompareWithMatlab<7, 6>(M1, PM1, 1);
    GenericTestCompareWithMatlab<6, 5>(M2, PM2, 2);
    GenericTestCompareWithMatlab<6, 5>(M3, PM3, 3);
    GenericTestCompareWithMatlab<5, 4>(M4, PM4, 4);
    GenericTestCompareWithMatlab<4, 8>(M5, PM5, 5);
    GenericTestCompareWithMatlab<6, 8>(M6, PM6, 6);
    GenericTestCompareWithMatlab<6, 6>(M7, PM7, 7, 1.1601e-005); // lower expectation for this case because of very bad conditioning, 10 times lower than others
}

CPPUNIT_TEST_SUITE_REGISTRATION(nmrPInverseTest);

