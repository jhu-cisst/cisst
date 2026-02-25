/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Brendan Burkhart
  Created on: 2026-02-15

  (C) Copyright 2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#pragma once
#ifndef _cmnDataFunctionsEigen_h
#define _cmnDataFunctionsEigen_h

#include <cisstCommon/cmnDataFunctions.h>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <sstream>
#include <stdexcept>
#include <type_traits>

#ifdef CISST_HAS_JSON
#include <cisstCommon/cmnDataFunctionsEigenJSON.h>
#endif

// always include last
#include <cisstCommon/cmnExport.h>

// `Eigen::PlainObjectBase` is a base class for simple/contiguous
// matrix/vector/array types which own their own memory, and is more or less the
// most general Eigen type is makes sense to use as a data type for
// (de)serialization.
//
// Note: `cmnData<>` must be specialized for `Derived::Scalar`, and must have
// fixed scalar number.
template <typename Derived>
class cmnData<Derived, typename std::enable_if_t<std::is_base_of_v<Eigen::PlainObjectBase<Derived>, Derived>>>
{
public:
    enum {IS_SPECIALIZED = 1};

    using DataType = Derived;

    static void Copy(DataType & data, const DataType & source)
    {
        data = source;
    }

    static std::string HumanReadable(const DataType & data)
    {
        Eigen::IOFormat format_spec(Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", "", "[", "]", "[", "]");
        std::stringstream human_readable;
        human_readable << data.format(format_spec);
        return human_readable.str();
    }

    static void SerializeText(const DataType & data,
                              std::ostream & outputStream,
                              const char delimiter) CISST_THROW(std::runtime_error)
    {
        const size_t rows = data.rows();
        const size_t cols = data.cols();
        cmnData<size_t>::SerializeText(rows, outputStream, delimiter);
        outputStream << delimiter;
        cmnData<size_t>::SerializeText(cols, outputStream, delimiter);
        if (rows * cols == 0) {
            return;
        }

        for (size_t row = 0; row < rows; row++) {
            for (size_t col = 0; col < cols; col++) {
                outputStream << delimiter;
                cmnData<typename DataType::Scalar>::SerializeText(
                    data.coeff(row, col),
                    outputStream,
                    delimiter
                );
            }
        }
    }

    static void DeSerializeText(DataType & data,
                                std::istream & inputStream,
                                const char delimiter) CISST_THROW(std::runtime_error)
    {
        size_t rows, cols;
        cmnData<size_t>::DeSerializeText(rows, inputStream, delimiter);
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "Eigen::PlainObjectBase");
        cmnData<size_t>::DeSerializeText(cols, inputStream, delimiter);

        if (DataType::RowsAtCompileTime != Eigen::Dynamic && DataType::RowsAtCompileTime != static_cast<Eigen::Index>(rows)) {
            std::string message("cmnData<Eigen::PlainObjectBase>::DeSerializeText: ");
            message.append(", data rows must match compile-time rows");
            cmnThrow(message);
        }

        if (DataType::ColsAtCompileTime != Eigen::Dynamic && DataType::ColsAtCompileTime != static_cast<Eigen::Index>(cols)) {
            std::string message("cmnData<Eigen::PlainObjectBase>::DeSerializeText: ");
            message.append(", data cols must match compile-time cols");
            cmnThrow(message);
        }

        // Safe to resize even for fixed-size matrices as long as rows/cols matches fixed values
        data.resize(rows, cols);

        for (size_t row = 0; row < rows; row++) {
            for (size_t col = 0; col < cols; col++) {
                cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "Eigen::PlainObjectBase");
                cmnData<typename DataType::Scalar>::DeSerializeText(
                    data(row, col),
                    inputStream,
                    delimiter
                );
            }
        }
    }

    static std::string SerializeDescription(const DataType & data,
                                            const char delimiter,
                                            const std::string & userDescription = "m")
    {
        std::string prefix = userDescription.empty() ? "m" : userDescription;
        std::stringstream description;
        const size_t rows = data.rows();
        const size_t cols = data.cols();
        description << cmnData<size_t>::SerializeDescription(rows, delimiter, prefix + ".rows")
                    << delimiter
                    << cmnData<size_t>::SerializeDescription(cols, delimiter, prefix + ".cols");

        for (size_t row = 0; row < rows; ++row) {
            for (size_t col = 0; col < cols; ++col) {
                description << delimiter;
                std::stringstream index_suffix;
                index_suffix << prefix << "[" << row << "," << col << "]";
                description << cmnData<typename DataType::Scalar>::SerializeDescription(
                    data.coeff(row, col),
                    delimiter,
                    index_suffix.str()
                );
            }
        }

        return description.str();
    }

    static void SerializeBinary(const DataType & data, std::ostream & outputStream)
        CISST_THROW(std::runtime_error)
    {
        cmnData<size_t>::SerializeBinary(data.rows(), outputStream);
        cmnData<size_t>::SerializeBinary(data.cols(), outputStream);

        for (Eigen::Index row = 0; row < data.rows(); ++row) {
            for (Eigen::Index col = 0; col < data.cols(); ++col) {
                cmnData<typename DataType::Scalar>::SerializeBinary(data.coeff(row, col), outputStream);
            }
        }
    }

    static void DeSerializeBinary(DataType & data,
                                  std::istream & inputStream,
                                  const cmnDataFormat & localFormat,
                                  const cmnDataFormat & remoteFormat)
        CISST_THROW(std::runtime_error)
    {
        size_t rows, cols;
        cmnDataDeSerializeBinary_size_t(rows, inputStream, localFormat, remoteFormat);
        cmnDataDeSerializeBinary_size_t(cols, inputStream, localFormat, remoteFormat);

        if (DataType::RowsAtCompileTime != Eigen::Dynamic && DataType::RowsAtCompileTime != static_cast<Eigen::Index>(rows)) {
            std::string message("cmnData<Eigen::PlainObjectBase>::DeSerializeBinary: ");
            message.append(", data rows must match compile-time rows");
            cmnThrow(message);
        }

        if (DataType::ColsAtCompileTime != Eigen::Dynamic && DataType::ColsAtCompileTime != static_cast<Eigen::Index>(cols)) {
            std::string message("cmnData<Eigen::PlainObjectBase>::DeSerializeBinary: ");
            message.append(", data cols must match compile-time cols");
            cmnThrow(message);
        }

        // Safe to resize even for fixed-size matrices as long as rows/cols matches fixed values
        data.resize(rows, cols);

        for (Eigen::Index row = 0; row < data.rows(); ++row) {
            for (Eigen::Index col = 0; col < data.cols(); ++col) {
                cmnData<typename DataType::Scalar>::DeSerializeBinary(
                    data(row, col),
                    inputStream,
                    localFormat,
                    remoteFormat
                );
            }
        }
    }

    static bool ScalarNumberIsFixed(const DataType & CMN_UNUSED(data))
    {
        bool rows_fixed = DataType::RowsAtCompileTime != Eigen::Dynamic;
        bool cols_fixed = DataType::ColsAtCompileTime != Eigen::Dynamic;
        return rows_fixed && cols_fixed;
    }

    static size_t ScalarNumber(const DataType & data)
    {
        // Currently assuming any scalar will have fixed scalar number
        return data.size() + 2; /* treat nrows, ncols as first two scalars */
    }

    static std::string ScalarDescription(const DataType & data, const size_t index,
                                         const std::string & userDescription = "m")
        CISST_THROW(std::out_of_range)
    {
        /* treat nrows, ncols as first two scalars */
        std::string prefix = userDescription.empty() ? "m" : userDescription;
        if (index == 0) {
            return cmnData<size_t>::ScalarDescription(data.rows(), 0, prefix + ".size");
        } else if (index == 1) {
            return cmnData<size_t>::ScalarDescription(data.cols(), 0, prefix + ".size");
        }

        if (index - 2 > (size_t)data.size()) {
            throw std::out_of_range("ScalarDescription index out of range");
        }

        std::stringstream suffix;
        suffix << "[" << data.rows() << "," << data.cols() << "]";
        return cmnData<typename DataType::Scalar>::ScalarDescription(
            // reshaped() provides linearly-indexed view
            data.template reshaped<Eigen::RowMajor>().coeff(index - 2),
            0,
            prefix + suffix.str()
        );
    }

    static double Scalar(const DataType & data, const size_t index)
        CISST_THROW(std::out_of_range)
    {
        /* treat nrows, ncols as first two scalars */
        if (index == 0) {
            return data.rows();
        } else if (index == 1) {
            return data.cols();
        }

        if (index - 2 > (size_t)data.size()) {
            throw std::out_of_range("Scalar index out of range");
        }

        // reshaped() provides linearly-indexed view
        return data.template reshaped<Eigen::RowMajor>().coeff(index - 2);
    }
};

