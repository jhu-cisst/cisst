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

#ifndef _robLSPB_h
#define _robLSPB_h

#include <cisstVector/vctDynamicVectorTypes.h>


// Always include last
#include <cisstRobot/robExport.h>

/*!
 \brief Linear Segments with Parabolic Blends trajectory generator.

 This trajectory generator is based on LSPB as described in "Robotics:
 Basic Analysis and Design" by William A. Wolovich.  The trajectory
 generated corresponds to maximum acceleration until maximum velocity
 is reached followed by maximum deceleration until final position is
 reached.  Desired velocities and accelerations are defined as
 strictly positive numbers.

 It can operate on multiple joints and the user must choose how to
 coordinate the motion.  Current options to coordinate the joint
 trajectories are LSPB_NONE and LSPB_DURATION.  With LSPB_NONE, each
 joint moves as fast as possible.  With LSPB_DURATION, the
 trajectories are scaled so they start and end at the same time based
 on the slowest joint.  It's important to note that acceleration,
 constant velocities and decelaration phases are not coordinated.

 If the user attempt to evaluate a point before the start time, the
 generator returns the start point.  For any time after the end time,
 the generator returns the end point.

 \ingroup cisstRobot
*/
class CISST_EXPORT robLSPB {

 public:
    typedef enum {LSPB_NONE, LSPB_DURATION} CoordinationType;

 protected:
    bool mIsSet;        /*!< To ensure we don't evaluate if the parameters are not set */
    size_t mDimension;  /*!< Number of degrees of freedom */
    double mStartTime;  /*!< Start time, used to evaluate relative to 0 */
    double mDuration;   /*!< Max of all finish times */

    CoordinationType mCoordination;

    vctDoubleVec
        mStart,
        mFinish,
        mVelocity,
        mAcceleration,
        mAccelerationTime,
        mFinishTime,
        mTimeScale,
        mTemp;

 public:
    robLSPB(void);
    robLSPB(const vctDoubleVec & start,
            const vctDoubleVec & finish,
            const vctDoubleVec & velocity,
            const vctDoubleVec & acceleration,
            const double startTime = 0.0,
            const CoordinationType coordination = LSPB_NONE);

    ~robLSPB() {};

    /*!  \brief Set start and finish position as well as desired
      maximum velocities and accelerations.  Start time is provided as
      a convenience so that future calls to Evaluate can use absolute
      time.  All vectors must match the start position size.

     \param start Start position
     \param finish Finish position
     \param velocity Desired maximum velocity
     \accelerations Acceleration (and deceleration) used to reach maximum velocity
    */
    void Set(const vctDoubleVec & start,
             const vctDoubleVec & finish,
             const vctDoubleVec & velocity,
             const vctDoubleVec & acceleration,
             const double startTime = 0.0,
             const CoordinationType coordination = LSPB_NONE);

    void Evaluate(const double time,
                  vctDoubleVec & position,
                  vctDoubleVec & velocity,
                  vctDoubleVec & acceleration);

    void Evaluate(const double time,
                  vctDoubleVec & position);

    //! Return start time
    double & StartTime(void);

    //! Return duration
    double Duration(void) const;
};

#endif // _robLSPB_h
