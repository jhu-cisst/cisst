/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2012-07-09

  (C) Copyright 2012-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "vctDataFunctionsFixedSizeMatrixTest.h"

#include <cisstCommon/cmnDataFunctionsString.h>

#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctDataFunctionsFixedSizeMatrix.h>
#include <cisstVector/vctRandomFixedSizeMatrix.h>

#include <cisstVector/vctDynamicMatrix.h>
#include <cisstVector/vctDataFunctionsDynamicMatrix.h>
#include <cisstVector/vctRandomDynamicMatrix.h>

void vctDataFunctionsFixedSizeMatrixTest::TestDataCopy(void)
{
    typedef vctFixedSizeMatrix<double, 7, 3> DataType;
    DataType source, data;
    vctRandom(source, -1.0, 1.0);
    cmnData<DataType>::Copy(data, source);
    CPPUNIT_ASSERT(source.Equal(data));
}


void vctDataFunctionsFixedSizeMatrixTest::TestBinarySerializationStream(void)
{
    cmnDataFormat local, remote;
    std::stringstream stream;
    typedef vctFixedSizeMatrix<double, 6, 3> DataType;
    DataType m1, m2, mReference;
    vctRandom(mReference, -10.0, 10.0);
    m1 = mReference;
    cmnData<DataType>::SerializeBinary(m1, stream);
    m1.SetAll(0);
    cmnData<DataType>::DeSerializeBinary(m2, stream, local, remote);
    CPPUNIT_ASSERT_EQUAL(mReference, m2);
}


void vctDataFunctionsFixedSizeMatrixTest::TestTextSerializationStream(void)
{
    std::stringstream stream;
    typedef vctFixedSizeMatrix<double, 4, 5> DataType;
    DataType m1, m2, mReference;
    vctRandom(mReference, -10.0, 10.0);
    m1 = mReference;
    cmnData<DataType>::SerializeText(m1, stream, ',');
    m1.SetAll(0);
    cmnData<DataType>::DeSerializeText(m2, stream, ',');
    CPPUNIT_ASSERT(mReference.AlmostEqual(m2, 0.01)); // low precision due to stream out loss
    CPPUNIT_ASSERT(!stream.fail());
    // try without delimiter, using space
    stream.clear();
    vctRandom(mReference, -20.0, 20.0);
    m1 = mReference;
    cmnData<DataType>::SerializeText(m1, stream, ' ');
    m2.SetAll(0.0);
    cmnData<DataType>::DeSerializeText(m2, stream, ' ');
    CPPUNIT_ASSERT(mReference.AlmostEqual(m2, 0.01)); // low precision due to stream out loss
    CPPUNIT_ASSERT(!stream.fail());
    // try with the wrong delimiter
    bool exceptionReceived = false;
    stream.clear();
    vctRandom(mReference, -20.0, 20.0);
    m1 = mReference;
    cmnData<DataType>::SerializeText(m1, stream, '|');
    m2.SetAll(0.0);
    try {
        cmnData<DataType>::DeSerializeText(m2, stream, ',');
    } catch (std::runtime_error &) {
        exceptionReceived = true;
    }
    CPPUNIT_ASSERT(exceptionReceived);
}


