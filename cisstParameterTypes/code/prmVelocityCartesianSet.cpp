/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):    Rajesh Kumar, Anton Deguet
  Created on:   2008-03-12

  (C) Copyright 2008-2022 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnDataFormat.h>
#include <cisstParameterTypes/prmVelocityCartesianSet.h>

prmVelocityCartesianSet::~prmVelocityCartesianSet() {}

void prmVelocityCartesianSet::SerializeRaw(std::ostream & outputStream) const
{
    BaseType::SerializeRaw(outputStream);
    cmnData<Eigen::Vector3d>::SerializeBinary(Velocity, outputStream);
    cmnData<Eigen::Vector3d>::SerializeBinary(VelocityAngular, outputStream);
    cmnData<Eigen::Vector3d>::SerializeBinary(Acceleration, outputStream);
    cmnData<Eigen::Vector3d>::SerializeBinary(AccelerationAngular, outputStream);
    cmnData<Eigen::Array<bool, 6, 1>>::SerializeBinary(Mask, outputStream);
}

void prmVelocityCartesianSet::DeSerializeRaw(std::istream & inputStream)
{
    BaseType::DeSerializeRaw(inputStream);
    cmnDataFormat format;
    cmnData<Eigen::Vector3d>::DeSerializeBinary(Velocity, inputStream, format, format);
    cmnData<Eigen::Vector3d>::DeSerializeBinary(VelocityAngular, inputStream, format, format);
    cmnData<Eigen::Vector3d>::DeSerializeBinary(Acceleration, inputStream, format, format);
    cmnData<Eigen::Vector3d>::DeSerializeBinary(AccelerationAngular, inputStream, format, format);
    cmnData<Eigen::Array<bool, 6, 1>>::DeSerializeBinary(Mask, inputStream, format, format);
}
