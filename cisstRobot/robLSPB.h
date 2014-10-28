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
 \brief Linear Segments with Parabolic Blends trajectory generator 

 \ingroup cisstRobot
*/
class CISST_EXPORT robLSPB {

 protected:
    bool mIsSet;        /*!< To ensure we don't evaluate if the parameters are not set */
    size_t mDimension;  /*!< Number of degrees of freedom */
    double mStartTime;  /*!< Start time, used to evaluate relative to 0 */
    double mDuration;   /*!< Max of all finish times */

    vctDoubleVec
        mStart,
        mFinish,
        mVelocity,
        mAcceleration,
        mAccelerationTime,
        mFinishTime;

 public:
    robLSPB(void);
    robLSPB(const vctDoubleVec & start,
            const vctDoubleVec & finish,
            const vctDoubleVec & velocity,
            const vctDoubleVec & acceleration,
            const double startTime = 0.0);

    ~robLSPB() {};

    /*!
     \brief Set start and stop time

     \param startTime start time
     \param stopTime  stop time
    */
    void Set(const vctDoubleVec & start,
             const vctDoubleVec & finish,
             const vctDoubleVec & velocity,
             const vctDoubleVec & acceleration,
             const double startTime = 0.0);

    void Evaluate(const double time,
                  vctDoubleVec & position);

    void Evaluate(const double time,
                  vctDoubleVec & position,
                  vctDoubleVec & velocity,
                  vctDoubleVec & acceleration);

    //! Return start time
    double & StartTime(void);

    //! Return duration
    double Duration(void) const;
};

#endif // _robLSPB_h
