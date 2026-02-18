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
#include <type_traits>
#ifndef _cmnDataFunctionsEigen_h
#define _cmnDataFunctionsEigen_h

#include <cisstCommon/cmnDataFunctions.h>
#include <Eigen/Core>

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
        std::string prefix = (userDescription == "") ? "m" : userDescription;
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
                    data.coef(row, col),
                    delimiter,
                    index_suffix
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
        if (index == 0) {
            return cmnData<size_t>::ScalarDescription(data.rows(), 0, userDescription + ".size");
        } else if (index == 1) {
            return cmnData<size_t>::ScalarDescription(data.cols(), 0, userDescription + ".size");
        }

        std::stringstream suffix;
        suffix << "[" << data.rows() << "," << data.cols() << "]";
        return cmnData<typename DataType::Scalar>::ScalarDescription(
            // reshaped() provides linearly-indexed view
            data.template reshaped<Eigen::RowMajor>().coeff(index - 2),
            0,
            userDescription + suffix.str()
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

        // reshaped() provides linearly-indexed view
        return data.template reshaped<Eigen::RowMajor>().coeff(index - 2);
    }
};

#endif // _cmnDataFunctionsEigen_h
