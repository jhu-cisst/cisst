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


project( ode )

add_executable( ODEtest ODEtest.cpp )
add_executable( ODEstereo ODEstereo.cpp )
add_executable( ODEwam ODEwam.cpp )

cisst_target_link_libraries( ODEtest cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )

cisst_target_link_libraries( ODEstereo cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )

cisst_target_link_libraries( ODEwam cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )

