#ifndef CISST_ROBOT_KDL_H
#define CISST_ROBOT_KDL_H

#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robLinkDynamics.h>
#include <cisstRobot/robLinkKinematics.h>
#include <cisstRobot/robModifiedDH.h>
#include <json/json.h>

#include <Eigen/Geometry>
#include <kdl/chain.hpp>
#include <kdl/chainfksolverpos_recursive.hpp>
#include <kdl/chainiksolverpos_nr_jl.hpp>
#include <kdl/chainiksolvervel_wdls.hpp>
#include <kdl/jntarray.hpp>
#include <kdl/joint.hpp>
#include <kdl/rigidbodyinertia.hpp>
#include <kdl/rotationalinertia.hpp>
#include <kdl/segment.hpp>

// Simple vector conversion
KDL::Vector toKDL(const Eigen::Vector3d& vector);

// Non-allocating/in-place vector conversion
void toKDL(const Eigen::Vector3d& vector, KDL::Vector& output);

// Non-allocating/in-place vector to JntArray conversion
void toKDL(const Eigen::VectorXd& vector, KDL::JntArray& output);

void fromKDL(const KDL::JntArray& vector, Eigen::VectorXd& output);

// Non-allocating vector conversion - note that the Eigen::Ref is only valid
// while the KDL::Vector is still in scope/alive
Eigen::Ref<const Eigen::Vector3d> fromKDL(const KDL::Vector& vector);

// Non-allocating in-place vector conversion
void fromKDL(const KDL::Vector& vector, Eigen::Vector3d& output);

// Simple rotation conversion
KDL::Rotation toKDL(const Eigen::Matrix3d& rotation);

// Non-allocating in-place rotation conversion
void toKDL(const Eigen::Matrix3d& rotation, KDL::Rotation& output);

// Non-allocating rotation conversion - note that the Eigen::Ref is only valid
// while the KDL::Rotation is still in scope/alive
Eigen::Ref<const Eigen::Matrix<double, 3, 3, Eigen::RowMajor>> fromKDL(
    const KDL::Rotation& rotation
);

// Non-allocating in-place rotation conversion
void fromKDL(const KDL::Rotation& rotation, Eigen::Matrix3d& output);

// Simple frame conversion
KDL::Frame toKDL(const Eigen::Isometry3d& frame);

// Non-allocating in-place frame conversion
void toKDL(const Eigen::Isometry3d& frame, KDL::Frame& output);

KDL::RigidBodyInertia toKDL(const robLinkDynamics& dynamics);

//! Create KDL Chain from the kinematics and the dynamtics of the robot
KDL::Chain loadChain(
    const std::vector<
        std::pair<std::unique_ptr<robLinkKinematics>, robLinkDynamics>>& robot
);

#endif