// Implement cmnData<Eigen::Transform> in terms of cmnData<Eigen::Transform::MatrixType>.
//
// Note: `cmnData<>` must be specialized for `Derived::Scalar`, and must have
// fixed scalar number.
template <typename Scalar_, int Dim_, int Mode_, int Options_>
class cmnData<Eigen::Transform<Scalar_, Dim_, Mode_, Options_>>
{
public:
    enum {IS_SPECIALIZED = 1};

    using DataType = Eigen::Transform<Scalar_, Dim_, Mode_, Options_>;
    using MatrixType = typename DataType::MatrixType;

    static void Copy(DataType & data, const DataType & source)
    {
        cmnData<MatrixType>::Copy(data.matrix(), source.matrix());
    }

    static std::string HumanReadable(const DataType & data)
    {
        return cmnData<MatrixType>::HumanReadable(data.matrix());
    }

    static void SerializeText(const DataType & data,
                          std::ostream & outputStream,
                          const char delimiter) CISST_THROW(std::runtime_error)
    {
        cmnData<MatrixType>::SerializeText(data.matrix(), outputStream, delimiter);
    }

    static void DeSerializeText(DataType & data,
                                std::istream & inputStream,
                                const char delimiter) CISST_THROW(std::runtime_error)
    {
        cmnData<MatrixType>::DeSerializeText(data.matrix(), inputStream, delimiter);
    }

