#
# $Id: CMakeLists.txt 75 2009-02-24 16:47:20Z adeguet1 $
#
# (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

# name of project and executable
project( glut )

# name the main executable and specifies with source files to use
add_executable( robodocglut robodocglut.cpp )
add_executable( wamglut wamglut.cpp )

# link with the cisst libraries
cisst_target_link_libraries( robodocglut "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstParameterTypes;cisstDevices;cisstRobot")

cisst_target_link_libraries( wamglut "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstParameterTypes;cisstDevices;cisstRobot")




