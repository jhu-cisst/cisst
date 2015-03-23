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
                 const double startTime,
                 const CoordinationType coordination)
{
    Set(start, finish, velocity, acceleration, startTime, coordination);
}

void robLSPB::Set(const vctDoubleVec & start,
                  const vctDoubleVec & finish,
                  const vctDoubleVec & velocity,
                  const vctDoubleVec & acceleration,
                  const double startTime,
                  const CoordinationType coordination)
{
    mIsSet = false;

    // sanity checks
    mDimension = start.size();
    if (finish.size() != mDimension) {
        cmnThrow("robLSPB::Set: finish point doesn't match start point dimension");
    }
    if (velocity.size() != mDimension) {
        cmnThrow("robLSPB::Set: velocity doesn't match start point dimension");
    }
    if (acceleration.size() != mDimension) {
        cmnThrow("robLSPB::Set: acceleration doesn't match start point dimension");
    }
    // store information and resize data members
    mCoordination = coordination;
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

        // compute time if distance != 0
        if (distance != 0) {
            if (mVelocity[i] == 0.0) {
                cmnThrow("robLSPB::Set: velocity must be greater than zero");
            }
            if (mAcceleration[i] == 0.0) {
                cmnThrow("robLSPB::Set: acceleration must be greater than zero");
            }
            mAccelerationTime[i] = mVelocity[i] / mAcceleration[i];
            // check distance over max accel and max decel at end of
            // acceleration phase to see if we're past mid-point.
            const double accelDistance = mAcceleration[i]
                * mAccelerationTime[i] * mAccelerationTime[i];
            if (fabs(accelDistance) >= fabs(distance)) {
                mFinishTime[i] = 2.0
                    * sqrt(distance / mAcceleration[i]);
                mAccelerationTime[i] = mFinishTime[i] * 0.5;
            } else {
                mFinishTime[i] = distance / mVelocity[i] + mAccelerationTime[i];
            }
        } else {
            mAccelerationTime[i] = 0.0;
            mFinishTime[i] = 0.0;
        }
    }
    // compute max time
    mDuration = mFinishTime.MaxElement();

    // scale time to all arrive at same time
    if (mCoordination == LSPB_DURATION) {
        mTimeScale.SetSize(mDimension);
        if (mDuration > 0) {
            mTimeScale.RatioOf(mFinishTime, mDuration);
        } else {
            mTimeScale.SetAll(1.0);
        }
    }
    mIsSet = true;
}

void robLSPB::Evaluate(const double absoluteTime,
                       vctDoubleVec & position,
                       vctDoubleVec & velocity,
                       vctDoubleVec & acceleration)
{
    // sanity checks
    if (!mIsSet) {
        cmnThrow("robLSPB::Evaluate trajectory parameters are not set yet");
    }
    if (position.size() != mDimension) {
        cmnThrow("robLSPB::Evaluate: position doesn't match dimension");
    }
    if (velocity.size() != mDimension) {
        cmnThrow("robLSPB::Evaluate: velocity doesn't match dimension");
    }
    if (acceleration.size() != mDimension) {
        cmnThrow("robLSPB::Evaluate: acceleration doesn't match dimension");
    }

    const double time = absoluteTime - mStartTime;
    if (time <= 0) {
        position.ForceAssign(mStart);
        velocity.Zeros();
        acceleration.Zeros();
        return;
    }
    for (size_t i = 0;
         i < mDimension;
         ++i) {
        double dimTime;
        if (mCoordination == LSPB_DURATION) {
            dimTime = time * mTimeScale[i];
        } else {
            dimTime = time;
        }
        const double time2 = dimTime * dimTime;

        if (dimTime >= mFinishTime[i]) {
            position[i] = mFinish[i];
            velocity[i] = 0.0;
            acceleration[i] = 0.0;
        } else {
            // acceleration phase
            if (dimTime <= mAccelerationTime[i]) {
                position[i] =
                    mStart[i]
                    + 0.5 * mAcceleration[i] * time2;
                velocity[i] = mAcceleration[i] * dimTime;
                acceleration[i] = mAcceleration[i];
            } else if (dimTime >= (mFinishTime[i] - mAccelerationTime[i])) {
                // deceleration phase
                position[i] =
                    mFinish[i]
                    - 0.5 * mAcceleration[i] * mFinishTime[i] * mFinishTime[i]
                    + mAcceleration[i] * mFinishTime[i] * dimTime
                    - 0.5 * mAcceleration[i] * time2;
                velocity[i] =
                    mAcceleration[i] * mFinishTime[i]
                    - mAcceleration[i] * dimTime;
                acceleration[i] = -mAcceleration[i];
            } else {
                // constant velocity
                position[i] =
                    0.5 * (mFinish[i] + mStart[i] - mVelocity[i] * mFinishTime[i])
                    + mVelocity[i] * dimTime;
                velocity[i] = mVelocity[i];
                acceleration[i] = 0.0;
            }
        }
    }
}

void robLSPB::Evaluate(const double absoluteTime,
                       vctDoubleVec & position)
{
    mTemp.SetSize(mDimension);
    Evaluate(absoluteTime, position, mTemp, mTemp);
}

double & robLSPB::StartTime(void) {
    return mStartTime;
}

double robLSPB::Duration(void) const {
    return mDuration;
}
