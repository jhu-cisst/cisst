/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2014-10-27

  (C) Copyright 2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstRobot/robLSPB.h>

robLSPB::robLSPB(void):
    mIsSet(false)
{}

robLSPB::robLSPB(const vctDoubleVec & start,
                 const vctDoubleVec & finish,
                 const vctDoubleVec & velocity,
                 const vctDoubleVec & acceleration,
                 const double startTime)
{
    Set(start, finish, velocity, acceleration, startTime);
}

void robLSPB::Set(const vctDoubleVec & start,
                  const vctDoubleVec & finish,
                  const vctDoubleVec & velocity,
                  const vctDoubleVec & acceleration,
                  const double startTime)
{
    mIsSet = false;

    // sanity checks
    mDimension = start.size();
    if (finish.size() != mDimension) {
        cmnThrow("robLSPB: finish point doesn't match start point dimension");
    }
    if (velocity.size() != mDimension) {
        cmnThrow("robLSPB: velocity doesn't match start point dimension");
    }
    if (acceleration.size() != mDimension) {
        cmnThrow("robLSPB: acceleration doesn't match start point dimension");
    }
    if (!velocity.Greater(0.0)) {
        cmnThrow("robLSPB: velocity must be greater than zero");
    }
    if (!acceleration.Greater(0.0)) {
        cmnThrow("robLSPB: acceleration must be greater than zero");
    }
    // store information and resize data members
    mStartTime = startTime;
    mStart.ForceAssign(start);
    mFinish.ForceAssign(finish);
    mVelocity.ForceAssign(velocity);
    mAcceleration.ForceAssign(acceleration);
    mAccelerationTime.SetSize(mDimension);
    mFinishTime.SetSize(mDimension);

    // compute trajectory parameters
    for (size_t i = 0;
         i < mDimension;
         ++i) {
        // compute direction
        const double distance = finish[i] - start[i];
        if (distance < 0.0) {
            mVelocity[i] *= -1.0;;
            mAcceleration[i] *= -1.0;
        } else if (distance == 0) {
            mVelocity[i] = 0.0;
            mAcceleration[i] = 0.0;
        }
        // compute time to reach constant velocity

        // compute position at end of acceleration
        
    }
    mIsSet = true;
}

void robLSPB::Evaluate(const double absoluteTime,
                       vctDoubleVec & position)
{
    if (!mIsSet) {
        cmnThrow("robLSPB: trajectory parameters are not set yet");
    }
    const double time = absoluteTime - mStartTime;
    const double time2 = time * time;
    if (time <= 0) {
        position.ForceAssign(mStart);
    }
    for (size_t i = 0;
         i < mDimension;
         ++i) {
        if (time >= mFinishTime[i]) {
            position[i] = mFinish[i];
        } else {
            // acceleration phase
            if (time <= mAccelerationTime[i]) {
                position[i] = mStart[i]
                    + 0.5 * mAcceleration[i] * time2;
            } else {




            }
        }
    }
}

void robLSPB::Evaluate(const double absoluteTime,
                       vctDoubleVec & position,
                       vctDoubleVec & velocity,
                       vctDoubleVec & acceleration)
{
    if (!mIsSet) {
        cmnThrow("robLSPB: trajectory parameters are not set yet");
    }
    const double time = absoluteTime - mStartTime;
    const double time2 = time * time;
    if (time <= 0) {
        position.ForceAssign(mStart);
        velocity.Zeros();
        acceleration.Zeros();
    }
    for (size_t i = 0;
         i < mDimension;
         ++i) {
        if (time >= mFinishTime[i]) {
            position[i] = mFinish[i];
            velocity[i] = 0.0;
            acceleration[i] = 0.0;
        } else {
            // acceleration phase
            if (time <= mAccelerationTime[i]) {
                position[i] = mStart[i]
                    + 0.5 * mAcceleration[i] * time2;
                velocity[i] = mAcceleration[i] * time;
                acceleration[i] = mAcceleration[i];
            } else {



            }
        }
    }
}
                       
double & robLSPB::StartTime(void) {
    return mStartTime;
}

double robLSPB::Duration(void) const {
    return mTotalTime - mStartTime;
}