    static std::string SerializeDescription(const DataType & data,
                                            const char delimiter,
                                            const std::string & userDescription = "T")
    {
        return cmnData<MatrixType>::SerializeDescription(data.matrix(), delimiter, userDescription);
    }

    static void SerializeBinary(const DataType & data, std::ostream & outputStream)
        CISST_THROW(std::runtime_error)
    {
        cmnData<MatrixType>::SerializeBinary(data.matrix(), outputStream);
    }

    static void DeSerializeBinary(DataType & data,
                                  std::istream & inputStream,
                                  const cmnDataFormat & localFormat,
                                  const cmnDataFormat & remoteFormat)
        CISST_THROW(std::runtime_error)
    {
        cmnData<MatrixType>::DeSerializeBinary(data.matrix(), inputStream, localFormat, remoteFormat);
    }

    static bool ScalarNumberIsFixed(const DataType & data)
    {
        return cmnData<MatrixType>::ScalarNumberIsFixed(data.matrix());
    }

    static size_t ScalarNumber(const DataType & data)
    {
        return cmnData<MatrixType>::ScalarNumber(data.matrix());
    }

    static std::string ScalarDescription(const DataType & data, const size_t index,
                                         const std::string & userDescription = "T")
        CISST_THROW(std::out_of_range)
    {
        return cmnData<MatrixType>::ScalarDescription(data.matrix(), index, userDescription);
    }

    static double Scalar(const DataType & data, const size_t index)
        CISST_THROW(std::out_of_range)
    {
        return cmnData<MatrixType>::Scalar(data.matrix(), index);
    }
};

// Implement cmnData<Eigen::Quaternion> in terms of cmnData<Eigen::Vector4>.
//
// Note: `cmnData<>` must be specialized for `Derived::Scalar`, and must have
// fixed scalar number.
template <typename Scalar_>
class cmnData<Eigen::Quaternion<Scalar_>>
{
public:
    enum {IS_SPECIALIZED = 1};

    using DataType = Eigen::Quaternion<Scalar_>;

    static void Copy(DataType & data, const DataType & source)
    {
        data = source;
    }

    static std::string HumanReadable(const DataType & data)
    {
        std::stringstream ss;
        ss << "qx: " << data.x() << ", "
           << "qy: " << data.y() << ", "
           << "qz: " << data.z() << ", "
           << "qw: " << data.x();
        return ss.str();
    }

    static void SerializeText(const DataType & data,
                          std::ostream & outputStream,
                          const char delimiter) CISST_THROW(std::runtime_error)
    {
        cmnData<Eigen::Vector4<Scalar_>>::SerializeText(data.coeffs(), outputStream, delimiter);
    }

    static void DeSerializeText(DataType & data,
                                std::istream & inputStream,
                                const char delimiter) CISST_THROW(std::runtime_error)
    {
        cmnData<Eigen::Vector4<Scalar_>>::DeSerializeText(data.coeffs(), inputStream, delimiter);
    }

    static std::string SerializeDescription(const DataType& CMN_UNUSED(data),
                                            const char delimiter,
                                            const std::string & userDescription = "q")
    {
        std::string prefix = userDescription.empty() ? "q" : userDescription;

        std::stringstream ss;
        ss << userDescription << ".x" << delimiter
           << userDescription << ".y" << delimiter
           << userDescription << ".z" << delimiter
           << userDescription << ".w" << delimiter;
        return ss.str();
    }

    static void SerializeBinary(const DataType & data, std::ostream & outputStream)
        CISST_THROW(std::runtime_error)
    {
        cmnData<Eigen::Vector4<Scalar_>>::SerializeBinary(data.coeffs(), outputStream);
    }

