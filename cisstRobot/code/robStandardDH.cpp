/*
  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2008-2024 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnConstants.h>
#include <cisstCommon/cmnJointType.h>
#include <cisstRobot/robStandardDH.h>

#include <Eigen/Geometry>
#include <iostream>
#include <string>

robStandardDH::robStandardDH(
    double alpha,
    double a,
    double theta,
    double d,
    cmnJointType joint_type,
    double joint_offset,
    bool active
)
    : robLinkKinematics(joint_type, joint_offset, active),
      alpha(alpha),
      a(a),
      theta(theta),
      d(d)
{
}

Eigen::Vector3d robStandardDH::JointAxis() const
{
    // X is common normal, so joint axis x component is zero
    // alpha describes YZ rotation of joint axis relative to previous link's
    // frame, but is measured clockwise about +X, so we negate it (or
    // equivalently negate just sin(alpha))
    return Eigen::Vector3d(0.0, 0.0, 1.0);
}

Eigen::Vector3d robStandardDH::JointOrigin() const
{
    // Joint origin is on common normal
    return Eigen::Vector3d(0.0, 0.0, 0.0);
}

Eigen::Isometry3d robStandardDH::LinkFrame() const
{
    const double theta = (joint_type == cmnJointType::CMN_JOINT_REVOLUTE)
                             ? this->theta + joint_offset
                             : this->theta;
    const double d = (joint_type == cmnJointType::CMN_JOINT_PRISMATIC)
                         ? this->d + joint_offset
                         : this->d;

    Eigen::Isometry3d T_alpha(Eigen::AngleAxisd(alpha, Eigen::Vector3d::UnitX())
    );
    Eigen::Isometry3d T_a(Eigen::Translation3d(a, 0.0, 0.0));
    Eigen::Isometry3d T_theta(Eigen::AngleAxisd(theta, Eigen::Vector3d::UnitZ())
    );
    Eigen::Isometry3d T_d(Eigen::Translation3d(0.0, 0.0, d));

    return T_theta * T_d * T_alpha * T_a;
}

#if CISST_HAS_JSON
std::optional<robStandardDH> robStandardDH::Read(const Json::Value& config)
{
    const double alpha = fixUpAngle(config.get("alpha", 0.0).asDouble());
    const double a = config.get("A", 0.0).asDouble();
    const double theta = fixUpAngle(config.get("theta", 0.0).asDouble());
    const double d = config.get("D", 0.0).asDouble();

    const double joint_offset = config.get("offset", 0.0).asDouble();

    std::string mode = config.get("mode", "active").asString();
    std::transform(mode.begin(), mode.end(), mode.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    if (mode != "active" && mode != "passive") {
        CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS << ": unknown joint mode. Got "
                           << mode << ", expected 'active' or 'passive'"
                           << std::endl;
        return {};
    }

    const bool active = mode == "active";

    const auto joint_type =
        jointTypeFromString(config.get("type", "").asString());
    if (!joint_type) {
        CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS << ": Expected a joint type. Got "
                           << config.get("type", "") << " instead" << std::endl;
        return {};
    }

    return robStandardDH(alpha, a, theta, d, *joint_type, joint_offset, active);
}
#endif
