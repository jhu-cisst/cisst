/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2005-07-27
  
  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "nmrIsOrthonormalTest.h"

#include <cisstVector/vctTransformationTypes.h>
#include <cisstVector/vctRandomTransformations.h>

void nmrIsOrthonormalTest::TestRotationMatrix(void) {
    vctMatRot3 matrix;
    vctRandom(matrix);

    CPPUNIT_ASSERT(nmrIsOrthonormal(matrix));
    CPPUNIT_ASSERT(nmrIsOrthonormal(matrix.TransposeRef()));

    vctDynamicMatrixRef<double> matrixRef(matrix);
    CPPUNIT_ASSERT(nmrIsOrthonormal(matrixRef));
    CPPUNIT_ASSERT(nmrIsOrthonormal(matrixRef.TransposeRef()));

    matrix[1][1] = 100.0;
    CPPUNIT_ASSERT(! nmrIsOrthonormal(matrix));
    CPPUNIT_ASSERT(! nmrIsOrthonormal(matrix.TransposeRef()));
    CPPUNIT_ASSERT(! nmrIsOrthonormal(matrixRef));
    CPPUNIT_ASSERT(! nmrIsOrthonormal(matrixRef.TransposeRef()));
}


#ifdef CISST_HAS_CISSTNETLIB
void nmrIsOrthonormalTest::TestFunctionDynamic(void) {
    CPPUNIT_ASSERT(nmrIsOrthonormal(UDynamic));
    CPPUNIT_ASSERT(nmrIsOrthonormal(UDynamic.TransposeRef()));

    CPPUNIT_ASSERT(nmrIsOrthonormal(VtDynamic));
    CPPUNIT_ASSERT(nmrIsOrthonormal(VtDynamic.TransposeRef()));

    vctDynamicMatrix<double> UCopy(UDynamic);
    UCopy(0, 0) += 1.0;
    CPPUNIT_ASSERT(!nmrIsOrthonormal(UCopy));
}


void nmrIsOrthonormalTest::TestFunctionWithReferenceDynamic(void) {
    // try with the correct size
    typedef nmrIsOrthonormalDynamicData<double> DataType;
    vctDynamicVector<double> workspace(DataType::WorkspaceSize(UDynamic));
    CPPUNIT_ASSERT(nmrIsOrthonormal(UDynamic, workspace));
    CPPUNIT_ASSERT(nmrIsOrthonormal(UDynamic.TransposeRef(), workspace));

    // try with a smaller workspace
    workspace.SetSize(DataType::WorkspaceSize(UDynamic) - 1);
    bool gotException = false;
    try {
        nmrIsOrthonormal(UDynamic, workspace);
    } catch (std::runtime_error & exception) {
        gotException = true;            
    }
    CPPUNIT_ASSERT(gotException);
}


void nmrIsOrthonormalTest::TestDynamicData(void) {
    typedef nmrIsOrthonormalDynamicData<double> DataType;
    DataType data(UDynamic);
    CPPUNIT_ASSERT(nmrIsOrthonormal(UDynamic, data));
    CPPUNIT_ASSERT(nmrIsOrthonormal(UDynamic.TransposeRef(), data));

    data.Allocate(VtDynamic);
    CPPUNIT_ASSERT(nmrIsOrthonormal(VtDynamic, data));
    CPPUNIT_ASSERT(nmrIsOrthonormal(VtDynamic.TransposeRef(), data));
}


void nmrIsOrthonormalTest::TestDynamicDataWithReferenceDynamic(void) {
    // try with the correct size
    typedef nmrIsOrthonormalDynamicData<double> DataType;
    vctDynamicVector<double> workspace(DataType::WorkspaceSize(UDynamic));
    DataType data(UDynamic, workspace);
    CPPUNIT_ASSERT(nmrIsOrthonormal(UDynamic, data));
    CPPUNIT_ASSERT(nmrIsOrthonormal(UDynamic.TransposeRef(), data));

    // try with a smaller workspace
    workspace.SetSize(DataType::WorkspaceSize(UDynamic) - 1);
    bool gotException = false;
    try {
        data.SetRef(UDynamic, workspace);
        nmrIsOrthonormal(UDynamic, data);
    } catch (std::runtime_error & exception) {
        gotException = true;            
    }
    CPPUNIT_ASSERT(gotException);
}


void nmrIsOrthonormalTest::TestFunctionFixedSize(void) {
    CPPUNIT_ASSERT(nmrIsOrthonormal(UFixedSize));
    CPPUNIT_ASSERT(nmrIsOrthonormal(UFixedSize.TransposeRef()));

    CPPUNIT_ASSERT(nmrIsOrthonormal(VtFixedSize));
    CPPUNIT_ASSERT(nmrIsOrthonormal(VtFixedSize.TransposeRef()));

    vctFixedSizeMatrix<double, SIZE, SIZE> UCopy(UFixedSize);
    UCopy(0, 0) += 1.0;
    CPPUNIT_ASSERT(!nmrIsOrthonormal(UCopy));
}


void nmrIsOrthonormalTest::TestFixedSizeData(void) {
    typedef nmrIsOrthonormalFixedSizeData<double, SIZE> DataType;
    DataType data;
    CPPUNIT_ASSERT(nmrIsOrthonormal(UFixedSize, data));
    CPPUNIT_ASSERT(nmrIsOrthonormal(UFixedSize.TransposeRef(), data));

    CPPUNIT_ASSERT(nmrIsOrthonormal(VtFixedSize, data));
    CPPUNIT_ASSERT(nmrIsOrthonormal(VtFixedSize.TransposeRef(), data));
}

#endif // CISST_HAS_CISSTNETLIB


CPPUNIT_TEST_SUITE_REGISTRATION(nmrIsOrthonormalTest);

