/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2012-07-09

  (C) Copyright 2012-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once

#ifndef _vctDataFunctionsTransformations_h
#define _vctDataFunctionsTransformations_h

#include <cisstCommon/cmnDataFunctions.h>
#include <cisstVector/vctDataFunctionsFixedSizeVector.h>
#include <cisstVector/vctDataFunctionsFixedSizeMatrix.h>

#if CISST_HAS_JSON
#include <cisstVector/vctDataFunctionsTransformationsJSON.h>
#endif // CISST_HAS_JSON

template <class _rotationType>
class cmnData<vctFrameBase<_rotationType> >
{
public:
    enum {IS_SPECIALIZED = 1};

    typedef vctFrameBase<_rotationType> DataType;
    typedef typename DataType::TranslationType TranslationType;
    typedef typename DataType::RotationType RotationType;

    static void Copy(DataType & data, const DataType & source)
    {
        data.Assign(source);
    }

    static void SerializeBinary(const DataType & data, std::ostream & outputStream)
        CISST_THROW(std::runtime_error)
    {
        cmnData<TranslationType>::SerializeBinary(data.Translation(), outputStream);
        cmnData<RotationType>::SerializeBinary(data.Rotation(), outputStream);
    }

    static void DeSerializeBinary(DataType & data, std::istream & inputStream,
                                  const cmnDataFormat & localFormat, const cmnDataFormat & remoteFormat)
        CISST_THROW(std::runtime_error)
    {
        cmnData<TranslationType>::DeSerializeBinary(data.Translation(), inputStream, localFormat, remoteFormat);
        cmnData<RotationType>::DeSerializeBinary(data.Rotation(), inputStream, localFormat, remoteFormat);
    }

    static void SerializeText(const DataType & data, std::ostream & outputStream,
                              const char delimiter = ',')
        CISST_THROW(std::runtime_error)
    {
        cmnData<TranslationType>::SerializeText(data.Translation(), outputStream, delimiter);
        outputStream << delimiter;
        cmnData<RotationType>::SerializeText(data.Rotation(), outputStream, delimiter);
    }

    static std::string HumanReadable(const DataType & data)
    {
        return data.ToString();
    }

    static std::string SerializeDescription(const DataType & data, const char delimiter = ',',
                                            const std::string & userDescription = "frm3")
    {
        const std::string prefix = (userDescription == "") ? "" : (userDescription + ".");
        std::stringstream description;
        description << cmnData<TranslationType>::SerializeDescription(data.Translation(), delimiter, prefix + "Translation")
                    << delimiter
                    << cmnData<RotationType>::SerializeDescription(data.Rotation(), delimiter, prefix + "Rotation");
        return description.str();
    }

    static void DeSerializeText(DataType & data, std::istream & inputStream,
                                const char delimiter = ',')
        CISST_THROW(std::runtime_error)
    {
        cmnData<TranslationType>::DeSerializeText(data.Translation(), inputStream, delimiter);
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "vctFrameBase");
        cmnData<RotationType>::DeSerializeText(data.Rotation(), inputStream, delimiter);
    }

    static bool ScalarNumberIsFixed(const DataType & data)
    {
        return (cmnData<TranslationType>::ScalarNumberIsFixed(data.Translation())
                && cmnData<RotationType>::ScalarNumberIsFixed(data.Rotation()));
    }

    static size_t ScalarNumber(const DataType & data)
    {
        return (cmnData<TranslationType>::ScalarNumber(data.Translation())
                + cmnData<RotationType>::ScalarNumber(data.Rotation()));
    }

    static std::string ScalarDescription(const DataType & data, const size_t & index,
                                         const std::string & userDescription = "frm3")
        CISST_THROW(std::out_of_range)
    {
        std::string prefix = (userDescription == "") ? "" : (userDescription + ".");
        const size_t scalarNumberTranslation = cmnData<TranslationType>::ScalarNumber(data.Translation());
        if (index < scalarNumberTranslation) {
            return prefix + cmnData<TranslationType>::ScalarDescription(data.Translation(), index, "Translation");
        }
        return prefix + cmnData<RotationType>::ScalarDescription(data.Rotation(), index - scalarNumberTranslation, "Rotation");
    }

    static double Scalar(const DataType & data, const size_t & index)
        CISST_THROW(std::out_of_range)
    {
        const size_t scalarNumberTranslation = cmnData<TranslationType>::ScalarNumber(data.Translation());
        if (index < scalarNumberTranslation) {
            return cmnData<TranslationType>::Scalar(data.Translation(), index);
        }
        return cmnData<RotationType>::Scalar(data.Rotation(), index - scalarNumberTranslation);
    }
};

// pass through class for rotation matrix
template <class _elementType, bool _rowMajor>
class cmnData<vctMatrixRotation3<_elementType, _rowMajor> >
{
public:
    enum {IS_SPECIALIZED = 1};
    typedef vctMatrixRotation3<_elementType, _rowMajor> DataType;
    typedef typename DataType::ContainerType ContainerType;

