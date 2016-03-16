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
}

void robReflexxes::Init(void)
{
    mIsSet = false;
    mResultValue = 0;
    mTime = 0.0;
    RML = 0;
    IP = 0;
    OP = 0;
}

double robReflexxes::getTime(void) const
{
    return mTime;
}

void robReflexxes::setTime(const double time)
{
    mTime = time;
}

int robReflexxes::getResultValue(void) const
{
    return mResultValue;
}

void robReflexxes::Set(const vctDoubleVec & MaxVelocity,
                       const vctDoubleVec & MaxAcceleration,
                       const double CycleTime,
                       const SynchronizationType synchronization)
{
    mIsSet = false;

    // sanity checks
    mDimension = MaxVelocity.size();
    if (MaxAcceleration.size() != mDimension) {
        cmnThrow("robReflexxes::Set: maximum acceleration doesn't match start point dimension");
    }
    mCurrentAcceleration.SetSize(mDimension);
    for(size_t i = 0;
        i < mDimension;
        ++i)
    {
        mCurrentAcceleration[i] = 0;
    }

    // Creating all relevant objects of the Reflexxes Motion Library
    if (RML != 0) delete RML;
    if (IP != 0) delete IP;
    if (OP != 0) delete OP;
    RML = new ReflexxesAPI(mDimension, CycleTime);
    IP = new RMLPositionInputParameters(mDimension);
    OP = new RMLPositionOutputParameters(mDimension);

    // Set-up the input parameters
    for (size_t i = 0;
         i < mDimension;
         ++i) {
        IP->MaxVelocityVector->VecData[i] = MaxVelocity[i];
        IP->MaxAccelerationVector->VecData[i] = MaxAcceleration[i];
        IP->MaxJerkVector->VecData[i] = 500.0;
        IP->SelectionVector->VecData[i] = true;
    }

    // Checking for input parameters
    if (IP->CheckForValidity()) {
        std::cout << "robReflexxes::Set: input values are valid!" << std::endl;
        //cmnThrow("robReflexxes::Set: input values are valid!");
    } else {
        cmnThrow("robReflexxes::Set: input values are INVALID!");
    }
    
    mIsSet = true;
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
    for(size_t i = 0;
        i < mDimension;
        ++i)
    {
        mCurrentAcceleration[i] = 0;
    }

    // Creating all relevant objects of the Reflexxes Motion Library
    if (RML != 0) delete RML;
    if (IP != 0) delete IP;
    if (OP != 0) delete OP;
    RML = new ReflexxesAPI(mDimension, CycleTime);
    IP = new RMLPositionInputParameters(mDimension);
    OP = new RMLPositionOutputParameters(mDimension);

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
    if (IP->CheckForValidity()) {
        std::cout << "robReflexxes::Set: input values are valid!" << std::endl;
        //cmnThrow("robReflexxes::Set: input values are valid!");
    } else {
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
    mResultValue = RML->RMLPosition(*IP, OP, Flags);

    if (mResultValue < 0) {
        printf("An error occurred (%d).\n", mResultValue);
        //break;
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
