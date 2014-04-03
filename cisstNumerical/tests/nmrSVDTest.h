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


#ifndef _nmrSVDTest_h
#define _nmrSVDTest_h

#include <cisstCommon/cmnPortability.h>

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstVector/vctRandomFixedSizeMatrix.h>
#include <cisstVector/vctRandomDynamicMatrix.h>

#include <cisstNumerical/nmrSVD.h>
#include <cisstNumerical/nmrIsOrthonormal.h>

#ifdef BUILD_MULTITHREADED_TESTS

#include <cisstOSAbstraction/osaThread.h>

struct argument {long int times, number;};

template<unsigned int _row, unsigned int _col, bool _storageOrder>
class nmrSVDFixedSizeTestObject
{
    typedef nmrSVDFixedSizeData<_row, _col, _storageOrder> DataType;
    typedef typename DataType::MatrixTypeA MatrixTypeA;
    typedef typename DataType::MatrixTypeU MatrixTypeU;
    typedef typename DataType::MatrixTypeVt MatrixTypeVt;
    typedef typename DataType::VectorTypeS VectorTypeS;
    typedef typename DataType::MatrixTypeS MatrixTypeS;

    /* Sub templated to allow MatrixRef ... */
    template <class _matrixTypeU, class _vectorType, class _matrixTypeVt>
    void GenericTestFixedSize(MatrixTypeA & input, _matrixTypeU &U, _vectorType &S, _matrixTypeVt &Vt)
    {
        MatrixTypeS matrixS;
        DataType::UpdateMatrixS(S, matrixS);
        /* Recompose */
        MatrixTypeA product;
        product.ProductOf(U, matrixS * Vt);
        /* Compare initial with result */
        double error = (input - product).LinfNorm();
        CPPUNIT_ASSERT(error < cmnTypeTraits<double>::Tolerance());
        /* Make sure that both U and V are orthonormal */
        CPPUNIT_ASSERT(nmrIsOrthonormal(U));
        CPPUNIT_ASSERT(nmrIsOrthonormal(Vt));
    }
public:
    MatrixTypeA input;
    MatrixTypeA inputOrig;
    MatrixTypeU U;
    MatrixTypeVt Vt;
    VectorTypeS S;
    
    nmrSVDFixedSizeTestObject() {}
    ~nmrSVDFixedSizeTestObject() {}
    void *RunThread(argument arg)
    {
        CMN_LOG_INIT_ERROR << "nmrSVDTest: Running thread #" << arg.number << ", " << arg.times << " iterations" << std::endl;
        for (int i = 0; i < arg.times; i++) {
            vctRandom(input, 0.0, 10.0);
            inputOrig.Assign(input);
            nmrSVD(input, U, S, Vt);
            GenericTestFixedSize(inputOrig, U, S, Vt);
        }
        return this;
    }
};

class nmrSVDDynamicTestObject
{
    template <class _inputOwner, class _UOwner, class _SOwner, class _VtOwner>
    void GenericTestDynamic(const vctDynamicConstMatrixBase<_inputOwner, double> & input,
                            const vctDynamicConstMatrixBase<_UOwner, double> &U,
                            const vctDynamicConstVectorBase<_SOwner, double> &S,
                            const vctDynamicConstMatrixBase<_VtOwner, double> &Vt) 
    {
        vctDynamicMatrix<double> matrixS(input.rows(), input.cols(), input.IsRowMajor());
        nmrSVDDynamicData::UpdateMatrixS(input, S, matrixS);
        /* Recompose */
        vctDynamicMatrix<double> product(input.rows(), input.cols(), input.IsRowMajor());
        product = U * (matrixS * Vt);
        /* Compare initial with result */
        double error = (input- product).LinfNorm();
        CPPUNIT_ASSERT(error < cmnTypeTraits<double>::Tolerance());
        /* Make sure that both U and V are orthonormal */
        const unsigned int size = (std::max)(U.rows(), Vt.rows());
        nmrIsOrthonormalDynamicData<double> orthoData(size);
        CPPUNIT_ASSERT(nmrIsOrthonormal(U, orthoData));
        CPPUNIT_ASSERT(nmrIsOrthonormal(Vt, orthoData));
    }
public:
    int rows, cols;
    vctDynamicMatrix<double> input;
    vctDynamicMatrix<double> inputOrig;
    
