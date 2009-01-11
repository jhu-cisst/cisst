/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrSVDTest.h,v 1.17 2008/09/09 15:10:58 anton Exp $
  
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
        CMN_LOG(1) << "nmrSVDTest: Running thread #" << arg.number << ", " << arg.times << " iterations" << std::endl;
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
        const unsigned int size = std::max(U.rows(), Vt.rows());
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
        CMN_LOG(1) << "nmrSVDTest: Running thread #" << arg.number << ", " << arg.times << " iterations" << std::endl;
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


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrSVDTest.h,v $
// Revision 1.17  2008/09/09 15:10:58  anton
// Removed some reference to now obsolete osaTime.h
//
// Revision 1.16  2007/08/28 14:49:26  anton
// cisstNumericalTests: Removed sprintf and code to print to cout or cerr.
// Also removed commented code to cleanup.
//
// Revision 1.15  2007/05/18 21:56:36  anton
// cisstNumerical tests: Modified signature of test methods to avoid implicit
// conversion between containers and containersRef as these constructors are
// now "explicit".   See [2349] by Ofri.
//
// Revision 1.14  2007/04/26 20:12:05  anton
// All files in tests: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
//
// Revision 1.13  2007/02/12 03:25:03  anton
// cisstVector and cisstNumerical tests: Used more refined #include for vctRandom
// (see ticket #264).
//
// Revision 1.12  2006/11/20 20:33:52  anton
// Licensing: Applied new license to tests.
//
// Revision 1.11  2006/10/06 14:32:51  anton
// All tests: Re-ordered #include to load cmnPortability.h before any system
// include.
//
// Revision 1.10  2006/01/27 01:00:42  anton
// cisstNumerical tests: Renamed "solution" to "data" (see #205).
//
// Revision 1.9  2006/01/09 22:46:01  anton
// nmrSVD: Added method UpdateMatrixS for dynamic containers.
//
// Revision 1.8  2006/01/09 04:47:32  anton
// nmrSVD:  Added helper function UpdateMatrixS for fixed size.
//
// Revision 1.7  2006/01/05 03:54:40  anton
// cisstNumerical tests: Compile multithreaded tests only if cisstOSAbstraction
// has been compiled instead of requiring cisstOSAbstraction when cisstNetlib
// is used.  See ticket #200.
//
// Revision 1.6  2006/01/04 22:37:42  anton
// nmrSVDTest: nmrIsOrthonormal solution now requires only the number of rows
// since the matrix has to be square.
//
// Revision 1.5  2006/01/03 22:44:11  anton
// nmrSVDTest.h: Now use nmrIsOrthonormalSolutionDynamic to avoid useless
// memory allocation.
//
// Revision 1.4  2005/12/20 16:39:51  anton
// nmrSVDTest: Updated to reflect the new nmrSVD API.
//
// Revision 1.3  2005/11/29 03:11:34  anton
// nmrSVDTest.h: Changed std::cout to CMN_LOG, indentation.
//
// Revision 1.2  2005/11/20 21:23:41  kapoor
// nmrSVD: Changes to the netlib wrappers. See wiki pages SvdNotes and ticket #184.
//
// Revision 1.1  2005/10/21 23:14:31  anton
// nmrSVD: Added tests from Ankur.
//
//
// ****************************************************************************
