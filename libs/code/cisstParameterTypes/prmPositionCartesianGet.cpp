/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Rajesh Kumar, Anton Deguet
  Created on:   2008-03-12

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstParameterTypes/prmPositionCartesianGet.h>

CMN_IMPLEMENT_SERVICES(prmPositionCartesianGet);

prmPositionCartesianGet::~prmPositionCartesianGet()
{
}


void prmPositionCartesianGet::ToStream(std::ostream & outputStream) const
{
    BaseType::ToStream(outputStream);
    outputStream << "\nReference frame: " << this->ReferenceFrameMember
                 << "\nMoving frame: " << this->MovingFrameMember
                 << "\nPosition: " << this->PositionMember;
}

void prmPositionCartesianGet::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                                          bool headerOnly, const std::string & headerPrefix) const {
    BaseType::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
    outputStream << delimiter;
    this->PositionMember.ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
}

void prmPositionCartesianGet::SerializeRaw(std::ostream & outputStream) const 
{
    BaseType::SerializeRaw(outputStream);
    this->PositionMember.SerializeRaw(outputStream);
}

void prmPositionCartesianGet::DeSerializeRaw(std::istream & inputStream) 
{
    BaseType::DeSerializeRaw(inputStream);
    this->PositionMember.DeSerializeRaw(inputStream);
}

unsigned int prmPositionCartesianGet::GetNumberOfScalar(const bool CMN_UNUSED(visualizable)) const 
{
    return 12; // 3 for 3D position, 9 for 3x3 rotation matrix
}

double prmPositionCartesianGet::GetScalarAsDouble(const size_t index) const 
{
    if (index >= GetNumberOfScalar()) {
        return 0.0;
    }

    if (index <= 2) {
        return static_cast<double>(this->PositionMember.Translation().at(index));
    } else {
        return static_cast<double>(this->PositionMember.Rotation().at(index - 3));
    }
}

std::string prmPositionCartesianGet::GetScalarName(const size_t index) const 
{
    if (index >= GetNumberOfScalar()) {
        return "N/A";
    }

    std::string signalName;
    switch (index) {
        case 0:  signalName = "translation x";  break;
        case 1:  signalName = "translation y";  break;
        case 2:  signalName = "translation z";  break;
        case 3:  signalName = "rotation (1,1)"; break;
        case 4:  signalName = "rotation (1,2)"; break;
        case 5:  signalName = "rotation (1,3)"; break;
        case 6:  signalName = "rotation (2,1)"; break;
        case 7:  signalName = "rotation (2,2)"; break;
        case 8:  signalName = "rotation (2,3)"; break;
        case 9:  signalName = "rotation (3,1)"; break;
        case 10: signalName = "rotation (3,2)"; break;
        case 11: signalName = "rotation (3,3)"; break;
    }
    return signalName;
}
