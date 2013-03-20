/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2012-07-09

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "vctGenericContainerTest.h"
#include "vctDataFunctionsDynamicMatrixTest.h"

#include <cisstVector/vctDynamicMatrix.h>
#include <cisstVector/vctDataFunctionsDynamicMatrix.h>
#include <cisstVector/vctRandomDynamicMatrix.h>

void vctDataFunctionsDynamicMatrixTest::TestDataCopy(void)
{
    vctDynamicMatrix<double> source, destination;
    source.SetSize(7, 3);
    vctRandom(source, -1.0, 1.0);
    cmnDataCopy(destination, source);
    CPPUNIT_ASSERT(source.Equal(destination));
}


void vctDataFunctionsDynamicMatrixTest::TestBinarySerializationStream(void)
{
    cmnDataFormat local, remote;
    std::stringstream stream;
    vctDynamicMatrix<double> m1, m2, mReference;
    m1.SetSize(12, 23);
    m2.SetSize(12, 23);
    mReference.SetSize(12, 23);
    vctRandom(mReference, -10.0, 10.0);
    m1 = mReference;
    cmnDataSerializeBinary(stream, m1);
    m1.SetAll(0);
    cmnDataDeSerializeBinary(stream, m2, remote, local);
    CPPUNIT_ASSERT_EQUAL(mReference, m2);
}


void vctDataFunctionsDynamicMatrixTest::TestTextSerializationStream(void)
{
    std::stringstream stream;
    vctDynamicMatrix<double> m1, m2, mReference;
    m1.SetSize(4, 5);
    mReference.SetSize(4,5);
    m2.SetSize(2, 3); // intentionally different
    vctRandom(mReference, -10.0, 10.0);
    m1 = mReference;
    cmnDataSerializeText(stream, m1, ',');
    m1.SetAll(0);
    cmnDataDeSerializeText(stream, m2, ',');
    CPPUNIT_ASSERT(mReference.AlmostEqual(m2, 0.01)); // low precision due to stream out loss
    CPPUNIT_ASSERT(!stream.fail());
    // try without delimiter, using space
    stream.clear();
    vctRandom(mReference, -20.0, 20.0);
    m1 = mReference;
    cmnDataSerializeText(stream, m1, ' ');
    m2.SetSize(2, 3); // intentionally different
    m2.SetAll(0.0);
    cmnDataDeSerializeText(stream, m2, ' ');
    CPPUNIT_ASSERT(mReference.AlmostEqual(m2, 0.01)); // low precision due to stream out loss
    CPPUNIT_ASSERT(!stream.fail());
    // try with the wrong delimiter
    bool exceptionReceived = false;
    stream.clear();
    vctRandom(mReference, -20.0, 20.0);
    m1 = mReference;
    cmnDataSerializeText(stream, m1, '|');
    m2.SetSize(2, 3); // intentionally different
    m2.SetAll(0.0);
    try {
        cmnDataDeSerializeText(stream, m2, ',');
    } catch (std::runtime_error) {
        exceptionReceived = true;
    }
    CPPUNIT_ASSERT(exceptionReceived);
}


void vctDataFunctionsDynamicMatrixTest::TestScalar(void)
{
    vctDynamicMatrix<int> mInt;
    mInt.SetSize(6, 3);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6 * 3), cmnDataScalarNumber(mInt));
    CPPUNIT_ASSERT_EQUAL(false, cmnDataScalarNumberIsFixed(mInt));

    vctDynamicMatrix<vctDynamicMatrix<double> > mmDouble;
    mmDouble.SetSize(4, 5);
    size_t numberOfScalarsExpected = 0;
    for (size_t i = 0; i < mmDouble.rows(); ++i) {
        for (size_t j = 0; j < mmDouble.cols(); ++j) {
            mmDouble.Element(i, j).SetSize(i + 1, j + 1); // + 1 to make sure we don't have a 0 size matrix
            numberOfScalarsExpected += ((i + 1) * (j + 1));
        }
    }
    CPPUNIT_ASSERT_EQUAL(numberOfScalarsExpected, cmnDataScalarNumber(mmDouble));
    CPPUNIT_ASSERT_EQUAL(false, cmnDataScalarNumberIsFixed(mmDouble));

    vctDynamicMatrix<std::string> mString;
    mString.SetSize(3, 2);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), cmnDataScalarNumber(mString));
    CPPUNIT_ASSERT_EQUAL(false, cmnDataScalarNumberIsFixed(mString));

    size_t row, col, subRow, subCol, position;
    bool exceptionReceived = false;
    std::string description;

    // exception expected if index too high
    try {
        description = cmnDataScalarDescription(mInt, cmnDataScalarNumber(mInt) + 1);
    } catch (std::out_of_range) {
        exceptionReceived = true;
    }
    CPPUNIT_ASSERT(exceptionReceived);

    exceptionReceived = false;
    try {
        description = cmnDataScalarDescription(mmDouble, cmnDataScalarNumber(mmDouble) + 1);
    } catch (std::out_of_range) {
        exceptionReceived = true;
    }
    CPPUNIT_ASSERT(exceptionReceived);

    // exception expected for any index
    exceptionReceived = false;
    try {
        description = cmnDataScalarDescription(mString, cmnDataScalarNumber(mString));
    } catch (std::out_of_range) {
        exceptionReceived = true;
    }
    CPPUNIT_ASSERT(exceptionReceived);

    // get scalar
    position = 0;
    for (row = 0; row < mInt.rows(); ++row) {
        for (col = 0; col < mInt.cols(); ++col) {
            mInt.Element(row, col) = static_cast<int>(row * 10 + col);
            CPPUNIT_ASSERT_EQUAL(static_cast<double>(row * 10 + col), cmnDataScalar(mInt, position));
            position++;
        }
    }

    position = 0;
    for (row = 0; row < mmDouble.rows(); ++row) {
        for (col = 0; col < mmDouble.cols(); ++col) {
            for (subRow = 0; subRow < mmDouble.Element(row, col).rows(); ++subRow) {
                for (subCol = 0; subCol < mmDouble.Element(row, col).cols(); ++subCol) {
                    mmDouble.Element(row, col).Element(subRow, subCol) = static_cast<double>(row * col * 100 + subRow * subCol);
                    CPPUNIT_ASSERT_EQUAL(static_cast<double>(row * col * 100 + subRow * subCol),
                                         cmnDataScalar(mmDouble, position));
                    position++;
                }
            }
        }
    }
}


CPPUNIT_TEST_SUITE_REGISTRATION(vctDataFunctionsDynamicMatrixTest);
