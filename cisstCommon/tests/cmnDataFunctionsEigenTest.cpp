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

void cmnDataFunctionsEigenTest::TestDynamicSizedCopy()
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
            CPPUNIT_ASSERT_EQUAL(source[index], data[index]);
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
            CPPUNIT_ASSERT_EQUAL(source[index], data[index]);
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
                CPPUNIT_ASSERT_EQUAL(source(row, col), data(row, col));
            }
        }
    }
}

void cmnDataFunctionsEigenTest::TestFixedSizedCopy()
{
    {
        Eigen::Vector4i source, data;
        for (Eigen::Index index = 0; index < source.size(); index++) {
            source[index] = static_cast<int>(index);
        }
        cmnData<Eigen::Vector4i>::Copy(data, source);
        for (Eigen::Index index = 0; index < source.size(); index++) {
            CPPUNIT_ASSERT_EQUAL(source[index], data[index]);
        }
    }

    {
        Eigen::Vector3d source, data;
        for (Eigen::Index index = 0; index < source.size(); index++) {
            source[index] = 1.0 / static_cast<double>(index);
        }
        cmnData<Eigen::Vector3d>::Copy(data, source);
        for (Eigen::Index index = 0; index < source.size(); index++) {
            CPPUNIT_ASSERT_EQUAL(source[index], data[index]);
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
                CPPUNIT_ASSERT_EQUAL(source(row, col), data(row, col));
            }
        }
    }
}

void cmnDataFunctionsEigenTest::TestDynamicSizedBinarySerializationStream()
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
            CPPUNIT_ASSERT_EQUAL(source(idx), destination(idx));
        }
    }
}

void cmnDataFunctionsEigenTest::TestFixedSizedBinarySerializationStream() {
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
            CPPUNIT_ASSERT_EQUAL(source(idx), destination(idx));
        }
    }
}

void cmnDataFunctionsEigenTest::TestDynamicSizedTextSerializationStream() {
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
            CPPUNIT_ASSERT_EQUAL(source(idx), destination(idx));
        }
    }
}
void cmnDataFunctionsEigenTest::TestFixedSizedTextSerializationStream() {
    std::stringstream stream;
    {
        Eigen::Vector4d source, destination;
        for (Eigen::Index idx = 0; idx < source.size(); ++idx) {
            source(idx) = static_cast<float>(idx);
        }

        cmnData<Eigen::Vector4d>::SerializeText(source, stream, ',');
        cmnData<Eigen::Vector4d>::DeSerializeText(destination, stream, ',');
        for (Eigen::Index idx = 0; idx < source.size(); ++idx) {
            CPPUNIT_ASSERT_EQUAL(source(idx), destination(idx));
        }
    }
}

void cmnDataFunctionsEigenTest::TestDynamicSizedScalars() {
    {
        Eigen::VectorXf vector;
        vector.resize(12);
        for (Eigen::Index idx = 0; idx < vector.size(); ++idx) {
            vector(idx) = static_cast<float>(idx);
        }

        CPPUNIT_ASSERT(!cmnData<Eigen::VectorXf>::ScalarNumberIsFixed(vector));
        CPPUNIT_ASSERT_EQUAL(std::size_t{12+2}, cmnData<Eigen::VectorXf>::ScalarNumber(vector));

        CPPUNIT_ASSERT_EQUAL(12.0, cmnData<Eigen::VectorXf>::Scalar(vector, 0));
        CPPUNIT_ASSERT_EQUAL(1.0, cmnData<Eigen::VectorXf>::Scalar(vector, 1));

        for (size_t idx = 0; idx < 12; idx++) {
            CPPUNIT_ASSERT_EQUAL(static_cast<double>(idx), cmnData<Eigen::VectorXf>::Scalar(vector, idx + 2));
        }
    }

    {
        Eigen::Matrix3Xf matrix;
        matrix.resize(3, 12);
        for (Eigen::Index row = 0; row < matrix.rows(); ++row) {
            for (Eigen::Index col = 0; col < matrix.cols(); ++col) {
                matrix(row, col) = static_cast<float>(2*row + col);
            }
        }

        CPPUNIT_ASSERT(!cmnData<Eigen::MatrixXf>::ScalarNumberIsFixed(matrix));
        CPPUNIT_ASSERT_EQUAL(std::size_t{3*12+2}, cmnData<Eigen::MatrixXf>::ScalarNumber(matrix));

        CPPUNIT_ASSERT_EQUAL(3.0, cmnData<Eigen::MatrixXf>::Scalar(matrix, 0));
        CPPUNIT_ASSERT_EQUAL(12.0, cmnData<Eigen::MatrixXf>::Scalar(matrix, 1));
        for (Eigen::Index row = 0; row < matrix.rows(); ++row) {
            for (Eigen::Index col = 0; col < matrix.cols(); ++col) {
                size_t linear_idx = static_cast<size_t>(row * matrix.cols() + col + 2);
                CPPUNIT_ASSERT_EQUAL(static_cast<double>(2*row + col), cmnData<Eigen::MatrixXf>::Scalar(matrix, linear_idx));
            }
        }
    }
}

