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


#ifndef _nmrInverseTest_h
#define _nmrInverseTest_h

#include <cisstCommon/cmnPortability.h>

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstVector/vctRandomFixedSizeMatrix.h>
#include <cisstVector/vctRandomDynamicMatrix.h>
#include <cisstNumerical/nmrInverse.h>

#ifdef BUILD_MULTITHREADED_TESTS

#include <cisstOSAbstraction/osaThread.h>

struct argument {long int times, number;};

template<unsigned int _size, bool _storageOrder>
class nmrInverseFixedSizeTestObject
{
    typedef nmrInverseFixedSizeData<_size, _storageOrder> DataType;
    typedef typename DataType::MatrixTypeA MatrixTypeA;
    typedef typename DataType::VectorTypePivotIndices VectorTypePivotIndices;
    typedef typename DataType::VectorTypeWorkspace VectorTypeWorkspace;

    void GenericTestFixedSize(const MatrixTypeA & A, const MatrixTypeA & AInv)
    {
        /* Recompose */
        MatrixTypeA product;
        product.ProductOf(A, AInv);
        /* Compare initial with result */
        product.Diagonal().Subtract(1.0);
        double error = product.LinfNorm();
        CPPUNIT_ASSERT(error < cmnTypeTraits<double>::Tolerance());
    }

public:
    MatrixTypeA A;
    MatrixTypeA AInv;
    VectorTypePivotIndices pivotIndices;
    VectorTypeWorkspace workspace;
    
    nmrInverseFixedSizeTestObject() {}
    ~nmrInverseFixedSizeTestObject() {}
    void *RunThread(argument arg)
    {
        CMN_LOG_INIT_ERROR << "nmrInverseTest: Running thread #" << arg.number << ", " << arg.times << " iterations" << std::endl;
        for (int i = 0; i < arg.times; i++) {
            vctRandom(A, 0.0, 10.0);
            AInv.Assign(A);
            nmrInverse(AInv, pivotIndices, workspace);
            GenericTestFixedSize(A, AInv);
        }
        return this;
    }
};



class nmrInverseDynamicTestObject
{
    void GenericTestDynamic(const vctDynamicMatrix<double> & input,
                            const vctDynamicMatrix<double> & inverse)
    {
        /* Recompose */
        vctDynamicMatrix<double> product(input.rows(), input.cols(), input.IsRowMajor());
        product = input * inverse;
        /* Compare initial with identity */
        product.Diagonal().Subtract(1.0);
        double error = product.LinfNorm();
        CPPUNIT_ASSERT(error < cmnTypeTraits<double>::Tolerance());
    }
public:
    int size;
    bool storageOrder;
    vctDynamicMatrix<double> A;
    vctDynamicMatrix<double> AInv;
    
    nmrInverseDynamicTestObject() {}
    ~nmrInverseDynamicTestObject() {}
    void *RunThread(argument arg)
    {
        CMN_LOG_INIT_ERROR << "nmrInverseTest: Running thread #" << arg.number << ", " << arg.times << " iterations" << std::endl;
        for (int i = 0; i < arg.times; i++) {
            cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
            randomSequence.ExtractRandomValue(10, 20, size);
            randomSequence.ExtractRandomValue(storageOrder);            
            A.SetSize(size, size, storageOrder);
            AInv.SetSize(size, size, storageOrder);
            vctRandom(A, 0.0, 10.0);
            AInv.Assign(A);
            nmrInverseDynamicData invData;
            invData.Allocate(AInv);
            nmrInverse(AInv, invData);
            GenericTestDynamic(A, AInv);
        }
        return this;
    }
};

#endif // BUILD_MULTITHREADED_TESTS


class nmrInverseTest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(nmrInverseTest);
    
    CPPUNIT_TEST(TestDynamicColumnMajor);
    CPPUNIT_TEST(TestDynamicRowMajor);
    
    CPPUNIT_TEST(TestDynamicColumnMajorUserAlloc);
    CPPUNIT_TEST(TestDynamicRowMajorUserAlloc);
    
    CPPUNIT_TEST(TestFixedSizeColumnMajor);
    CPPUNIT_TEST(TestFixedSizeRowMajor);

    CPPUNIT_TEST(TestFixedSizeColumnMajorUserAlloc);
    CPPUNIT_TEST(TestFixedSizeRowMajorUserAlloc);

#ifdef BUILD_MULTITHREADED_TESTS
    CPPUNIT_TEST(TestThreadSafetyDynamic);
    CPPUNIT_TEST(TestThreadSafetyFixedSize);
#endif // BUILD_MULTITHREADED_TESTS

    CPPUNIT_TEST_SUITE_END();

public:
    void setUp()
    {}
    
    void tearDown()
    {}

    /*! Templated generic test */
    template <class _matrixTypeA, class _matrixTypeAInverse>
    inline void GenericTestFixedSize(const _matrixTypeA & A,
                                     const _matrixTypeAInverse & AInv);

    void inline GenericTestDynamic(const vctDynamicMatrix<double> & input,
                                   const vctDynamicMatrix<double> & inverse);

    /*! Test using column major matrices. */
    void TestDynamicColumnMajor(void);

    /*! Test using column major matrices. */
    void TestDynamicRowMajor(void);

    /*! Test using column major matrices. */
    void TestDynamicColumnMajorUserAlloc(void);

    /*! Test using column major matrices. */
    void TestDynamicRowMajorUserAlloc(void);

    /*! Test using column major matrices. */
    void TestFixedSizeColumnMajor(void);

    /*! Test using column major matrices. */
    void TestFixedSizeRowMajor(void);

    /*! Test using column major matrices. */
    void TestFixedSizeColumnMajorUserAlloc(void);

    /*! Test using column major matrices. */
    void TestFixedSizeRowMajorUserAlloc(void);

#ifdef BUILD_MULTITHREADED_TESTS
    void TestThreadSafetyDynamic(void);
    void TestThreadSafetyFixedSize(void);
#endif // BUILD_MULTITHREADED_TESTS
};


#endif // _nmrInverseTest_h

