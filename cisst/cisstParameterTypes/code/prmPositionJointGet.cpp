/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Rajesh Kumar, Anton Deguet
  Created on: 2008-04-10

  (C) Copyright 2008-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstParameterTypes/prmPositionJointGet.h>

prmPositionJointGet::~prmPositionJointGet()
{
}

void prmPositionJointGet::SetSize(size_t size)
{
    PositionMember.SetSize(size);
}


void prmPositionJointGet::ToStream(std::ostream & outputStream) const
{
    BaseType::ToStream(outputStream);
    outputStream << "\nPosition: " << this->PositionMember;
}

void prmPositionJointGet::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                                      bool headerOnly, const std::string & headerPrefix) const {
    BaseType::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
    outputStream << delimiter;
    this->PositionMember.ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
}

void prmPositionJointGet::SerializeRaw(std::ostream & outputStream) const
{
    BaseType::SerializeRaw(outputStream);
    this->PositionMember.SerializeRaw(outputStream);
}

void prmPositionJointGet::DeSerializeRaw(std::istream & inputStream)
{
    BaseType::DeSerializeRaw(inputStream);
    this->PositionMember.DeSerializeRaw(inputStream);
}

size_t prmPositionJointGet::GetNumberOfScalars(const bool visualizable) const
{
    return BaseType::GetNumberOfScalars(visualizable) + PositionMember.size();
}

double prmPositionJointGet::GetScalarAsDouble(const size_t index) const
{
    if (index >= GetNumberOfScalars()) {
        return 0.0;
    }
    if (index < BaseType::GetNumberOfScalars()) {
        return BaseType::GetScalarAsDouble(index);
    }
    const ptrdiff_t offset = index - BaseType::GetNumberOfScalars();
    return static_cast<double>(this->PositionMember.at(offset));
}

std::string prmPositionJointGet::GetScalarName(const size_t index) const
{
    if (index >= GetNumberOfScalars()) {
        return "index out of range";
    }
    if (index < BaseType::GetNumberOfScalars()) {
        return BaseType::GetScalarName(index);
    }
    const ptrdiff_t offset = index - BaseType::GetNumberOfScalars();
    std::stringstream ss;
    ss << "JointPosition";
    ss << offset;
    return ss.str();
}
