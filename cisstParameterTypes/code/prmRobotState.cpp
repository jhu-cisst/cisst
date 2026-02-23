/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
Author(s):    Marcin Balicki
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
    CartesianPosition().resize(6);
    CartesianVelocity().resize(6);
    CartesianPositionGoal().resize(6);
    CartesianVelocityGoal().resize(6);
    CartesianVelocityError().resize(6);
    CartesianPositionError().resize(6);
}

prmRobotState::prmRobotState(size_type size)
{
    SetSize(size);
}

void prmRobotState::ToStream(std::ostream & outputStream) const
{
    outputStream << "JointName: "                   << cmnData<std::vector<std::string>>::HumanReadable(JointNameMember)
                 << "\nJointPosition: "             << JointPositionMember
                 << "\nJointVelocity : "            << JointVelocityMember
                 << "\nJointPositionGoal: "         << JointPositionGoalMember
                 << "\nJointVelocityGoal: "         << JointVelocityGoalMember
                 << "\nJointPositionError: "        << JointPositionErrorMember
                 << "\nJointVelocityError: "        << JointVelocityErrorMember
                 << "\nReferenceFrame: "            << ReferenceFrameMember
                 << "\nMovingFrame: "               << MovingFrameMember
                 << "\nCartesianPosition: "         << CartesianPositionMember
                 << "\nCartesianVelocity : "        << CartesianVelocityMember
                 << "\nCartesianPositionGoal: "     << CartesianPositionGoalMember
                 << "\nCartesianVelocityGoal: "     << CartesianVelocityGoalMember
                 << "\nCartesianVelocityError: "    << CartesianVelocityErrorMember
                 << "\nCartesianPositionError: "    << CartesianPositionErrorMember
                 << "\nEndEffectorFrame: "          << EndEffectorFrameMember.matrix();
}

void prmRobotState::SetSize(size_type size)
{
    JointNameMember.resize(size);
    JointPositionMember.resize(size);
    JointVelocityMember.resize(size);
    JointPositionGoalMember.resize(size);
    JointVelocityGoalMember.resize(size);
    JointPositionErrorMember.resize(size);
    JointVelocityErrorMember.resize(size);
}

void prmRobotState::SerializeRaw(std::ostream& outputStream) const
{
    BaseType::SerializeRaw(outputStream);

    cmnData<std::vector<std::string>>::SerializeBinary(JointNameMember, outputStream);
    cmnData<Eigen::VectorXd>::SerializeBinary(JointPositionMember, outputStream);
    cmnData<Eigen::VectorXd>::SerializeBinary(JointVelocityMember, outputStream);
    cmnData<Eigen::VectorXd>::SerializeBinary(JointPositionGoalMember, outputStream);
    cmnData<Eigen::VectorXd>::SerializeBinary(JointVelocityGoalMember, outputStream);
    cmnData<Eigen::VectorXd>::SerializeBinary(JointPositionErrorMember, outputStream);
    cmnData<Eigen::VectorXd>::SerializeBinary(JointVelocityErrorMember, outputStream);
    cmnData<Eigen::VectorXd>::SerializeBinary(CartesianPositionMember, outputStream);
    cmnData<Eigen::VectorXd>::SerializeBinary(CartesianVelocityMember, outputStream);
    cmnData<Eigen::VectorXd>::SerializeBinary(CartesianPositionGoalMember, outputStream);
    cmnData<Eigen::VectorXd>::SerializeBinary(CartesianVelocityGoalMember, outputStream);
    cmnData<Eigen::VectorXd>::SerializeBinary(CartesianPositionErrorMember, outputStream);
    cmnData<Eigen::VectorXd>::SerializeBinary(CartesianVelocityErrorMember, outputStream);
    cmnData<Eigen::Isometry3d>::SerializeBinary(EndEffectorFrameMember, outputStream);
}

