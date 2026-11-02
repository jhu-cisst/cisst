/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):    Rajesh Kumar, Anton Deguet
  Created on:   2008-03-12

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnDataFormat.h>
#include <cisstParameterTypes/prmForceCartesianGet.h>

prmForceCartesianGet::~prmForceCartesianGet() {}

void prmForceCartesianGet::ToStream(std::ostream & outputStream) const
{
    outputStream << "Force: " << ForceMember
                 << "\nMask: " << MaskMember;
}

void prmForceCartesianGet::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                                       bool headerOnly, const std::string & headerPrefix) const
{
    BaseType::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
    outputStream << delimiter;
    
    // TODO: Why isn't MaskMember serialized? should it be deprecated

    if (headerOnly) {
        outputStream << cmnData<ForceType>::SerializeDescription(ForceMember, delimiter, headerPrefix);
    } else {
        cmnData<ForceType>::SerializeText(ForceMember, outputStream, delimiter);
    }
}

void prmForceCartesianGet::SerializeRaw(std::ostream & outputStream) const 
{
    BaseType::SerializeRaw(outputStream);
    cmnData<ForceType>::SerializeBinary(ForceMember, outputStream);
}

void prmForceCartesianGet::DeSerializeRaw(std::istream & inputStream) 
{
    BaseType::DeSerializeRaw(inputStream);
    cmnDataFormat format;
    cmnData<ForceType>::DeSerializeBinary(ForceMember, inputStream, format, format);
}
