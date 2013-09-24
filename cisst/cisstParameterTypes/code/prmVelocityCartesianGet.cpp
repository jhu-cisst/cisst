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

#include <cisstParameterTypes/prmVelocityCartesianGet.h>

prmVelocityCartesianGet::~prmVelocityCartesianGet()
{
}


void prmVelocityCartesianGet::ToStream(std::ostream & outputStream) const
{
    BaseType::ToStream(outputStream);
    outputStream << "Linear velocity: " << this->VelocityLinearMember
                 << "\nAngular velocity: " << this->VelocityAngularMember;
}

void prmVelocityCartesianGet::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                                          bool headerOnly, const std::string & headerPrefix) const
{
    BaseType::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
    outputStream << delimiter;
    this->VelocityLinearMember.ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix + "-linear");
    outputStream << delimiter;
    this->VelocityAngularMember.ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix + "-angular");
}

void prmVelocityCartesianGet::SerializeRaw(std::ostream & outputStream) const 
{
    BaseType::SerializeRaw(outputStream);
    this->VelocityLinearMember.SerializeRaw(outputStream);
    this->VelocityAngularMember.SerializeRaw(outputStream);
}

void prmVelocityCartesianGet::DeSerializeRaw(std::istream & inputStream) 
{
    BaseType::DeSerializeRaw(inputStream);
    this->VelocityLinearMember.DeSerializeRaw(inputStream);
    this->VelocityAngularMember.DeSerializeRaw(inputStream);
}
