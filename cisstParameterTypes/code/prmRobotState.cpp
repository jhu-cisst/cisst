/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
Author(s):	Marcin Balicki
Created on:   2008-09-14

(C) Copyright 2008-2023 Johns Hopkins University (JHU), All Rights
Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstParameterTypes/prmRobotState.h>

prmRobotState::prmRobotState()
{
    CartesianPosition().SetSize(6);
    CartesianVelocity().SetSize(6);
    CartesianPositionGoal().SetSize(6);
    CartesianVelocityGoal().SetSize(6);
    CartesianVelocityError().SetSize(6);
    CartesianPositionError().SetSize(6);

}

prmRobotState::prmRobotState(size_type size)
{
    SetSize(size);
}

void prmRobotState::ToStream(std::ostream & outputStream) const
{

    outputStream << "JointName: "                   << this->JointNameMember
                 << "\nJointPosition: "             << this->JointPositionMember
                 << "\nJointVelocity : "		    << this->JointVelocityMember
                 << "\nJointPositionGoal: "		    << this->JointPositionGoalMember
                 << "\nJointVelocityGoal: "         << this->JointVelocityGoalMember
                 << "\nJointPositionError: "	    << this->JointPositionErrorMember
                 << "\nJointVelocityError: "	    << this->JointVelocityErrorMember
                 << "\nReferenceFrame: "            << this->ReferenceFrameMember
                 << "\nMovingFrame: "               << this->MovingFrameMember
                 << "\nCartesianPosition: "         << this->CartesianPositionMember
                 << "\nCartesianVelocity : "		<< this->CartesianVelocityMember
                 << "\nCartesianPositionGoal: "		<< this->CartesianPositionGoalMember
                 << "\nCartesianVelocityGoal: "     << this->CartesianVelocityGoalMember
                 << "\nCartesianVelocityError: "	<< this->CartesianVelocityErrorMember
                 << "\nCartesianPositionError: "	<< this->CartesianPositionErrorMember
                 << "\nEndEffectorFrame: "          << this->EndEffectorFrameMember;
}

void prmRobotState::SetSize(size_type size)
{
    JointNameMember.SetSize(size);
    JointPositionMember.SetSize(size);
    JointVelocityMember.SetSize(size);
    JointPositionGoalMember.SetSize(size);
    JointVelocityGoalMember.SetSize(size);
    JointPositionErrorMember.SetSize(size);
    JointVelocityErrorMember.SetSize(size);
    /* these are 6 by nature.   */
    CartesianPositionMember.SetSize(6);
    CartesianVelocityMember.SetSize(6);
    CartesianPositionGoalMember.SetSize(6);
    CartesianVelocityGoalMember.SetSize(6);
    CartesianVelocityErrorMember.SetSize(6);
    CartesianPositionErrorMember.SetSize(6);

}


void prmRobotState::SerializeRaw(std::ostream & outputStream) const
{
    BaseType::SerializeRaw(outputStream);

    this->JointNameMember.SerializeRaw(outputStream);
    this->JointPositionMember.SerializeRaw(outputStream);
    this->JointVelocityMember.SerializeRaw(outputStream);
    this->JointPositionGoalMember.SerializeRaw(outputStream);
    this->JointVelocityGoalMember.SerializeRaw(outputStream);
    this->JointPositionErrorMember.SerializeRaw(outputStream);
    this->JointVelocityErrorMember.SerializeRaw(outputStream);
    this->CartesianPositionMember.SerializeRaw(outputStream);
    this->CartesianVelocityMember.SerializeRaw(outputStream);
    this->CartesianPositionGoalMember.SerializeRaw(outputStream);
    this->CartesianVelocityGoalMember.SerializeRaw(outputStream);
    this->CartesianVelocityErrorMember.SerializeRaw(outputStream);
    this->CartesianPositionErrorMember.SerializeRaw(outputStream);
    this->EndEffectorFrameMember.SerializeRaw(outputStream);

}

void prmRobotState::DeSerializeRaw(std::istream & inputStream)
{
    BaseType::DeSerializeRaw(inputStream);

    this->JointNameMember.DeSerializeRaw(inputStream);
    this->JointPositionMember.DeSerializeRaw(inputStream);
    this->JointVelocityMember.DeSerializeRaw(inputStream);
    this->JointPositionGoalMember.DeSerializeRaw(inputStream);
    this->JointVelocityGoalMember.DeSerializeRaw(inputStream);
    this->JointPositionErrorMember.DeSerializeRaw(inputStream);
    this->JointVelocityErrorMember.DeSerializeRaw(inputStream);
    this->CartesianPositionMember.DeSerializeRaw(inputStream);
    this->CartesianVelocityMember.DeSerializeRaw(inputStream);
    this->CartesianPositionGoalMember.DeSerializeRaw(inputStream);
    this->CartesianVelocityGoalMember.DeSerializeRaw(inputStream);
    this->CartesianVelocityErrorMember.DeSerializeRaw(inputStream);
    this->CartesianPositionErrorMember.DeSerializeRaw(inputStream);
    this->EndEffectorFrameMember.DeSerializeRaw(inputStream);

}

bool prmRobotStateToStateJointMeasured(const prmRobotState & input, prmStateJoint & output)
{
    output.Valid() = input.Valid();
    output.Timestamp() = input.Timestamp();
    output.Name().ForceAssign(input.JointName());
    output.Position().ForceAssign(input.JointPosition());
    output.Velocity().ForceAssign(input.JointVelocity());
    return true;
}

bool prmRobotStateToStateJointSetpoint(const prmRobotState & input, prmStateJoint & output)
{
    output.Valid() = input.Valid();
    output.Timestamp() = input.Timestamp();
    output.Name().ForceAssign(input.JointName());
    output.Position().ForceAssign(input.JointPositionGoal());
    output.Velocity().ForceAssign(input.JointVelocityGoal());
    return true;
}

bool prmRobotStateToCartesianPositionRxRyMeasured(const prmRobotState & input, prmPositionCartesianGet & output)
{
    if (input.CartesianPosition().size() != 5) {
        return false;
    }
    output.Valid() = true;
    output.Timestamp() = input.Timestamp();
    output.ReferenceFrame() = input.ReferenceFrame();
    output.MovingFrame() = input.MovingFrame();
    output.Position().Translation().Assign(input.CartesianPosition().Ref(3));
    vctMatRot3 rotation =
        vctMatRot3(vctAxAnRot3(vct3(0.0, 1.0, 0.0), input.CartesianPosition().at(4)))
        * vctMatRot3(vctAxAnRot3(vct3(1.0, 0.0, 0.0), input.CartesianPosition().at(3)));
    output.Position().Rotation().From(rotation);
    return true;
}

bool prmRobotStateToCartesianPositionRxRySetpoint(const prmRobotState & input, prmPositionCartesianGet & output)
{
    if (input.CartesianPositionGoal().size() != 5) {
        return false;
    }
    output.Valid() = input.Valid();
    output.Timestamp() = input.Timestamp();
    output.ReferenceFrame() = input.ReferenceFrame();
    output.MovingFrame() = input.MovingFrame();
    output.Position().Translation().Assign(input.CartesianPositionGoal().Ref(3));
    vctMatRot3 rotation =
        vctMatRot3(vctAxAnRot3(vct3(0.0, 1.0, 0.0), input.CartesianPositionGoal().at(4)))
        * vctMatRot3(vctAxAnRot3(vct3(1.0, 0.0, 0.0), input.CartesianPositionGoal().at(3)));
    output.Position().Rotation().From(rotation);
    return true;
}

//  $Log