    static void Copy(DataType & data, const DataType & source)
    {
        cmnData<ContainerType>::Copy(data, source);
    }

    static void SerializeBinary(const DataType & data, std::ostream & outputStream)
        CISST_THROW(std::runtime_error)
    {
        cmnData<ContainerType>::SerializeBinary(data, outputStream);
    }

    static void DeSerializeBinary(DataType & data, std::istream & inputStream,
                                  const cmnDataFormat & localFormat, const cmnDataFormat & remoteFormat)
        CISST_THROW(std::runtime_error)
    {
        cmnData<ContainerType>::DeSerializeBinary(data, inputStream, localFormat, remoteFormat);
    }

    static void SerializeText(const DataType & data, std::ostream & outputStream,
                              const char delimiter = ',')
        CISST_THROW(std::runtime_error)
    {
        cmnData<ContainerType>::SerializeText(data, outputStream, delimiter);
    }

    static std::string HumanReadable(const DataType & data)
    {
        return cmnData<ContainerType>::HumanReadable(data);
    }

    static std::string SerializeDescription(const DataType & data, const char delimiter = ',',
                                            const std::string & userDescription = "mr3")
    {
        return cmnData<ContainerType>::SerializeDescription(data, delimiter, userDescription);
    }

    static void DeSerializeText(DataType & data, std::istream & inputStream,
                                const char delimiter = ',')
        CISST_THROW(std::runtime_error)
    {
        return cmnData<ContainerType>::DeSerializeText(data, inputStream, delimiter);
    }

    static bool ScalarNumberIsFixed(const DataType & data)
    {
        return cmnData<ContainerType>::ScalarNumberIsFixed(data);
    }

    static size_t ScalarNumber(const DataType & data)
    {
        return cmnData<ContainerType>::ScalarNumber(data);
    }

    static std::string ScalarDescription(const DataType & data, const size_t & index,
                                         const std::string & userDescription = "mr3")
        CISST_THROW(std::out_of_range)
    {
       return  cmnData<ContainerType>::ScalarDescription(data, index, userDescription);
    }

    static double Scalar(const DataType & data, const size_t & index)
        CISST_THROW(std::out_of_range)
    {
        return cmnData<ContainerType>::Scalar(data, index);
    }
};


// pass through class for frame4x4
template <class _elementType, bool _rowMajor>
class cmnData<vctFrame4x4<_elementType, _rowMajor> >
{
public:
    enum {IS_SPECIALIZED = 1};
    typedef vctFrame4x4<_elementType, _rowMajor> DataType;
    typedef typename DataType::ContainerType ContainerType;

    static void Copy(DataType & data, const DataType & source)
    {
        cmnData<ContainerType>::Copy(data, source);
    }

    static void SerializeBinary(const DataType & data, std::ostream & outputStream)
        CISST_THROW(std::runtime_error)
    {
        cmnData<ContainerType>::SerializeBinary(data, outputStream);
    }

    static void DeSerializeBinary(DataType & data, std::istream & inputStream,
                                  const cmnDataFormat & localFormat, const cmnDataFormat & remoteFormat)
        CISST_THROW(std::runtime_error)
    {
        cmnData<ContainerType>::DeSerializeBinary(data, inputStream, localFormat, remoteFormat);
    }

    static void SerializeText(const DataType & data, std::ostream & outputStream,
                              const char delimiter = ',')
        CISST_THROW(std::runtime_error)
    {
        cmnData<ContainerType>::SerializeText(data, outputStream, delimiter);
    }

    static std::string HumanReadable(const DataType & data)
    {
        return cmnData<ContainerType>::HumanReadable(data);
    }

    static std::string SerializeDescription(const DataType & data, const char delimiter = ',',
                                            const std::string & userDescription = "mr3")
    {
        return cmnData<ContainerType>::SerializeDescription(data, delimiter, userDescription);
    }

    static void DeSerializeText(DataType & data, std::istream & inputStream,
                                const char delimiter = ',')
        CISST_THROW(std::runtime_error)
    {
        return cmnData<ContainerType>::DeSerializeText(data, inputStream, delimiter);
    }

    static bool ScalarNumberIsFixed(const DataType & data)
    {
        return cmnData<ContainerType>::ScalarNumberIsFixed(data);
    }

    static size_t ScalarNumber(const DataType & data)
    {
        return cmnData<ContainerType>::ScalarNumber(data);
    }

    static std::string ScalarDescription(const DataType & data, const size_t & index,
                                         const std::string & userDescription = "mr3")
        CISST_THROW(std::out_of_range)
    {
       return  cmnData<ContainerType>::ScalarDescription(data, index, userDescription);
    }

    static double Scalar(const DataType & data, const size_t & index)
        CISST_THROW(std::out_of_range)
    {
        return cmnData<ContainerType>::Scalar(data, index);
    }
};

#endif // _vctDataFunctionsTransformations_h
