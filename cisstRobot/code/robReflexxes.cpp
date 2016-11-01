/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

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

#include <cisstRobot/robReflexxes.h>

#include <ReflexxesAPI.h>
#include <RMLPositionFlags.h>
#include <RMLPositionInputParameters.h>
#include <RMLPositionOutputParameters.h>

robReflexxes::robReflexxes(void)
{
    Init();
}

robReflexxes::robReflexxes(const vctDoubleVec & MaxVelocity,
                           const vctDoubleVec & MaxAcceleration,
                           const double CycleTime,
                           const SynchronizationType synchronization)
{
    Init();
    Set(MaxVelocity, MaxAcceleration, CycleTime, synchronization);
}

robReflexxes::robReflexxes(const vctDoubleVec & MaxVelocity,
                           const vctDoubleVec & MaxAcceleration,
                           const vctDoubleVec & MaxJerk,
                           const double CycleTime,
                           const SynchronizationType synchronization)
{
    Init();
    Set(MaxVelocity, MaxAcceleration, MaxJerk, CycleTime, synchronization);
}

robReflexxes::~robReflexxes()
{
    if (RML != 0) delete RML;
    if (IP != 0) delete IP;
    if (OP != 0) delete OP;
    if (mFlags != 0) delete mFlags;
}

void robReflexxes::Init(void)
{
    mIsSet = false;
    mResultValue = Reflexxes_UNDEFINDED;
    mTime = 0.0;
    RML = 0;
    IP = 0;
    OP = 0;
    mFlags = 0;
}

const double & robReflexxes::Time(void) const
{
    return mTime;
}

void robReflexxes::SetTime(const double & time)
{
    mTime = time;
}

const robReflexxes::ResultType & robReflexxes::ResultValue(void) const
{
    return mResultValue;
}

double robReflexxes::Duration(void) const
{
    return OP->GetGreatestExecutionTime();
}

void robReflexxes::Set(const vctDoubleVec & MaxVelocity,
                       const vctDoubleVec & MaxAcceleration,
                       const double CycleTime,
                       const SynchronizationType synchronization)
{
    // create a jerk vector and then call method Set with jerk
    vctDoubleVec jerk(MaxVelocity.size());
    jerk.SetAll(500.0);
    Set(MaxVelocity, MaxAcceleration, jerk, CycleTime, synchronization);
}

void robReflexxes::Set(const vctDoubleVec & MaxVelocity,
                       const vctDoubleVec & MaxAcceleration,
                       const vctDoubleVec & MaxJerk,
                       const double CycleTime,
                       const SynchronizationType synchronization)
{
    mIsSet = false;

    // sanity checks
    mDimension = MaxVelocity.size();
    if (MaxAcceleration.size() != mDimension) {
        cmnThrow("robReflexxes::Set: maximum acceleration doesn't match start point dimension");
    }
    if (MaxJerk.size() != mDimension) {
        cmnThrow("robReflexxes::Set: maximum jerk doesn't match start point dimension");
    }
    mCurrentAcceleration.SetSize(mDimension);
    for (size_t i = 0;
         i < mDimension;
         ++i)
    {
        mCurrentAcceleration[i] = 0;
    }

    // Creating all relevant objects of the Reflexxes Motion Library
    if (RML != 0) delete RML;
    if (IP != 0) delete IP;
    if (OP != 0) delete OP;
    if (mFlags != 0) delete mFlags;
    RML = new ReflexxesAPI(mDimension, CycleTime);
    IP = new RMLPositionInputParameters(mDimension);
    OP = new RMLPositionOutputParameters(mDimension);
    mFlags = new RMLPositionFlags();

    // reset RML state
    mResultValue = Reflexxes_UNDEFINDED;

    // Set-up the input parameters
    for (size_t i = 0;
         i < mDimension;
         ++i) {
        IP->MaxVelocityVector->VecData[i] = MaxVelocity[i];
        IP->MaxAccelerationVector->VecData[i] = MaxAcceleration[i];
        IP->MaxJerkVector->VecData[i] = MaxJerk[i];
        IP->SelectionVector->VecData[i] = true;
    }

    // Checking for input parameters
    if (!IP->CheckForValidity()) {
        cmnThrow("robReflexxes::Set: input values are INVALID!");
    }

    mIsSet = true;
}

void robReflexxes::Evaluate(vctDoubleVec & CurrentPosition,
                            vctDoubleVec & CurrentVelocity,
                            const vctDoubleVec & TargetPosition,
                            const vctDoubleVec & TargetVelocity)
{
    // sanity checks
    if (!mIsSet) {
        cmnThrow("robReflexxes::Evaluate: trajectory parameters are not set yet");
    }
    if (CurrentPosition.size() != mDimension) {
        cmnThrow("robReflexxes::Evaluate: position doesn't match dimension");
    }
    if (CurrentVelocity.size() != mDimension) {
        cmnThrow("robReflexxes::Evaluate: velocity doesn't match dimension");
    }
    if (TargetPosition.size() != mDimension) {
        cmnThrow("robReflexxes::Evaluate: target position doesn't match dimension");
    }
    if (TargetVelocity.size() != mDimension) {
        cmnThrow("robReflexxes::Evaluate: target velocity doesn't match dimension");
    }

    // Set-up the input parameters
    for (size_t i = 0;
         i < mDimension;
         ++i) {
        IP->CurrentPositionVector->VecData[i] = CurrentPosition[i];
        IP->CurrentVelocityVector->VecData[i] = CurrentVelocity[i];
        IP->CurrentAccelerationVector->VecData[i] = mCurrentAcceleration[i];
        IP->TargetPositionVector->VecData[i] = TargetPosition[i];
        IP->TargetVelocityVector->VecData[i] = TargetVelocity[i];
    }

    // Calling the Reflexxes OTG algorithm
    int rmlResult = RML->RMLPosition(*IP, OP, *mFlags);
    if (rmlResult < 0) {
        printf("An error occurred (%d).\n", rmlResult);
    }

    switch (rmlResult) {
    case ReflexxesAPI::RML_WORKING:
        mResultValue = Reflexxes_WORKING;
        break;
    case ReflexxesAPI::RML_FINAL_STATE_REACHED:
        mResultValue = Reflexxes_FINAL_STATE_REACHED;
        break;
    default:
        mResultValue = Reflexxes_ERROR;
        break;
    }

    // Feed the output values of the current control cycle back to input values of the next control cycle
    for (size_t i = 0;
         i < mDimension;
         ++i) {
        CurrentPosition[i] = OP->NewPositionVector->VecData[i];
        CurrentVelocity[i] = OP->NewVelocityVector->VecData[i];
        mCurrentAcceleration[i] = OP->NewAccelerationVector->VecData[i];
    }
}
