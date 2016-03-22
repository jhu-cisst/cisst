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

# create wrappers using swig
cisst_add_swig_module (MODULE cisstRobot
                       INTERFACE_DIRECTORY ..
                       FOLDER cisstRobot
                       HEADER_FILES ../robPython.h
                       MODULE_LINK_LIBRARIES cisstRobot cisstNumerical cisstVector cisstCommon
                       INSTALL_FILES 1)
