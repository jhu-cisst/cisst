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

#ifndef _robReflexxes_h
#define _robReflexxes_h

#include <Eigen/Core>
#include <memory>

// forward declarations for Reflexxes types
class ReflexxesAPI;
class RMLPositionInputParameters;
class RMLPositionOutputParameters;
class RMLPositionFlags;

// Always include last
#include <cisstRobot/robExport.h>

/*!
  \brief Trajectory generator based on Reflexxes Type II Library

  Details can be found in www.reflexxes.com.

  \ingroup cisstRobot
*/
class CISST_EXPORT robReflexxes
{
public:
    enum class SynchronizationType { NONE, TIME, PHASE };

    enum class ResultType { UNDEFINED, WORKING, FINAL_STATE_REACHED, ERROR };

protected:
    bool mIsSet; /*!< To ensure we don't evaluate if the parameters are not set
                  */
    Eigen::Index mDimension;              /*!< Number of degrees of freedom */
    SynchronizationType mSynchronization; /*!< synchronization type we can set*/

    std::unique_ptr<ReflexxesAPI> RML;
    std::unique_ptr<RMLPositionInputParameters> IP;
    std::unique_ptr<RMLPositionOutputParameters> OP;
    std::unique_ptr<RMLPositionFlags> mFlags;

    ResultType mResultValue;
    double mTime;
    Eigen::VectorXd mCurrentAcceleration;

public:
    robReflexxes(void);
    robReflexxes(
        const Eigen::VectorXd& MaxVelocity,
        const Eigen::VectorXd& MaxAcceleration,
        const double CycleTime,
        const SynchronizationType synchronization = SynchronizationType::NONE
    );
    robReflexxes(
        const Eigen::VectorXd& MaxVelocity,
        const Eigen::VectorXd& MaxAcceleration,
        const Eigen::VectorXd& MaxJerk,
        const double CycleTime,
        const SynchronizationType synchronization = SynchronizationType::NONE
    );
    ~robReflexxes();

    void Init(void);

    const ResultType& ResultValue(void) const;

    /*! Returns expected time left after last Evaluate call. */
    double Duration(void) const;

    const double& Time(void) const;
    void SetTime(const double& time);

    /*!  \brief Set target positions and velocities as well as desired
      maximum velocities and accelerations. All vectors must match the
      target position size.

      \param maximum velocities
      \param maximum acceleration
    */
    void Set(
        const Eigen::VectorXd& MaxVelocity,
        const Eigen::VectorXd& MaxAcceleration,
        const double CycleTime,
        const SynchronizationType synchronization = SynchronizationType::NONE
    );

    void Set(
        const Eigen::VectorXd& MaxVelocity,
        const Eigen::VectorXd& MaxAcceleration,
        const Eigen::VectorXd& MaxJerk,
        const double CycleTime,
        const SynchronizationType synchronization = SynchronizationType::NONE
    );

    /*
      \param current position
      \param current velocity
      \param target position
      \param target velocities
    */
    void Evaluate(
        Eigen::VectorXd& CurrentPosition,
        Eigen::VectorXd& CurrentVelocity,
        const Eigen::VectorXd& TargetPosition,
        const Eigen::VectorXd& TargetVelocity
    );
};

#endif