    nmrSVDDynamicTestObject() {}
    ~nmrSVDDynamicTestObject() {}
    void *RunThread(argument arg)
    {
        CMN_LOG_INIT_ERROR << "nmrSVDTest: Running thread #" << arg.number << ", " << arg.times << " iterations" << std::endl;
        for (int i = 0; i < arg.times; i++) {
            cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
            randomSequence.ExtractRandomValue(10, 20, rows);
            randomSequence.ExtractRandomValue(10, 20, cols);
            input.SetSize(rows, cols, VCT_COL_MAJOR);
            inputOrig.SetSize(rows, cols, VCT_COL_MAJOR);
            vctRandom(input, 0.0, 10.0);
            inputOrig.Assign(input);
            nmrSVDDynamicData svdData;
            svdData.Allocate(input);
            nmrSVD(input, svdData);
            GenericTestDynamic(inputOrig, svdData.U(), svdData.S(), svdData.Vt());
        }
        return this;
    }
};
#endif

class nmrSVDTest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(nmrSVDTest);
    
    CPPUNIT_TEST(TestDynamicColumnMajor);
    CPPUNIT_TEST(TestDynamicRowMajor);
    
    CPPUNIT_TEST(TestDynamicColumnMajorUserAlloc);
    CPPUNIT_TEST(TestDynamicRowMajorUserAlloc);
    
    CPPUNIT_TEST(TestFixedSizeColumnMajorMLeqN);
    CPPUNIT_TEST(TestFixedSizeRowMajorMLeqN);

    CPPUNIT_TEST(TestFixedSizeColumnMajorMGeqN);
    CPPUNIT_TEST(TestFixedSizeRowMajorMGeqN);

    CPPUNIT_TEST(TestFixedSizeColumnMajorMLeqN_T2);
    CPPUNIT_TEST(TestFixedSizeRowMajorMLeqN_T2);

    CPPUNIT_TEST(TestFixedSizeColumnMajorMGeqN_T2);
    CPPUNIT_TEST(TestFixedSizeRowMajorMGeqN_T2);

#ifdef BUILD_MULTITHREADED_TESTS
    CPPUNIT_TEST(TestThreadSafetyDynamic);
    CPPUNIT_TEST(TestThreadSafetyFixedSize);
#endif

    CPPUNIT_TEST_SUITE_END();

public:
    void setUp()
    {}
    
    void tearDown()
    {}

    /*! Templated generic test */
    template <class _matrixTypeI, class _matrixTypeU, class _vectorType, class _matrixTypeVt>
    inline void GenericTestFixedSize(_matrixTypeI & input, _matrixTypeU &U, _vectorType &S, _matrixTypeVt &Vt);

    template <class _inputOwner, class _UOwner, class _SOwner, class _VtOwner>
    inline void GenericTestDynamic(const vctDynamicConstMatrixBase<_inputOwner, double> & input,
                                   const vctDynamicConstMatrixBase<_UOwner, double> &U,
                                   const vctDynamicConstVectorBase<_SOwner, double> &S,
                                   const vctDynamicConstMatrixBase<_VtOwner, double> &Vt) 
    {
        vctDynamicMatrix<double> matrixS(nmrSVDDynamicData::MatrixSSize(input));
        nmrSVDDynamicData::UpdateMatrixS(input, S, matrixS);
        /* Recompose */
        vctDynamicMatrix<double> product(input.rows(), input.cols(), input.StorageOrder());
        product = U * (matrixS * Vt);
        /* Compare initial with result */
        double error = (input - product).LinfNorm();
        CPPUNIT_ASSERT(error < cmnTypeTraits<double>::Tolerance());
        /* Make sure that both U and V are orthonormal */
        CPPUNIT_ASSERT(nmrIsOrthonormal(U));
        CPPUNIT_ASSERT(nmrIsOrthonormal(Vt));
    }

    /*! Test using column major matrices. */
    void TestDynamicColumnMajor(void);

    /*! Test using column major matrices. */
    void TestDynamicRowMajor(void);

    /*! Test using column major matrices. */
    void TestDynamicColumnMajorUserAlloc(void);

    /*! Test using column major matrices. */
    void TestDynamicRowMajorUserAlloc(void);

    void TestFixedSizeColumnMajorMLeqN(void);

    void TestFixedSizeRowMajorMLeqN(void);

    void TestFixedSizeColumnMajorMGeqN(void);

    void TestFixedSizeRowMajorMGeqN(void);

    void TestFixedSizeColumnMajorMLeqN_T2(void);

    void TestFixedSizeRowMajorMLeqN_T2(void);

    void TestFixedSizeColumnMajorMGeqN_T2(void);

    void TestFixedSizeRowMajorMGeqN_T2(void);

#ifdef BUILD_MULTITHREADED_TESTS
    void TestThreadSafetyDynamic(void);
    void TestThreadSafetyFixedSize(void);
#endif
};


#endif // _nmrSVDTest_h

