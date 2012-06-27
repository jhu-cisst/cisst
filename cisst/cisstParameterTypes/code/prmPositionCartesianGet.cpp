/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Rajesh Kumar, Anton Deguet
  Created on: 2008-03-12

  (C) Copyright 2008-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstParameterTypes/prmPositionCartesianGet.h>

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

size_t prmPositionCartesianGet::GetNumberOfScalars(const bool visualizable) const
{
    return BaseType::GetNumberOfScalars(visualizable) + 12; // 3 for 3D position, 9 for 3x3 rotation matrix
}

double prmPositionCartesianGet::GetScalarAsDouble(const size_t index) const
{
    if (index >= GetNumberOfScalars()) {
        return 0.0;
    }
    if (index < BaseType::GetNumberOfScalars()) {
        return BaseType::GetScalarAsDouble(index);
    }
    const ptrdiff_t offset = index - BaseType::GetNumberOfScalars();
    if (offset < 3) {
        return static_cast<double>(this->PositionMember.Translation().at(offset));
    } else {
        return static_cast<double>(this->PositionMember.Rotation().at(offset - 3));
    }
}


std::string prmPositionCartesianGet::GetScalarName(const size_t index) const
{
    if (index >= GetNumberOfScalars()) {
        return "index out of range";
    }
    if (index < BaseType::GetNumberOfScalars()) {
        return BaseType::GetScalarName(index);
    }
    const ptrdiff_t offset = index - BaseType::GetNumberOfScalars();

    // adeguet1 todo: move this code to cmnDataGetScalarName overloaded for frame, vector and matrices
    // adeguet1 todo: in cisstVector, add method to compute indices based on memory position, i.e. matrix.IndexForPosition(0) returns (0,0) - uses strides as well
    std::string signalName;
    switch (offset) {
        case 0:  signalName = "Position.Translation[0]"; break;
        case 1:  signalName = "Position.Translation[1]"; break;
        case 2:  signalName = "Position.Translation[2]"; break;
        case 3:  signalName = "Position.Rotation[0,0]"; break;
        case 4:  signalName = "Position.Rotation[0,1]"; break;
        case 5:  signalName = "Position.Rotation[0,2]"; break;
        case 6:  signalName = "Position.Rotation[1,0]"; break;
        case 7:  signalName = "Position.Rotation[1,1]"; break;
        case 8:  signalName = "Position.Rotation[1,2]"; break;
        case 9:  signalName = "Position.Rotation[2,0]"; break;
        case 10: signalName = "Position.Rotation[2,1]"; break;
        case 11: signalName = "Position.Rotation[2,2]"; break;
    }
    return signalName;
}
