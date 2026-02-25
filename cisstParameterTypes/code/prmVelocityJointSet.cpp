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

#include <cisstParameterTypes/prmVelocityJointSet.h>

#include <cisstCommon/cmnDataFunctionsEigen.h>

prmVelocityJointSet::~prmVelocityJointSet() {}

void prmVelocityJointSet::SetSize(size_type size)
{
    MaskMember.resize(size);
    GuardMember.resize(size);
    GoalMember.resize(size);
    AccelerationMember.resize(size);
    DecelerationMember.resize(size);
}

void prmVelocityJointSet::SerializeRaw(std::ostream & outputStream) const
{
    BaseType::SerializeRaw(outputStream);

    cmnData<Eigen::ArrayX<bool>>::SerializeBinary(MaskMember, outputStream);
    cmnData<Eigen::VectorXd>::SerializeBinary(GuardMember, outputStream);
    cmnData<Eigen::VectorXd>::SerializeBinary(GoalMember, outputStream);
    cmnData<Eigen::VectorXd>::SerializeBinary(AccelerationMember, outputStream);
    cmnData<Eigen::VectorXd>::SerializeBinary(DecelerationMember, outputStream);
    cmnSerializeRaw(outputStream, IsPositionGuardedMember);
}

void prmVelocityJointSet::DeSerializeRaw(std::istream & inputStream)
{
    BaseType::DeSerializeRaw(inputStream);

    cmnDataFormat format;
    cmnData<Eigen::ArrayX<bool>>::DeSerializeBinary(MaskMember, inputStream, format, format);
    cmnData<Eigen::VectorXd>::DeSerializeBinary(GuardMember, inputStream, format, format);
    cmnData<Eigen::VectorXd>::DeSerializeBinary(GoalMember, inputStream, format, format);
    cmnData<Eigen::VectorXd>::DeSerializeBinary(AccelerationMember, inputStream, format, format);
    cmnData<Eigen::VectorXd>::DeSerializeBinary(DecelerationMember, inputStream, format, format);
    cmnDeSerializeRaw(inputStream,IsPositionGuardedMember);
}
