/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
Author(s):  Rajesh Kumar, Anton Deguet
Created on: 2008-04-10

(C) Copyright 2008 Johns Hopkins University (JHU), All Rights
Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstParameterTypes/prmVelocityJointGet.h>

#include <cisstCommon/cmnDataFunctionsEigen.h>

prmVelocityJointGet::~prmVelocityJointGet() {}

void prmVelocityJointGet::SetSize(size_type size)
{
    VelocityMember.resize(size);
}

void prmVelocityJointGet::ToStream(std::ostream & outputStream) const
{
    BaseType::ToStream(outputStream);
    outputStream << " Velocity: " << VelocityMember;
}

void prmVelocityJointGet::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                                      bool headerOnly, const std::string & headerPrefix) const
{
    BaseType::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
    outputStream << delimiter;
    
    if (headerOnly) {
        outputStream << cmnData<Eigen::VectorXd>::SerializeDescription(VelocityMember, delimiter, headerPrefix);
    } else {
        cmnData<Eigen::VectorXd>::SerializeText(VelocityMember, outputStream, delimiter);
    }
}

void prmVelocityJointGet::SerializeRaw(std::ostream & outputStream) const 
{
    BaseType::SerializeRaw(outputStream);
    cmnData<Eigen::VectorXd>::SerializeBinary(VelocityMember, outputStream);
}

void prmVelocityJointGet::DeSerializeRaw(std::istream & inputStream) 
{
    BaseType::DeSerializeRaw(inputStream);
    cmnDataFormat format;
    cmnData<Eigen::VectorXd>::DeSerializeBinary(VelocityMember, inputStream, format, format);
}
