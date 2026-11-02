#include <cisstRobot/robJacobian.h>

Eigen::Vector<double, 6> robJacobian::cartesianVelocity(
    const Eigen::VectorXd& joint_velocity
) const
{
    return jacobian * joint_velocity;
}

void robJacobian::cartesianVelocity(
    const Eigen::VectorXd& joint_velocity, Eigen::Vector<double, 6>& output
) const
{
    output = jacobian * joint_velocity;
}

Eigen::VectorXd robJacobian::jointVelocity(
    const Eigen::Vector<double, 6>& cartesian_velocity
) const
{
    return decomposition.solve(cartesian_velocity);
}

void robJacobian::jointVelocity(
    const Eigen::Vector<double, 6>& cartesian_velocity, Eigen::VectorXd& output
) const
{
    output = decomposition.solve(cartesian_velocity);
}

Eigen::VectorXd robJacobian::jointForce(
    const Eigen::Vector<double, 6>& cartesian_force
) const
{
    return jacobian.transpose() * cartesian_force;
}

void robJacobian::jointForce(
    const Eigen::Vector<double, 6>& cartesian_force, Eigen::VectorXd& output
) const
{
    output = jacobian.transpose() * cartesian_force;
}

Eigen::Vector<double, 6> robJacobian::cartesianForce(
    const Eigen::VectorXd& joint_force
) const
{
    return decomposition.transpose().solve(joint_force);
}

void robJacobian::cartesianForce(
    const Eigen::VectorXd& joint_force, Eigen::Vector<double, 6>& output
) const
{
    output = decomposition.transpose().solve(joint_force);
}
