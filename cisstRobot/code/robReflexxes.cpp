/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab:
 */

/*
  Author(s):  Anton Deguet, Zerui Wang
  Created on: 2016-02-22

  (C) Copyright 2016 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#include <RMLPositionFlags.h>
#include <RMLPositionInputParameters.h>
#include <RMLPositionOutputParameters.h>
#include <ReflexxesAPI.h>
#include <cisstCommon/cmnThrow.h>
#include <cisstRobot/robReflexxes.h>

robReflexxes::robReflexxes(void) { Init(); }

robReflexxes::robReflexxes(
    const Eigen::VectorXd& MaxVelocity,
    const Eigen::VectorXd& MaxAcceleration,
    const double CycleTime,
    const SynchronizationType synchronization
)
{
    Init();
    Set(MaxVelocity, MaxAcceleration, CycleTime, synchronization);
}

robReflexxes::robReflexxes(
    const Eigen::VectorXd& MaxVelocity,
    const Eigen::VectorXd& MaxAcceleration,
    const Eigen::VectorXd& MaxJerk,
    const double CycleTime,
    const SynchronizationType synchronization
)
{
    Init();
    Set(MaxVelocity, MaxAcceleration, MaxJerk, CycleTime, synchronization);
}

// Defined once ReflexxesAPI definition is available
robReflexxes::~robReflexxes() = default;

void robReflexxes::Init(void)
{
    mIsSet = false;
    mResultValue = ResultType::UNDEFINED;
    mTime = 0.0;
    RML = nullptr;
    IP = nullptr;
    OP = nullptr;
    mFlags = nullptr;
}

const double& robReflexxes::Time(void) const { return mTime; }

void robReflexxes::SetTime(const double& time) { mTime = time; }

const robReflexxes::ResultType& robReflexxes::ResultValue(void) const
{
    return mResultValue;
}

double robReflexxes::Duration(void) const
{
    return OP->GetGreatestExecutionTime();
}

void robReflexxes::Set(
    const Eigen::VectorXd& MaxVelocity,
    const Eigen::VectorXd& MaxAcceleration,
    const double CycleTime,
    const SynchronizationType synchronization
)
{
    // create a jerk vector and then call method Set with jerk
    Eigen::VectorXd jerk = 500.0 * Eigen::VectorXd::Ones(MaxVelocity.size());
    Set(MaxVelocity, MaxAcceleration, jerk, CycleTime, synchronization);
}

void robReflexxes::Set(
    const Eigen::VectorXd& MaxVelocity,
    const Eigen::VectorXd& MaxAcceleration,
    const Eigen::VectorXd& MaxJerk,
    const double CycleTime,
    const SynchronizationType CMN_UNUSED(synchronization)
)
{
    mIsSet = false;

    // sanity checks
    mDimension = MaxVelocity.size();
    if (MaxAcceleration.size() != mDimension) {
        cmnThrow(
            "robReflexxes::Set: maximum acceleration doesn't match start point "
            "dimension"
        );
    }
    if (MaxJerk.size() != mDimension) {
        cmnThrow(
            "robReflexxes::Set: maximum jerk doesn't match start point "
            "dimension"
        );
    }
    mCurrentAcceleration = Eigen::VectorXd::Zero(mDimension);

    // Creating all relevant objects of the Reflexxes Motion Library
    RML = std::make_unique<ReflexxesAPI>(mDimension, CycleTime);
    IP = std::make_unique<RMLPositionInputParameters>(mDimension);
    OP = std::make_unique<RMLPositionOutputParameters>(mDimension);
    mFlags = std::make_unique<RMLPositionFlags>();

    // reset RML state
    mResultValue = ResultType::UNDEFINED;

    // Set-up the input parameters
    Eigen::Map<Eigen::VectorXd>(IP->MaxVelocityVector->VecData, mDimension) = MaxVelocity;
    Eigen::Map<Eigen::VectorXd>(IP->MaxAccelerationVector->VecData, mDimension) = MaxAcceleration;
    Eigen::Map<Eigen::VectorXd>(IP->MaxJerkVector->VecData, mDimension) = MaxJerk;
    Eigen::Map<Eigen::VectorX<bool>>(IP->SelectionVector->VecData, mDimension).fill(true);

    // Checking for input parameters
    if (!IP->CheckForValidity()) {
        cmnThrow("robReflexxes::Set: input values are INVALID!");
    }

    mIsSet = true;
}

void robReflexxes::Evaluate(
    Eigen::VectorXd& CurrentPosition,
    Eigen::VectorXd& CurrentVelocity,
    const Eigen::VectorXd& TargetPosition,
    const Eigen::VectorXd& TargetVelocity
)
{
    // sanity checks
    if (!mIsSet) {
        cmnThrow("robReflexxes::Evaluate: trajectory parameters are not set yet"
        );
    }
    if (CurrentPosition.size() != mDimension) {
        cmnThrow("robReflexxes::Evaluate: position doesn't match dimension");
    }
    if (CurrentVelocity.size() != mDimension) {
        cmnThrow("robReflexxes::Evaluate: velocity doesn't match dimension");
    }
    if (TargetPosition.size() != mDimension) {
        cmnThrow(
            "robReflexxes::Evaluate: target position doesn't match dimension"
        );
    }
    if (TargetVelocity.size() != mDimension) {
        cmnThrow(
            "robReflexxes::Evaluate: target velocity doesn't match dimension"
        );
    }

    // Set-up the input parameters
    Eigen::Map<Eigen::VectorXd>(IP->CurrentPositionVector->VecData, mDimension) = CurrentPosition;
    Eigen::Map<Eigen::VectorXd>(IP->CurrentVelocityVector->VecData, mDimension) = CurrentVelocity;
    Eigen::Map<Eigen::VectorXd>(IP->CurrentAccelerationVector->VecData, mDimension) = mCurrentAcceleration;
    Eigen::Map<Eigen::VectorXd>(IP->TargetPositionVector->VecData, mDimension) = TargetPosition;
    Eigen::Map<Eigen::VectorXd>(IP->TargetVelocityVector->VecData, mDimension) = TargetVelocity;

    // Calling the Reflexxes OTG algorithm
    int rmlResult = RML->RMLPosition(*IP, OP.get(), *mFlags);
    if (rmlResult < 0) { printf("An error occurred (%d).\n", rmlResult); }

    switch (rmlResult) {
        case ReflexxesAPI::RML_WORKING:
            mResultValue = ResultType::WORKING;
            break;
        case ReflexxesAPI::RML_FINAL_STATE_REACHED:
            mResultValue = ResultType::FINAL_STATE_REACHED;
            break;
        default:
            mResultValue = ResultType::ERROR;
            break;
    }

    // Feed the output values of the current control cycle back to input values
    // of the next control cycle
    CurrentPosition = Eigen::Map<Eigen::VectorXd>(OP->NewPositionVector->VecData, mDimension);
    CurrentVelocity = Eigen::Map<Eigen::VectorXd>(OP->NewVelocityVector->VecData, mDimension);
    mCurrentAcceleration = Eigen::Map<Eigen::VectorXd>(OP->NewAccelerationVector->VecData, mDimension);
}
