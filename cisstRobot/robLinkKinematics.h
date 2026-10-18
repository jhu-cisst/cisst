/*

  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef CISST_ROBOT_ROBLINKKINEMATICS_H
#define CISST_ROBOT_ROBLINKKINEMATICS_H

#include <cisstCommon/cmnJointType.h>
#include <cisstRobot/robExport.h>

#include <Eigen/Geometry>
#include <optional>

class CISST_EXPORT robLinkKinematics {
protected:
    // Revolute or prismatic
    cmnJointType joint_type;

    bool active; // active or passive/fixed

    double joint_offset;

    // For loading from JSON - if user has rounded common angles, we use full-precision instead
    static double fixUpAngle(double angle);

public:
    robLinkKinematics(
        cmnJointType joint_type,
        double joint_offset,
        bool active=true
    );
    virtual ~robLinkKinematics() = default;

    // Joint axis with respect to base frame (previous link)
    virtual Eigen::Vector3d JointAxis() const = 0;

    // Joint origin with respect to base frame
    virtual Eigen::Vector3d JointOrigin() const = 0;

    // Link frame with respect to previous link when joint position is zero
    virtual Eigen::Isometry3d LinkFrame() const = 0;

    cmnJointType JointType() const { return joint_type; }

    // Whether joint is active or passive/fixed
    bool isActive() const { return active; }

    double JointOffset() const { return joint_offset; }

    static std::optional<cmnJointType> jointTypeFromString(const std::string& name);
};

#endif
