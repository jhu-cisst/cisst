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

#ifndef CISST_ROBOT_ROBLINKDYNAMICS_H
#define CISST_ROBOT_ROBLINKDYNAMICS_H

#include <Eigen/Core>

#if CISST_HAS_JSON
#include <json/json.h>
#include <optional>
#endif

#include <cisstRobot/robExport.h>

//! Dynamics model of a single rigid link
class CISST_EXPORT robLinkDynamics {
protected:
    double mass;

    // center of mass with respect to link frame
    Eigen::Vector3d center_of_mass;

    // body inertia defined in frame aligned with link frame but at center of
    // mass
    Eigen::Matrix3d moment_of_inertia;

public:
    robLinkDynamics(
        double mass,
        const Eigen::Vector3d& center_of_mass,
        const Eigen::Matrix3d& moment_of_inertia
    );

    //! Read the DH and body parameters
#if CISST_HAS_JSON
    static std::optional<robLinkDynamics> Read(const Json::Value& linkConfig);
#endif

    double Mass() const { return mass; }

    Eigen::Vector3d CenterOfMass() const { return center_of_mass; }

    Eigen::Matrix3d MomentOfInertiaAtCOM() const { return moment_of_inertia; }
};

#endif
