/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Brendan Burkhart
  Created on: 2026-02-24

  (C) Copyright 2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once

#ifndef _cmnDataFunctionsEigenJSON_h
#define _cmnDataFunctionsEigenJSON_h

#include <cisstCommon/cmnDataFunctionsJSON.h>

#include <Eigen/Core>

#if CISST_HAS_JSON
template <typename Derived>
class cmnDataJSON<Derived, typename std::enable_if_t<std::is_base_of_v<Eigen::PlainObjectBase<Derived>, Derived>>>
{
public:
    using DataType = Derived;

    static void SerializeText(const DataType& matrix, Json::Value& jsonValue) {
        for (int row = 0; row < matrix.rows(); ++row) {
            for (int col = 0; col < matrix.cols(); ++col) {
                cmnDataJSON<typename DataType::Scalar>::SerializeText(matrix(row, col), jsonValue[row][col]);
            }
        }
    }

    static void DeSerializeText(DataType& matrix, const Json::Value & jsonValue)
        CISST_THROW(std::runtime_error)
    {
        // get the matrix size from JSON and resize
        const size_t rows = jsonValue.size();
        const size_t cols = (rows > 0) ? jsonValue[0].size() : 0;
        if (DataType::RowsAtCompileTime != Eigen::Dynamic && rows != DataType::RowsAtCompileTime) {
            cmnThrow("cmnDataJSON<Eigen::PlainObjectBase>::DeSerializeText: rows doesn't match compile time size");
        }

        if (DataType::ColsAtCompileTime != Eigen::Dynamic && cols != DataType::ColsAtCompileTime) {
            cmnThrow("cmnDataJSON<Eigen::PlainObjectBase>::DeSerializeText: cols doesn't match compile time size");
        }

        matrix.resize(rows, cols);

        for (int row = 0; row < matrix.rows(); row++) {
            // make sure both rows have same size
            if (cols != jsonValue[row].size()) {
                cmnThrow("cmnDataJSON<Eigen::PlainObjectBase>::DeSerializeText: row sizes don't match");
            }

            for (int col = 0; col < matrix.cols(); col++) {
                cmnDataJSON<typename DataType::Scalar>::DeSerializeText(matrix(row, col), jsonValue[row][col]);
            }
        }
    }
};

// Implement cmnDataJSON<Eigen::Transform> in terms of cmnDataJSON<Eigen::Transform::MatrixType>.
//
// Note: `cmnDataJSON<>` must be specialized for `Scalar_`.
template <typename Scalar_, int Dim_, int Mode_, int Options_>
class cmnDataJSON<Eigen::Transform<Scalar_, Dim_, Mode_, Options_>>
{
public:
    using DataType = Eigen::Transform<Scalar_, Dim_, Mode_, Options_>;

    static void SerializeText(const DataType& transform, Json::Value& jsonValue) {
        cmnDataJSON<typename DataType::MatrixType>::SerializeText(transform.matrix(), jsonValue);
    }

    static void DeSerializeText(DataType& transform, const Json::Value & jsonValue)
        CISST_THROW(std::runtime_error)
    {
        cmnDataJSON<typename DataType::MatrixType>::DeSerializeText(transform.matrix(), jsonValue);
    }
};


// Implement cmnDataJSON<Eigen::Quaternion> in terms of cmnDataJSON<Eigen::Vector4>.
//
// Note: `cmnDataJSON<>` must be specialized for `Scalar_`.
template <typename Scalar_>
class cmnDataJSON<Eigen::Quaternion<Scalar_>>
{
public:
    using DataType = Eigen::Quaternion<Scalar_>;

    static void SerializeText(const DataType& q, Json::Value& jsonValue) {
        cmnDataJSON<Eigen::Vector4<Scalar_>>::SerializeText(q.coeffs(), jsonValue);
    }

    static void DeSerializeText(DataType& q, const Json::Value & jsonValue)
        CISST_THROW(std::runtime_error)
    {
        cmnDataJSON<Eigen::Vector4<Scalar_>>::DeSerializeText(q.coeffs(), jsonValue);
    }
};

// Specialization for cmnDataJSON<Eigen::Rotation2D>.
//
// Note: `cmnDataJSON<>` must be specialized for `Scalar_`.
template <typename Scalar_>
class cmnDataJSON<Eigen::Rotation2D<Scalar_>>
{
public:
    using DataType = Eigen::Rotation2D<Scalar_>;

    static void SerializeText(const DataType& r, Json::Value& jsonValue) {
        cmnDataJSON<Scalar_>::SerializeText(r.angle(), jsonValue);
    }

    static void DeSerializeText(DataType& r, const Json::Value & jsonValue)
        CISST_THROW(std::runtime_error)
    {
        Scalar_ angle;
        cmnDataJSON<Eigen::Vector4<Scalar_>>::DeSerializeText(angle, jsonValue);
        r = Eigen::Rotation2D<Scalar_>(angle);
    }
};

#endif // CISST_HAS_JSON

#endif // _cmnDataFunctionsEigenJSON_h
