#include <cisstCommon/cmnJointType.h>
#include <cisstRobot/kdl_utilities.h>
#include <cisstRobot/robModifiedDH.h>

#include <kdl/joint.hpp>

KDL::Vector toKDL(const Eigen::Vector3d& vector)
{
    return KDL::Vector(vector.x(), vector.y(), vector.z());
}

void toKDL(const Eigen::Vector3d& vector, KDL::Vector& output)
{
    output(0) = vector.x();
    output(1) = vector.y();
    output(2) = vector.z();
}

void toKDL(const Eigen::VectorXd& vector, KDL::JntArray& output)
{
    output.data = vector;
}

void fromKDL(const KDL::JntArray& vector, Eigen::VectorXd& output)
{
    output = vector.data;
}

Eigen::Ref<const Eigen::Vector3d> fromKDL(const KDL::Vector& vector)
{
    return Eigen::Map<const Eigen::Vector3d>(vector.data);
}

void fromKDL(const KDL::Vector& vector, Eigen::Vector3d& output)
{
    output = Eigen::Map<const Eigen::Vector3d>(vector.data);
}

KDL::Rotation toKDL(const Eigen::Matrix3d& rotation)
{
    return KDL::Rotation(
        toKDL(Eigen::Vector3d(rotation.col(0))),
        toKDL(Eigen::Vector3d(rotation.col(1))),
        toKDL(Eigen::Vector3d(rotation.col(2)))
    );
}

void toKDL(const Eigen::Matrix3d& rotation, KDL::Rotation& output)
{
    Eigen::Map<Eigen::Matrix<double, 3, 3, Eigen::RowMajor>>(output.data) =
        rotation;
}

Eigen::Ref<const Eigen::Matrix<double, 3, 3, Eigen::RowMajor>> fromKDL(
    const KDL::Rotation& rotation
)
{
    return Eigen::Map<const Eigen::Matrix<double, 3, 3, Eigen::RowMajor>>(
        rotation.data
    );
}

void fromKDL(const KDL::Rotation& rotation, Eigen::Matrix3d& output)
{
    output = Eigen::Map<const Eigen::Matrix<double, 3, 3, Eigen::RowMajor>>(
        rotation.data
    );
}

KDL::Frame toKDL(const Eigen::Isometry3d& frame)
{
    return KDL::Frame(
        toKDL(Eigen::Matrix3d(frame.linear())),
        toKDL(Eigen::Vector3d(frame.translation()))
    );
}

void toKDL(const Eigen::Isometry3d& frame, KDL::Frame& output)
{
    toKDL(frame.linear(), output.M);
    toKDL(frame.translation(), output.p);
}

KDL::RigidBodyInertia toKDL(const robLinkDynamics& dynamics)
{
    const Eigen::Matrix3d inertia_matrix = dynamics.MomentOfInertiaAtCOM();
    const KDL::RotationalInertia inertia(
        inertia_matrix(0, 0),
        inertia_matrix(1, 1),
        inertia_matrix(2, 2),
        inertia_matrix(1, 0),
        inertia_matrix(2, 0),
        inertia_matrix(2, 1)
    );

    return KDL::RigidBodyInertia(
        dynamics.Mass(), toKDL(dynamics.CenterOfMass()), inertia
    );
}

KDL::Joint toKDL(const robLinkKinematics& joint)
{
    if (!joint.isActive()) { return KDL::Joint(); }

    const auto type = joint.JointType() == cmnJointType::CMN_JOINT_REVOLUTE
                          ? KDL::Joint::JointType::RotAxis
                          : KDL::Joint::JointType::TransAxis;
    return KDL::Joint(
        toKDL(joint.JointOrigin()), toKDL(joint.JointAxis()), type
    );
}

KDL::Chain loadChain(
    const std::vector<
        std::pair<std::unique_ptr<robLinkKinematics>, robLinkDynamics>>& robot
)
{
    KDL::Chain chain;

    for (const auto& link : robot) {
        const KDL::Joint joint = toKDL(*link.first);
        KDL::Segment segment(
            joint, toKDL(link.first->LinkFrame()), toKDL(link.second)
        );

        chain.addSegment(segment);
    }

    return chain;
}
