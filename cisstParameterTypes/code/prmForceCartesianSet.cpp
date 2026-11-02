/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):    Rajesh Kumar, Anton Deguet
  Created on:   2008-03-12

  (C) Copyright 2008-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstParameterTypes/prmForceCartesianSet.h>

prmForceCartesianSet::~prmForceCartesianSet() {}

void prmForceCartesianSet::ToStream(std::ostream & outputStream) const
{
    BaseType::ToStream(outputStream);
    outputStream << "\nForce: " << this->ForceMember
                 << "\nMask: " << this->MaskMember;
}

void prmForceCartesianSet::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                                          bool headerOnly, const std::string & headerPrefix) const {
    BaseType::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
    outputStream << delimiter;
    
    if (headerOnly) {
        outputStream << cmnData<ForceType>::SerializeDescription(ForceMember, delimiter, headerPrefix + "-force");
    } else {
        cmnData<ForceType>::SerializeText(ForceMember, outputStream, delimiter);
    }
    outputStream << delimiter;
    if (headerOnly) {
        outputStream << cmnData<MaskType>::SerializeDescription(MaskMember, delimiter, headerPrefix + "-mask");
    } else {
        cmnData<MaskType>::SerializeText(MaskMember, outputStream, delimiter);
    }
}

void prmForceCartesianSet::SerializeRaw(std::ostream & outputStream) const 
{
    BaseType::SerializeRaw(outputStream);
    cmnData<ForceType>::SerializeBinary(ForceMember, outputStream);
    cmnData<MaskType>::SerializeBinary(MaskMember, outputStream);
}

void prmForceCartesianSet::DeSerializeRaw(std::istream & inputStream) 
{
    BaseType::DeSerializeRaw(inputStream);
    cmnDataFormat format;
    cmnData<ForceType>::DeSerializeBinary(ForceMember, inputStream, format, format);
    cmnData<MaskType>::DeSerializeBinary(MaskMember, inputStream, format, format);
}
