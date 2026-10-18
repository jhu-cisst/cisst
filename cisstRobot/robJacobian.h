/*

  Author(s): Brendan Burkhart
  Created on: 2026-10-03

  (C) Copyright 2026 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef CISST_ROBOT_ROBJACOBIAN_H
#define CISST_ROBOT_ROBJACOBIAN_H

#include <cisstCommon/cmnJointType.h>
#include <cisstRobot/robExport.h>

#include <Eigen/QR>

class CISST_EXPORT robJacobian
{
public:
    using jacobian_t = Eigen::Matrix<double, 6, Eigen::Dynamic>;

private:

    jacobian_t jacobian;
    Eigen::ColPivHouseholderQR<jacobian_t> decomposition;

public:
    template <typename Derived>
    void update(const Eigen::DenseBase<Derived>& jacobian)
    {
        this->jacobian = jacobian;
        decomposition.compute(jacobian);
    }

    const jacobian_t& matrix() const {
        return jacobian;
    }

    // Compute Cartesian twist given joint velocity
    Eigen::Vector<double, 6> cartesianVelocity(
        const Eigen::VectorXd& joint_velocity
    ) const;

    // Compute Cartesian twist in-place, given joint velocity
    void cartesianVelocity(
        const Eigen::VectorXd& joint_velocity, Eigen::Vector<double, 6>& output
    ) const;

    // Compute joint velocity given Cartesian twist
    Eigen::VectorXd jointVelocity(
        const Eigen::Vector<double, 6>& cartesian_velocity
    ) const;

    // Compute joint velocity in-place given Cartesian twist
    void jointVelocity(
        const Eigen::Vector<double, 6>& cartesian_velocity,
        Eigen::VectorXd& output
    ) const;

    // Compute joint force/efforts given Cartesian wrench
    Eigen::VectorXd jointForce(const Eigen::Vector<double, 6>& cartesian_force
    ) const;

    // Compute joint force/efforts in-place given Cartesian wrench
    void jointForce(
        const Eigen::Vector<double, 6>& cartesian_force, Eigen::VectorXd& output
    ) const;

    // Compute Cartesian wrench, given joint force/efforts
    Eigen::Vector<double, 6> cartesianForce(const Eigen::VectorXd& joint_force
    ) const;

    // Compute Cartesian wrench in-place, given joint force/efforts
    void cartesianForce(
        const Eigen::VectorXd& joint_force, Eigen::Vector<double, 6>& output
    ) const;
};

#endif