void cmnDataFunctionsEigenTest::TestFixedSizedScalars() {
    Eigen::Vector3d data;
    for (Eigen::Index idx = 0; idx < data.size(); ++idx) {
        data(idx) = static_cast<double>(idx);
    }

    CPPUNIT_ASSERT(cmnData<Eigen::Vector3d>::ScalarNumberIsFixed(data));
    CPPUNIT_ASSERT_EQUAL(std::size_t{3+2}, cmnData<Eigen::Vector3d>::ScalarNumber(data));

    CPPUNIT_ASSERT_EQUAL(3.0, cmnData<Eigen::Vector3d>::Scalar(data, 0));
    CPPUNIT_ASSERT_EQUAL(1.0, cmnData<Eigen::Vector3d>::Scalar(data, 1));

    for (size_t idx = 0; idx < 3; idx++) {
        CPPUNIT_ASSERT_EQUAL(static_cast<double>(idx), cmnData<Eigen::Vector3d>::Scalar(data, idx + 2));
    }
}

void cmnDataFunctionsEigenTest::TestTransformBinarySerializationStream() {
    cmnDataFormat local, remote;

    std::stringstream stream;
    {
        stream.str("");
        stream.clear();
        Eigen::Isometry3d source(Eigen::Translation3d(1.0, -1.0, 2.0));
        Eigen::Isometry3d destination;

        cmnData<Eigen::Isometry3d>::SerializeBinary(source, stream);
        cmnData<Eigen::Isometry3d>::DeSerializeBinary(destination, stream, local, remote);
        for (Eigen::Index row = 0; row < source.rows(); ++row) {
            for (Eigen::Index col = 0; col < source.cols(); ++col) {
                CPPUNIT_ASSERT_EQUAL(source.matrix()(row, col), destination.matrix()(row, col));
            }
        }
    }

    {
        stream.str("");
        stream.clear();
        Eigen::Vector3f axis = Eigen::Vector3f(0.06, 0.7, -0.2).normalized();
        Eigen::Projective3f source(Eigen::AngleAxisf(0.25, axis));
        Eigen::Projective3f destination;

        cmnData<Eigen::Projective3f>::SerializeBinary(source, stream);
        cmnData<Eigen::Projective3f>::DeSerializeBinary(destination, stream, local, remote);
        for (Eigen::Index row = 0; row < source.rows(); ++row) {
            for (Eigen::Index col = 0; col < source.cols(); ++col) {
                CPPUNIT_ASSERT_EQUAL(source.matrix()(row, col), destination.matrix()(row, col));
            }
        }
    }

    {
        stream.str("");
        stream.clear();
        Eigen::AffineCompact2f source(Eigen::Translation2f(8.2, -4.5));
        Eigen::AffineCompact2f destination;

        cmnData<Eigen::AffineCompact2f>::SerializeBinary(source, stream);
        cmnData<Eigen::AffineCompact2f>::DeSerializeBinary(destination, stream, local, remote);
        for (Eigen::Index row = 0; row < source.rows(); ++row) {
            for (Eigen::Index col = 0; col < source.cols(); ++col) {
                CPPUNIT_ASSERT_EQUAL(source.matrix()(row, col), destination.matrix()(row, col));
            }
        }
    }
}

