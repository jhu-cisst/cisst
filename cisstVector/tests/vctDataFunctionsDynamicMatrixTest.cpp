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

#include "vctGenericContainerTest.h"
#include "vctDataFunctionsDynamicMatrixTest.h"

#include <cisstCommon/cmnDataFunctionsString.h>

#include <cisstVector/vctDynamicMatrix.h>
#include <cisstVector/vctDataFunctionsDynamicMatrix.h>
#include <cisstVector/vctRandomDynamicMatrix.h>

void vctDataFunctionsDynamicMatrixTest::TestDataCopy(void)
{
    typedef vctDynamicMatrix<double> DataType;
    DataType source, data;
    source.SetSize(7, 3);
    vctRandom(source, -1.0, 1.0);
    cmnData<DataType>::Copy(data, source);
    CPPUNIT_ASSERT(source.Equal(data));
}


void vctDataFunctionsDynamicMatrixTest::TestBinarySerializationStream(void)
{
    cmnDataFormat local, remote;
    std::stringstream stream;
    typedef vctDynamicMatrix<double> DataType;
    DataType m1, m2, mReference;
    m1.SetSize(12, 23);
    m2.SetSize(12, 23);
    mReference.SetSize(12, 23);
    vctRandom(mReference, -10.0, 10.0);
    m1 = mReference;
    cmnData<DataType>::SerializeBinary(m1, stream);
    m1.SetAll(0);
    cmnData<DataType>::DeSerializeBinary(m2, stream, local, remote);
    CPPUNIT_ASSERT_EQUAL(mReference, m2);
}


void vctDataFunctionsDynamicMatrixTest::TestTextSerializationStream(void)
{
    std::stringstream stream;
    typedef vctDynamicMatrix<double> DataType;
    DataType m1, m2, mReference;
    m1.SetSize(4, 5);
    mReference.SetSize(4,5);
    m2.SetSize(2, 3); // intentionally different
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
    m2.SetSize(2, 3); // intentionally different
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
    m2.SetSize(2, 3); // intentionally different
    m2.SetAll(0.0);
    try {
        cmnData<DataType>::DeSerializeText(m2, stream, ',');
    } catch (std::runtime_error &) {
        exceptionReceived = true;
    }
    CPPUNIT_ASSERT(exceptionReceived);
}


void vctDataFunctionsDynamicMatrixTest::TestScalar(void)
{
    typedef vctDynamicMatrix<int> DataType;
    DataType mInt;
    mInt.SetSize(6, 3);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mInt.size() + 2), cmnData<DataType>::ScalarNumber(mInt));
    CPPUNIT_ASSERT_EQUAL(false, cmnData<DataType>::ScalarNumberIsFixed(mInt));

    typedef vctDynamicMatrix<vctDynamicMatrix<double> > DataType2;
    DataType2 mmDouble;
    mmDouble.SetSize(4, 5);
    size_t numberOfScalarsExpected = 2; // size of the containing matrix
    for (size_t i = 0; i < mmDouble.rows(); ++i) {
        for (size_t j = 0; j < mmDouble.cols(); ++j) {
            mmDouble.Element(i, j).SetSize(i + 1, j + 1); // + 1 to make sure we don't have a 0 size matrix
            numberOfScalarsExpected += ((i + 1) * (j + 1));
            numberOfScalarsExpected += 2; // size of each contained matrix
        }
    }
    CPPUNIT_ASSERT_EQUAL(numberOfScalarsExpected, cmnData<DataType2>::ScalarNumber(mmDouble));
    CPPUNIT_ASSERT_EQUAL(false, cmnData<DataType2>::ScalarNumberIsFixed(mmDouble));

    typedef vctDynamicMatrix<std::string> DataType3;
    DataType3 mString;
    mString.SetSize(3, 2);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), cmnData<DataType3>::ScalarNumber(mString));
    CPPUNIT_ASSERT_EQUAL(false, cmnData<DataType3>::ScalarNumberIsFixed(mString));

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

    // exception expected for any index above 2
    exceptionReceived = false;
    try {
        description = cmnData<DataType3>::ScalarDescription(mString, cmnData<DataType3>::ScalarNumber(mString) + 1);
    } catch (std::out_of_range &) {
        exceptionReceived = true;
    }
    CPPUNIT_ASSERT(exceptionReceived);

    // get scalar
    position = 0;
    CPPUNIT_ASSERT_EQUAL(static_cast<double>(mInt.rows()), cmnData<DataType>::Scalar(mInt, position));
    position++;
    CPPUNIT_ASSERT_EQUAL(static_cast<double>(mInt.cols()), cmnData<DataType>::Scalar(mInt, position));
    position++;

    for (row = 0; row < mInt.rows(); ++row) {
        for (col = 0; col < mInt.cols(); ++col) {
            mInt.Element(row, col) = static_cast<int>(row * 10 + col);
            CPPUNIT_ASSERT_EQUAL(static_cast<double>(row * 10 + col), cmnData<DataType>::Scalar(mInt, position));
            position++;
        }
    }

    // check size
    position = 0;
    CPPUNIT_ASSERT_EQUAL(static_cast<double>(mmDouble.rows()), cmnData<DataType2>::Scalar(mmDouble, position));
    position++;
    CPPUNIT_ASSERT_EQUAL(static_cast<double>(mmDouble.cols()), cmnData<DataType2>::Scalar(mmDouble, position));
    position++;

    for (row = 0; row < mmDouble.rows(); ++row) {
        for (col = 0; col < mmDouble.cols(); ++col) {
            CPPUNIT_ASSERT_EQUAL(static_cast<double>(mmDouble.Element(row, col).rows()), cmnData<DataType2>::Scalar(mmDouble, position));
            position++;
            CPPUNIT_ASSERT_EQUAL(static_cast<double>(mmDouble.Element(row, col).cols()), cmnData<DataType2>::Scalar(mmDouble, position));
            position++;
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
}


CPPUNIT_TEST_SUITE_REGISTRATION(vctDataFunctionsDynamicMatrixTest);
