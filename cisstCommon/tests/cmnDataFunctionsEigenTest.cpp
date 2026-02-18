/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Brendan Burkhart
  Created on: 2026-02-15

  (C) Copyright 2026 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "cmnDataFunctionsEigenTest.h"

#include <cisstCommon/cmnDataFunctionsEigen.h>

void cmnDataFunctionsEigenTest::TestDynamicSizedCopy(void)
{
    {
        Eigen::VectorXi source, data;
        source.resize(10);
        for (Eigen::Index index = 0; index < source.size(); index++) {
            source[index] = static_cast<int>(index);
        }
        cmnData<Eigen::VectorXi>::Copy(data, source);
        CPPUNIT_ASSERT_EQUAL(data.size(), source.size());
        for (Eigen::Index index = 0; index < source.size(); index++) {
            CPPUNIT_ASSERT_EQUAL(data[index], source[index]);
        }
    }

    {
        Eigen::VectorXd source, data;
        source.resize(10);
        for (Eigen::Index index = 0; index < source.size(); index++) {
            source[index] = 1.0 / static_cast<double>(index);
        }
        cmnData<Eigen::VectorXd>::Copy(data, source);
        CPPUNIT_ASSERT_EQUAL(data.size(), source.size());
        for (Eigen::Index index = 0; index < source.size(); index++) {
            CPPUNIT_ASSERT_EQUAL(data[index], source[index]);
        }
    }

    {
        Eigen::MatrixXd source, data;
        source.resize(2, 3);
        for (Eigen::Index row = 0; row < source.rows(); row++) {
            for (Eigen::Index col = 0; col < source.cols(); col++) {
                source(row, col) = static_cast<double>(col);
            }
        }
        cmnData<Eigen::MatrixXd>::Copy(data, source);
        CPPUNIT_ASSERT_EQUAL(data.rows(), source.rows());
        CPPUNIT_ASSERT_EQUAL(data.cols(), source.cols());
        for (Eigen::Index row = 0; row < source.rows(); row++) {
            for (Eigen::Index col = 0; col < source.cols(); col++) {
                source(row, col) = static_cast<double>(col);
                CPPUNIT_ASSERT_EQUAL(data(row, col), source(row, col));
            }
        }
    }
}

void cmnDataFunctionsEigenTest::TestFixedSizedCopy(void)
{
    {
        Eigen::Vector4i source, data;
        for (Eigen::Index index = 0; index < source.size(); index++) {
            source[index] = static_cast<int>(index);
        }
        cmnData<Eigen::Vector4i>::Copy(data, source);
        for (Eigen::Index index = 0; index < source.size(); index++) {
            CPPUNIT_ASSERT_EQUAL(data[index], source[index]);
        }
    }

    {
        Eigen::Vector3d source, data;
        for (Eigen::Index index = 0; index < source.size(); index++) {
            source[index] = 1.0 / static_cast<double>(index);
        }
        cmnData<Eigen::Vector3d>::Copy(data, source);
        for (Eigen::Index index = 0; index < source.size(); index++) {
            CPPUNIT_ASSERT_EQUAL(data[index], source[index]);
        }
    }

    {
        Eigen::Matrix3d source, data;
        for (Eigen::Index row = 0; row < source.rows(); row++) {
            for (Eigen::Index col = 0; col < source.cols(); col++) {
                source(row, col) = static_cast<double>(col);
            }
        }
        cmnData<Eigen::Matrix3d>::Copy(data, source);
        for (Eigen::Index row = 0; row < source.rows(); row++) {
            for (Eigen::Index col = 0; col < source.cols(); col++) {
                source(row, col) = static_cast<double>(col);
                CPPUNIT_ASSERT_EQUAL(data(row, col), source(row, col));
            }
        }
    }
}

void cmnDataFunctionsEigenTest::TestDynamicSizedBinarySerializationStream(void)
{
    cmnDataFormat local, remote;

    std::stringstream stream;
    {
        Eigen::VectorXf source, destination;
        source.resize(10);
        for (Eigen::Index idx = 0; idx < source.size(); ++idx) {
            source(idx) = static_cast<float>(idx);
        }

        cmnData<Eigen::VectorXf>::SerializeBinary(source, stream);
        cmnData<Eigen::VectorXf>::DeSerializeBinary(destination, stream, local, remote);
        CPPUNIT_ASSERT_EQUAL(destination.size(), source.size());
        for (Eigen::Index idx = 0; idx < source.size(); ++idx) {
            CPPUNIT_ASSERT_EQUAL(destination(idx), source(idx));
        }
    }
}

void cmnDataFunctionsEigenTest::TestFixedSizedBinarySerializationStream(void) {
    cmnDataFormat local, remote;

    std::stringstream stream;
    {
        Eigen::Vector2f source, destination;
        for (Eigen::Index idx = 0; idx < source.size(); ++idx) {
            source(idx) = static_cast<float>(idx);
        }

        cmnData<Eigen::Vector2f>::SerializeBinary(source, stream);
        cmnData<Eigen::Vector2f>::DeSerializeBinary(destination, stream, local, remote);
        for (Eigen::Index idx = 0; idx < source.size(); ++idx) {
            CPPUNIT_ASSERT_EQUAL(destination(idx), source(idx));
        }
    }
}

void cmnDataFunctionsEigenTest::TestDynamicSizedTextSerializationStream(void) {
    std::stringstream stream;
    {
        Eigen::VectorXf source, destination;
        source.resize(12);
        for (Eigen::Index idx = 0; idx < source.size(); ++idx) {
            source(idx) = static_cast<float>(idx);
        }

        cmnData<Eigen::VectorXf>::SerializeText(source, stream, ',');
        cmnData<Eigen::VectorXf>::DeSerializeText(destination, stream, ',');
        CPPUNIT_ASSERT_EQUAL(destination.size(), source.size());
        for (Eigen::Index idx = 0; idx < source.size(); ++idx) {
            CPPUNIT_ASSERT_EQUAL(destination(idx), source(idx));
        }
    }
}
void cmnDataFunctionsEigenTest::TestFixedSizedTextSerializationStream(void) {
    std::stringstream stream;
    {
        Eigen::Vector4d source, destination;
        for (Eigen::Index idx = 0; idx < source.size(); ++idx) {
            source(idx) = static_cast<float>(idx);
        }

        cmnData<Eigen::Vector4d>::SerializeText(source, stream, ',');
        cmnData<Eigen::Vector4d>::DeSerializeText(destination, stream, ',');
        for (Eigen::Index idx = 0; idx < source.size(); ++idx) {
            CPPUNIT_ASSERT_EQUAL(destination(idx), source(idx));
        }
    }
}

void cmnDataFunctionsEigenTest::TestDynamicSizedScalars(void) {}
void cmnDataFunctionsEigenTest::TestFixedSizedScalars(void) {}
