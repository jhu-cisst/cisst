#
# $Id$
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
project(wamode)

# name the main executable and specifies with source files to use
add_executable( wamodegc wamodegc.cpp )
add_executable( wamodect wamodect.cpp )


# link with the cisst libraries
cisst_target_link_libraries( wamodegc "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstParameterTypes;cisstDevices;cisstRobot")

cisst_target_link_libraries( wamodect "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstParameterTypes;cisstDevices;cisstRobot")




