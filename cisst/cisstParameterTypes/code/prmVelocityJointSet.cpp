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

prmVelocityJointSet::~prmVelocityJointSet()
{
}

void prmVelocityJointSet::SetSize(size_type size)
{
    MaskMember.SetSize(size);
    GuardMember.SetSize(size);
    GoalMember.SetSize(size);
    AccelerationMember.SetSize(size);
    DecelerationMember.SetSize(size);

}

void prmVelocityJointSet::SerializeRaw(std::ostream & outputStream) const
{
    BaseType::SerializeRaw(outputStream);

    MaskMember.SerializeRaw(outputStream);
    GuardMember.SerializeRaw(outputStream);
    GoalMember.SerializeRaw(outputStream);
    AccelerationMember.SerializeRaw(outputStream);
    DecelerationMember.SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream,IsPositionGuardedMember);

}

void prmVelocityJointSet::DeSerializeRaw(std::istream & inputStream)
{
    BaseType::DeSerializeRaw(inputStream);
    MaskMember.DeSerializeRaw(inputStream);
    GuardMember.DeSerializeRaw(inputStream);
    GoalMember.DeSerializeRaw(inputStream);
    AccelerationMember.DeSerializeRaw(inputStream);
    DecelerationMember.DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream,IsPositionGuardedMember);

}