    static void DeSerializeBinary(DataType & data,
                                  std::istream & inputStream,
                                  const cmnDataFormat & localFormat,
                                  const cmnDataFormat & remoteFormat)
        CISST_THROW(std::runtime_error)
    {
        cmnData<Eigen::Vector4<Scalar_>>::DeSerializeBinary(data.coeffs(), inputStream, localFormat, remoteFormat);
    }

    static bool ScalarNumberIsFixed(const DataType & CMN_UNUSED(data))
    {
        return true;
    }

    static size_t ScalarNumber(const DataType & CMN_UNUSED(data))
    {
        return 4;
    }

    static std::string ScalarDescription(const DataType & CMN_UNUSED(data), const size_t index,
                                         const std::string & userDescription = "q")
        CISST_THROW(std::out_of_range)
    {
        std::string prefix = userDescription.empty() ? "q" : userDescription;

        switch (index) {
        case 0:
            return prefix + ".x";
        case 1:
            return prefix + ".y";
        case 2:
            return prefix + ".z";
        case 3:
            return prefix + ".w";
        default:
            throw std::out_of_range("ScalarDescription index out of range");
        }
    }

    static double Scalar(const DataType & data, const size_t index)
        CISST_THROW(std::out_of_range)
    {
        return cmnData<Eigen::Vector4<Scalar_>>(data.coeffs(), index);
    }
};

// Specialization for cmnData<Eigen::Rotation2D>.
//
// Note: `cmnData<>` must be specialized for `Derived::Scalar`, and must have
// fixed scalar number.
template <typename Scalar_>
class cmnData<Eigen::Rotation2D<Scalar_>>
{
public:
    enum {IS_SPECIALIZED = 1};

    using DataType = Eigen::Rotation2D<Scalar_>;
    using MatrixType = typename DataType::RotationMatrixType;

    static void Copy(DataType & data, const DataType & source)
    {
        data = source;
    }

    static std::string HumanReadable(const DataType & data)
    {
        std::string description = "angle: " + std::to_string(data.angle());
        return description;
    }

    static void SerializeText(const DataType & data,
                          std::ostream & outputStream,
                          const char delimiter) CISST_THROW(std::runtime_error)
    {
        cmnData<Scalar_>::SerializeText(data.angle(), outputStream, delimiter);
    }

    static void DeSerializeText(DataType & data,
                                std::istream & inputStream,
                                const char delimiter) CISST_THROW(std::runtime_error)
    {
        Scalar_ angle;
        cmnData<Scalar_>::DeSerializeText(angle, inputStream, delimiter);
        data = Eigen::Rotation2D<Scalar_>(angle);
    }

    static std::string SerializeDescription(const DataType & CMN_UNUSED(data),
                                            const char CMN_UNUSED(delimiter),
                                            const std::string & userDescription = "R")
    {
        std::string prefix = userDescription.empty() ? "R" : userDescription;
        return prefix + ".angle";
    }

    static void SerializeBinary(const DataType & data, std::ostream & outputStream)
        CISST_THROW(std::runtime_error)
    {
        cmnData<Scalar_>::SerializeBinary(data.angle(), outputStream);
    }

    static void DeSerializeBinary(DataType & data,
                                  std::istream & inputStream,
                                  const cmnDataFormat & localFormat,
                                  const cmnDataFormat & remoteFormat)
        CISST_THROW(std::runtime_error)
    {
        Scalar_ angle;
        cmnData<Scalar_>::DeSerializeBinary(angle, inputStream, localFormat, remoteFormat);
        data = Eigen::Rotation2D<Scalar_>(angle);
    }

    static bool ScalarNumberIsFixed(const DataType & CMN_UNUSED(data))
    {
        return true;
    }

    static size_t ScalarNumber(const DataType & CMN_UNUSED(data))
    {
        return 1;
    }

    static std::string ScalarDescription(const DataType & CMN_UNUSED(data), const size_t index,
                                         const std::string & userDescription = "R")
        CISST_THROW(std::out_of_range)
    {
        if (index == 0) {
            std::string prefix = userDescription.empty() ? "R" : userDescription;
            return prefix + ".angle";
        } else {
            throw std::out_of_range("ScalarDescription index out of range");
        }
    }

    static double Scalar(const DataType & data, const size_t index)
        CISST_THROW(std::out_of_range)
    {
        if (index == 0) {
            return data.angle();
        } else {
            throw std::out_of_range("Scalar index out of range");
        }
    }
};

#endif // _cmnDataFunctionsEigen_h