void cmnDataFunctionsEigenTest::TestTransformTextSerializationStream() {
    std::stringstream stream;
    double stream_tolerance = std::pow(0.1, stream.precision() - 1);

    {
        stream.str("");
        stream.clear();
        Eigen::Isometry2f source(Eigen::Translation2f(-1.0, 2.0));
        Eigen::Isometry2f destination;

        cmnData<Eigen::Isometry2f>::SerializeText(source, stream, ',');
        cmnData<Eigen::Isometry2f>::DeSerializeText(destination, stream, ',');
        for (Eigen::Index row = 0; row < source.rows(); ++row) {
            for (Eigen::Index col = 0; col < source.cols(); ++col) {
                CPPUNIT_ASSERT_DOUBLES_EQUAL(source.matrix()(row, col), destination.matrix()(row, col), stream_tolerance);
            }
        }
    }

    {
        stream.str("");
        stream.clear();
        Eigen::Vector3d axis = Eigen::Vector3d(0.06, 0.7, -0.2).normalized();
        Eigen::Affine3d source(Eigen::AngleAxisd(0.25, axis));
        Eigen::Affine3d destination;

        cmnData<Eigen::Affine3d>::SerializeText(source, stream, ',');
        cmnData<Eigen::Affine3d>::DeSerializeText(destination, stream, ',');
        for (Eigen::Index row = 0; row < source.rows(); ++row) {
            for (Eigen::Index col = 0; col < source.cols(); ++col) {
                CPPUNIT_ASSERT_DOUBLES_EQUAL(source.matrix()(row, col), destination.matrix()(row, col), stream_tolerance);
            }
        }
    }

    {
        stream.str("");
        stream.clear();
        Eigen::AffineCompact2d source(Eigen::Translation2d(3.2, -42.0));
        Eigen::AffineCompact2d destination;

        cmnData<Eigen::AffineCompact2d>::SerializeText(source, stream, ',');
        cmnData<Eigen::AffineCompact2d>::DeSerializeText(destination, stream, ',');
        for (Eigen::Index row = 0; row < source.rows(); ++row) {
            for (Eigen::Index col = 0; col < source.cols(); ++col) {
                CPPUNIT_ASSERT_DOUBLES_EQUAL(source.matrix()(row, col), destination.matrix()(row, col), stream_tolerance);
            }
        }
    }
}

void cmnDataFunctionsEigenTest::TestQuaternionBinarySerializationStream() {
    cmnDataFormat local, remote;

    std::stringstream stream;
    {
        Eigen::Quaterniond source(1.0, -2.0, 0.5, 0.15);
        source.coeffs().normalize();
        Eigen::Quaterniond destination;

        cmnData<Eigen::Quaterniond>::SerializeBinary(source, stream);
        cmnData<Eigen::Quaterniond>::DeSerializeBinary(destination, stream, local, remote);

        CPPUNIT_ASSERT_EQUAL(source.x(), destination.x());
        CPPUNIT_ASSERT_EQUAL(source.y(), destination.y());
        CPPUNIT_ASSERT_EQUAL(source.z(), destination.z());
        CPPUNIT_ASSERT_EQUAL(source.w(), destination.w());
    }
}

void cmnDataFunctionsEigenTest::TestQuaternionTextSerializationStream() {
    std::stringstream stream;
    {
        Eigen::Quaterniond source(0.8, -0.7, -0.7, -3.0);
        source.coeffs().normalize();
        Eigen::Quaterniond destination;

        cmnData<Eigen::Quaterniond>::SerializeText(source, stream, ',');
        cmnData<Eigen::Quaterniond>::DeSerializeText(destination, stream, ',');

        double stream_tolerance = std::pow(0.1, stream.precision() - 1);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(source.x(), destination.x(), stream_tolerance);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(source.y(), destination.y(), stream_tolerance);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(source.z(), destination.z(), stream_tolerance);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(source.w(), destination.w(), stream_tolerance);
    }
}

void cmnDataFunctionsEigenTest::TestRotation2DBinarySerializationStream() {
    cmnDataFormat local, remote;

    std::stringstream stream;
    {
        Eigen::Rotation2Dd source(-0.674);
        Eigen::Rotation2Dd destination(0.0);

        cmnData<Eigen::Rotation2Dd>::SerializeBinary(source, stream);
        cmnData<Eigen::Rotation2Dd>::DeSerializeBinary(destination, stream, local, remote);

        CPPUNIT_ASSERT_EQUAL(source.angle(), destination.angle());
    }
}

void cmnDataFunctionsEigenTest::TestRotation2DTextSerializationStream() {
    std::stringstream stream;
    {
        Eigen::Rotation2Df source(0.341);
        Eigen::Rotation2Df destination(0.0);

        cmnData<Eigen::Rotation2Df>::SerializeText(source, stream, ',');
        cmnData<Eigen::Rotation2Df>::DeSerializeText(destination, stream, ',');

        CPPUNIT_ASSERT_EQUAL(source.angle(), destination.angle());
    }
}
