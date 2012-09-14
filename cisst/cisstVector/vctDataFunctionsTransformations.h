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

#pragma once

#ifndef _vctDataFunctionsTransformations_h
#define _vctDataFunctionsTransformations_h

#include <cisstCommon/cmnDataFunctions.h>
#include <cisstVector/vctDataFunctionsFixedSizeVector.h>
#include <cisstVector/vctDataFunctionsFixedSizeMatrix.h>

template <class _rotationType>
void cmnDataCopy(vctFrameBase<_rotationType> & destination,
                 const vctFrameBase<_rotationType> & source)
{
    destination.Assign(source);
}


template <class _rotationType>
void cmnDataSerializeBinary(std::ostream & outputStream,
                            const vctFrameBase<_rotationType> & data)
    throw (std::runtime_error)
{
    cmnDataSerializeBinary(outputStream, data.Translation());
    cmnDataSerializeBinary(outputStream, data.Rotation());
}


template <class _rotationType>
void cmnDataDeSerializeBinary(std::istream & inputStream,
                              vctFrameBase<_rotationType> & data,
                              const cmnDataFormat & remoteFormat,
                              const cmnDataFormat & localFormat)
    throw (std::runtime_error)
{
    cmnDataDeSerializeBinary(inputStream, data.Translation(), remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, data.Rotation(), remoteFormat, localFormat);
}


template <class _rotationType>
void cmnDataSerializeText(std::ostream & outputStream,
                          const vctFrameBase<_rotationType> & data,
                          const char delimiter)
    throw (std::runtime_error)
{
    cmnDataSerializeText(outputStream, data.Translation(), delimiter);
    outputStream << delimiter;
    cmnDataSerializeText(outputStream, data.Rotation(), delimiter);
}


template <class _rotationType>
std::string cmnDataSerializeTextDescription(const vctFrameBase<_rotationType> & data,
                                            const char delimiter,
                                            const std::string & userDescription = "frm3")
{
    const std::string prefix = (userDescription == "") ? "" : (userDescription + ".");
    std::stringstream description;
    description << cmnDataSerializeTextDescription(data.Translation(), delimiter, prefix + "Translation")
                << delimiter
                << cmnDataSerializeTextDescription(data.Rotation(), delimiter, prefix + "Rotation");
    return description.str();
}


template <class _rotationType>
void cmnDataDeSerializeText(std::istream & inputStream,
                            vctFrameBase<_rotationType> & data,
                            const char delimiter)
    throw (std::runtime_error)
{
    cmnDataDeSerializeText(inputStream, data.Translation(), delimiter);
    cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "vctFrameBase");
    cmnDataDeSerializeText(inputStream, data.Rotation(), delimiter);
}


template <class _rotationType>
bool cmnDataScalarNumberIsFixed(const vctFrameBase<_rotationType> & data)
{
    return (cmnDataScalarNumberIsFixed(data.Translation()) && cmnDataScalarNumberIsFixed(data.Rotation()));
}


template <class _rotationType>
size_t cmnDataScalarNumber(const vctFrameBase<_rotationType> & data)
{
    return cmnDataScalarNumber(data.Translation()) + cmnDataScalarNumber(data.Rotation());
}


template <class _rotationType>
std::string
cmnDataScalarDescription(const vctFrameBase<_rotationType> & data,
                         const size_t & index,
                         const std::string & userDescription = "f")
    throw (std::out_of_range)
{
    std::string prefix = (userDescription == "") ? "" : (userDescription + ".");
    const size_t scalarNumberTranslation = cmnDataScalarNumber(data.Translation());
    if (index < scalarNumberTranslation) {
        return prefix + cmnDataScalarDescription(data.Translation(), index, "Translation");
    }
    return prefix + cmnDataScalarDescription(data.Rotation(), index - scalarNumberTranslation, "Rotation");
}


template <class _rotationType>
double
cmnDataScalar(const vctFrameBase<_rotationType> & data,
              const size_t & index)
    throw (std::out_of_range)
{
    const size_t scalarNumberTranslation = cmnDataScalarNumber(data.Translation());
    if (index < scalarNumberTranslation) {
        return cmnDataScalar(data.Translation(), index);
    }
    return cmnDataScalar(data.Rotation(), index - scalarNumberTranslation);
}


#endif // _vctDataFunctionsTransformations_h
