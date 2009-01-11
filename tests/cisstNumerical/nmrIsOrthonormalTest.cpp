/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrIsOrthonormalTest.cpp,v 1.12 2007/04/26 20:12:05 anton Exp $
  
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


#ifdef CISST_HAS_NETLIB
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
    } catch (std::runtime_error exception) {
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
    } catch (std::runtime_error exception) {
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

#endif // CISST_HAS_NETLIB


CPPUNIT_TEST_SUITE_REGISTRATION(nmrIsOrthonormalTest);


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrIsOrthonormalTest.cpp,v $
// Revision 1.12  2007/04/26 20:12:05  anton
// All files in tests: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
//
// Revision 1.11  2007/02/12 03:25:03  anton
// cisstVector and cisstNumerical tests: Used more refined #include for vctRandom
// (see ticket #264).
//
// Revision 1.10  2007/01/26 21:23:22  anton
// cisstVector:  Updated vctMatrixRotation3 code to reflect new design (i.e. avoid
// derived class and use typedef instead, see ticket #263.
//
// Revision 1.9  2006/11/20 20:33:52  anton
// Licensing: Applied new license to tests.
//
// Revision 1.8  2006/01/27 01:00:42  anton
// cisstNumerical tests: Renamed "solution" to "data" (see #205).
//
// Revision 1.7  2006/01/11 20:53:17  anton
// cisstNumerical Code: Introduced CISST_HAS_NETLIB to indicate that any
// netlib distribution is available (either cnetlib or cisstNetlib).
//
// Revision 1.6  2006/01/04 22:36:36  anton
// nmrIsOrthonormalTest: Added test for fixed size functions and solution.
//
// Revision 1.5  2006/01/03 22:42:35  anton
// nmrIsOrthonormalTest: Added tests for new signatures using nmrIsOrthonormalSolutionDynamic.
// Also use setUp() to create a couple of orthonormal matrices using SVD.
//
// Revision 1.4  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.3  2005/09/06 15:41:37  anton
// cisstNumerical tests: Added license.
//
// Revision 1.2  2005/08/04 19:25:46  anton
// nmrIsOrthonormalTest: More subtile includes
//
// Revision 1.1  2005/07/27 22:40:33  anton
// cisstNumerical tests: Added some basic tests for nmrIsOthonormal.
//
// ****************************************************************************