void vctDataFunctionsFixedSizeMatrixTest::TestScalar(void)
{
    typedef vctFixedSizeMatrix<int, 3, 6> DataType;
    DataType mInt;
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3 * 6), cmnData<DataType>::ScalarNumber(mInt));
    CPPUNIT_ASSERT_EQUAL(true, cmnData<DataType>::ScalarNumberIsFixed(mInt));

    typedef vctFixedSizeMatrix<vctFixedSizeMatrix<double, 3, 4>, 2, 3> DataType2;
    DataType2 mmDouble;
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3 * 4 * 2 * 3), cmnData<DataType2>::ScalarNumber(mmDouble));
    CPPUNIT_ASSERT_EQUAL(true, cmnData<DataType2>::ScalarNumberIsFixed(mmDouble));

    typedef vctFixedSizeMatrix<std::string, 3, 2> DataType3;
    DataType3 mString;
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), cmnData<DataType3>::ScalarNumber(mString));
    CPPUNIT_ASSERT_EQUAL(true, cmnData<DataType3>::ScalarNumberIsFixed(mString));

    typedef vctFixedSizeMatrix<vctDynamicMatrix<double>, 4, 2> DataType4;
    DataType4 mmMixed;
    size_t numberOfScalarsExpected = 0;
    for (size_t i = 0; i < mmMixed.rows(); i++) {
        for (size_t j = 0; j < mmMixed.cols(); j++) {
            numberOfScalarsExpected += 2; // size of dynamic matrix
            mmMixed.Element(i, j).SetSize(i + 1, j + 1); // + 1 to make sure we don't have a 0 size matrix
            numberOfScalarsExpected += ((i + 1) * (j + 1));
        }
    }
    CPPUNIT_ASSERT_EQUAL(numberOfScalarsExpected, cmnData<DataType4>::ScalarNumber(mmMixed));
    CPPUNIT_ASSERT_EQUAL(false, cmnData<DataType4>::ScalarNumberIsFixed(mmMixed));

    size_t row, col, subRow, subCol, position;
    bool exceptionReceived = false;
    std::string description;

    // exception expected if index too high
    try {
        description = cmnData<DataType>::ScalarDescription(mInt, cmnData<DataType>::ScalarNumber(mInt) + 1);
    } catch (std::out_of_range &) {
        exceptionReceived = true;
    }
    CPPUNIT_ASSERT(exceptionReceived);

    exceptionReceived = false;
    try {
        description = cmnData<DataType2>::ScalarDescription(mmDouble, cmnData<DataType2>::ScalarNumber(mmDouble) + 1);
    } catch (std::out_of_range &) {
        exceptionReceived = true;
    }
    CPPUNIT_ASSERT(exceptionReceived);

    // exception expected for any index
    exceptionReceived = false;
    try {
        description = cmnData<DataType3>::ScalarDescription(mString, cmnData<DataType3>::ScalarNumber(mString));
    } catch (std::out_of_range &) {
        exceptionReceived = true;
    }
    CPPUNIT_ASSERT(exceptionReceived);

    // get scalar
    position = 0;
    for (row = 0; row < mInt.rows(); ++row) {
        for (col = 0; col < mInt.cols(); ++col) {
            mInt.Element(row, col) = static_cast<int>(row * 10 + col);
            CPPUNIT_ASSERT_EQUAL(static_cast<double>(row * 10 + col), cmnData<DataType>::Scalar(mInt, position));
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
                                         cmnData<DataType2>::Scalar(mmDouble, position));
                    position++;
                }
            }
        }
    }

    // try with fixed size matrix of dynamic matrices
    position = 0;
    for (row = 0; row < mmMixed.rows(); ++row) {
        for (col = 0; col < mmMixed.cols(); ++col) {
            CPPUNIT_ASSERT_EQUAL(static_cast<double>(mmMixed.Element(row, col).rows()), cmnData<DataType4>::Scalar(mmMixed, position));
            position++;
            CPPUNIT_ASSERT_EQUAL(static_cast<double>(mmMixed.Element(row, col).cols()), cmnData<DataType4>::Scalar(mmMixed, position));
            position++;
            for (subRow = 0; subRow < mmMixed.Element(row, col).rows(); ++subRow) {
                for (subCol = 0; subCol < mmMixed.Element(row, col).cols(); ++subCol) {
                    mmMixed.Element(row, col).Element(subRow, subCol) = static_cast<double>(row * col * 100 + subRow * subCol);
                    CPPUNIT_ASSERT_EQUAL(static_cast<double>(row * col * 100 + subRow * subCol),
                                         cmnData<DataType4>::Scalar(mmMixed, position));
                    position++;
                }
            }
        }
    }
}


CPPUNIT_TEST_SUITE_REGISTRATION(vctDataFunctionsFixedSizeMatrixTest);
