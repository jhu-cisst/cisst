#include <cisstCommon/cmnConstants.h>
#include <cisstRobot/robLinkKinematics.h>

robLinkKinematics::robLinkKinematics(
    cmnJointType joint_type, double joint_offset, bool active
)
    : joint_type(joint_type), active(active), joint_offset(joint_offset)
{
}

std::optional<cmnJointType> robLinkKinematics::jointTypeFromString(
    const std::string& type_name
)
{
    std::string name;
    std::transform(
        type_name.begin(),
        type_name.end(),
        std::back_inserter(name),
        [](unsigned char c) { return std::toupper(c); }
    );

    // match to string to a joint type
    cmnJointType joint_type;
    if (name == "REVOLUTE" || name == "HINGE") {
        joint_type = cmnJointType::CMN_JOINT_REVOLUTE;
    } else if (name == "PRISMATIC" || name == "SLIDER") {
        joint_type = cmnJointType::CMN_JOINT_PRISMATIC;
    } else if (name == "UNIVERSAL") {
        joint_type = cmnJointType::CMN_JOINT_UNIVERSAL;
    } else if (name == "BALLSOCKET") {
        joint_type = cmnJointType::CMN_JOINT_BALL_SOCKET;
    } else {
        return {};
    }

    return joint_type;
}

double robLinkKinematics::fixUpAngle(double angle)
{
    if (std::abs(angle) == 1.5708) {
        return std::copysign(cmnPI_2, angle);
    } else if (std::abs(angle) == 3.1415) {
        return std::copysign(cmnPI, angle);
    } else if (std::abs(angle) == 0.7854) {
        return std::copysign(cmnPI_4, angle);
    } else {
        return angle;
    }
}
