#
# (C) Copyright 2005-2016 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# All cisstRobot libraries
project (cisstRobotLibs)

add_subdirectory (code)

target_link_libraries(cisstRobot Eigen3::Eigen orocos-kdl)

# SWIG Python wrappers
cisst_offer_python (cisstRobot)