void prmRobotState::DeSerializeRaw(std::istream& inputStream)
{
    BaseType::DeSerializeRaw(inputStream);

    cmnDataFormat format;
    cmnData<std::vector<std::string>>::DeSerializeBinary(JointNameMember, inputStream, format, format);
    cmnData<Eigen::VectorXd>::DeSerializeBinary(JointPositionMember, inputStream, format, format);
    cmnData<Eigen::VectorXd>::DeSerializeBinary(JointVelocityMember, inputStream, format, format);
    cmnData<Eigen::VectorXd>::DeSerializeBinary(JointPositionGoalMember, inputStream, format, format);
    cmnData<Eigen::VectorXd>::DeSerializeBinary(JointVelocityGoalMember, inputStream, format, format);
    cmnData<Eigen::VectorXd>::DeSerializeBinary(JointPositionErrorMember, inputStream, format, format);
    cmnData<Eigen::VectorXd>::DeSerializeBinary(JointVelocityErrorMember, inputStream, format, format);
    cmnData<Eigen::VectorXd>::DeSerializeBinary(CartesianPositionMember, inputStream, format, format);
    cmnData<Eigen::VectorXd>::DeSerializeBinary(CartesianVelocityMember, inputStream, format, format);
    cmnData<Eigen::VectorXd>::DeSerializeBinary(CartesianPositionGoalMember, inputStream, format, format);
    cmnData<Eigen::VectorXd>::DeSerializeBinary(CartesianVelocityGoalMember, inputStream, format, format);
    cmnData<Eigen::VectorXd>::DeSerializeBinary(CartesianPositionErrorMember, inputStream, format, format);
    cmnData<Eigen::VectorXd>::DeSerializeBinary(CartesianVelocityErrorMember, inputStream, format, format);
    cmnData<Eigen::Isometry3d>::DeSerializeBinary(EndEffectorFrameMember, inputStream, format, format);
}

bool prmRobotStateToStateJointMeasured(const prmRobotState& input, prmStateJoint& output)
{
    output.Valid() = input.Valid();
    output.Timestamp() = input.Timestamp();
    output.Name().resize(input.JointName().size());
    std::copy(input.JointName().begin(), input.JointName().end(), output.Name().begin());
    output.Position() = input.JointPosition();
    output.Velocity() = input.JointVelocity();
    return true;
}

bool prmRobotStateToStateJointSetpoint(const prmRobotState& input, prmStateJoint& output)
{
    output.Valid() = input.Valid();
    output.Timestamp() = input.Timestamp();
    output.Name().resize(input.JointName().size());
    std::copy(input.JointName().begin(), input.JointName().end(), output.Name().begin());
    output.Position() = input.JointPositionGoal();
    output.Velocity() = input.JointVelocityGoal();
    return true;
}

bool prmRobotStateToCartesianPositionRxRyMeasured(const prmRobotState& input, prmPositionCartesianGet& output)
{
    if (input.CartesianPosition().size() != 5) {
        return false;
    }

    output.Valid() = true;
    output.Timestamp() = input.Timestamp();
    output.ReferenceFrame() = input.ReferenceFrame();
    output.MovingFrame() = input.MovingFrame();
    output.Position().translation() = input.CartesianPosition().head<3>();

    Eigen::AngleAxisd y_rot(input.CartesianPosition()(4), Eigen::Vector3d::UnitY());
    Eigen::AngleAxisd x_rot(input.CartesianPosition()(3), Eigen::Vector3d::UnitX());
    output.Position() = y_rot * x_rot;
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
    output.Position().translation() = input.CartesianPositionGoal().head<3>();

    Eigen::AngleAxisd y_rot(input.CartesianPositionGoal()(4), Eigen::Vector3d::UnitY());
    Eigen::AngleAxisd x_rot(input.CartesianPositionGoal()(3), Eigen::Vector3d::UnitX());
    output.Position() = y_rot * x_rot;
    return true;
}
