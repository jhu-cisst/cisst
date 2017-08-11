/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2017-08-10

  (C) Copyright 2017 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "vctEigen3ConversionTest.h"

#include <cisstVector/vctDynamicMatrix.h>
#include <cisstVector/vctRandom.h>

#include <Eigen/Dense>

CPPUNIT_TEST_SUITE_REGISTRATION(vctEigen3ConversionTest);

void vctEigen3ConversionTest::TestCisstDynamicMatrixToEigenBlock(void)
{
    std::cerr << "----------------------" << std::endl;
    const size_t nbRows = 4;
    const size_t nbCols = 3;
    vctDynamicMatrix<double> mCr(nbRows, nbCols, VCT_ROW_MAJOR);
    vctRandom(mCr, -10.0, 10.0);

    std::cerr << "cisst" << std::endl << mCr << std::endl;

    typedef  Eigen::Map<Eigen::MatrixXd,
                        0,
                        Eigen::Stride<Eigen::Dynamic, Eigen::Dynamic> > vctEigenDoubleMatRef;

    vctEigenDoubleMatRef map = vctEigenDoubleMatRef(mCr.Pointer(),
                                                    mCr.rows(), mCr.cols(),
                                                    Eigen::Stride<Eigen::Dynamic, Eigen::Dynamic>(mCr.col_stride(), mCr.row_stride()));

    std::cerr << "map: " << std::endl << map << std::endl;
    map(1, 1) = 1000;
    std::cerr << "cisst" << std::endl << mCr << std::endl;

    std::cerr << "-------------------------" << std::endl;
    
    /* 
       int array[8];
       for(int i = 0; i < 8; ++i) array[i] = i;
       cout << "Column-major:\n" << Map<Matrix<int,2,4> >(array) << endl;
       cout << "Row-major:\n" << Map<Matrix<int,2,4,RowMajor> >(array) << endl;
       cout << "Row-major using stride:\n" <<
       Map<Matrix<int,2,4>, Unaligned, Stride<1,4> >(array) << endl;/     CPPUNIT_ASSERT(point.X() == (i % dataElements - dataElements));
       
    */
}

void vctEigen3ConversionTest::TestEigenMatrixToCisstRef(void)
{
    // default Eigen matrix
    const size_t nbRows = 4;
    const size_t nbCols = 3;
    Eigen::MatrixXd mE(nbRows, nbCols);
    for (size_t row = 0; row < nbRows; row++) {
        for (size_t col = 0; col < nbCols; col++) {
            mE(row, col) = 10.0 * row + col + 0.1;
        }
    }

    std::cerr << "first element " << mE.data()[0] << std::endl;

    std::cerr << "nb row " << mE.rows() << std::endl;
    std::cerr << "nb col " << mE.cols() << std::endl;
    std::cerr << "row major " << mE.IsRowMajor << std::endl;

    vctDynamicMatrixRef<double> ref;
    ref.SetRef(mE.rows(), mE.cols(), mE.data(),
               mE.IsRowMajor ? VCT_ROW_MAJOR : VCT_COL_MAJOR);

    std::cerr << "cisst Ref" << std::endl << ref << std::endl;

    ref.Add(0.1);
    mE(1, 1) = 0.0;
    mE.cwiseAdd(1000.0);
    std::cerr << "Eigen matrix" << std::endl << mE << std::endl;
    std::cerr << "cisst Ref" << std::endl << ref << std::endl;
}
