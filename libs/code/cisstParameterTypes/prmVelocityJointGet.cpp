/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
Author(s):	Rajesh Kumar, Anton Deguet
Created on:   2008-04-10

(C) Copyright 2008 Johns Hopkins University (JHU), All Rights
Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstParameterTypes/prmVelocityJointGet.h>

prmVelocityJointGet::~prmVelocityJointGet()
{
}

void prmVelocityJointGet::SetSize(size_type size)
{
    VelocityMember.SetSize(size);
}

void prmVelocityJointGet::ToStream(std::ostream & outputStream) const
{
    BaseType::ToStream(outputStream);
    outputStream << " Velocity: " << this->VelocityMember;
}

void prmVelocityJointGet::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                                      bool headerOnly, const std::string & headerPrefix) const
{
    BaseType::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
    outputStream << delimiter;
    this->VelocityMember.ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
}

void prmVelocityJointGet::SerializeRaw(std::ostream & outputStream) const 
{
    BaseType::SerializeRaw(outputStream);
    this->VelocityMember.SerializeRaw(outputStream);
}

void prmVelocityJointGet::DeSerializeRaw(std::istream & inputStream) 
{
    BaseType::DeSerializeRaw(inputStream);
    this->VelocityMember.DeSerializeRaw(inputStream);
}
