/*
  Author(s): Simon Leonard
  Created on: Dec 17 2009

  (C) Copyright 2008-2024 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robLinkDynamics.h>
#include <json/json.h>

#include <optional>

robLinkDynamics::robLinkDynamics(
    double mass,
    const Eigen::Vector3d& center_of_mass,
    const Eigen::Matrix3d& moment_of_inertia
)
    : mass(mass),
      center_of_mass(center_of_mass),
      moment_of_inertia(moment_of_inertia) {}

#if CISST_HAS_JSON
std::optional<robLinkDynamics> robLinkDynamics::Read(
    const Json::Value& link_config
) {
    const double mass = link_config.get("mass", 0.0).asDouble();

    const Eigen::Vector3d center_os_mass(
        link_config.get("cx", 0.0).asDouble(),
        link_config.get("cy", 0.0).asDouble(),
        link_config.get("cz", 0.0).asDouble()
    );

    const Eigen::Vector3d principal_moments(
        link_config.get("Ixx", 0.0).asDouble(),
        link_config.get("Iyy", 0.0).asDouble(),
        link_config.get("Izz", 0.0).asDouble()
    );

    if ((principal_moments.array() < 0.0).any()) {
        CMN_LOG_INIT_ERROR
            << CMN_LOG_DETAILS
            << ": Principal moments of inertia must be non-negative"
            << std::endl;
        return {};
    }

    const Eigen::Vector3d principal_axis_x = Eigen::Vector3d(
        link_config.get("x1", 1.0).asDouble(),
        link_config.get("x2", 0.0).asDouble(),
        link_config.get("x3", 0.0).asDouble()
    );

    const Eigen::Vector3d principal_axis_y = Eigen::Vector3d(
        link_config.get("y1", 0.0).asDouble(),
        link_config.get("y2", 1.0).asDouble(),
        link_config.get("y3", 0.0).asDouble()
    );

    const Eigen::Vector3d principal_axis_z = Eigen::Vector3d(
        link_config.get("z1", 0.0).asDouble(),
        link_config.get("z2", 0.0).asDouble(),
        link_config.get("z3", 1.0).asDouble()
    );

    Eigen::Matrix3d principal_axes;
    principal_axes.col(0) = principal_axis_x.normalized();
    principal_axes.col(1) = principal_axis_y.normalized();
    principal_axes.col(2) = principal_axis_z.normalized();

    const Eigen::Matrix3d inertia = principal_axes *
                                    principal_moments.asDiagonal() *
                                    principal_axes.transpose();

    return robLinkDynamics(mass, center_os_mass, inertia);
}
#endif
