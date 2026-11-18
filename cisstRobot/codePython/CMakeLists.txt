#
# CMakeLists for cisstRobotPython
#
# (C) Copyright 2016 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (cisstRobotPython)

# create wrappers using pybind11
cisst_add_pybind11_module (MODULE cisstRobotPython
                            FOLDER cisstRobot
                            SOURCES cisstRobotPybind.cpp
                            MODULE_LINK_LIBRARIES cisstRobot cisstCommon
                            INSTALL_FILES)
