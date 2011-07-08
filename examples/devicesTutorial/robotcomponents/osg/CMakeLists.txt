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


project( osg )

add_executable( OSGtest OSGtest.cpp )
add_executable( OSGviewer OSGviewer.cpp )
add_executable( OSGPick OSGpick.cpp )
add_executable( OSGstereo OSGstereo.cpp )
add_executable( OSGstereox2 OSGstereox2.cpp )
add_executable( OSGwam OSGwam.cpp )
add_executable( OSGlwr OSGlwr.cpp )


cisst_target_link_libraries( OSGtest cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )

cisst_target_link_libraries( OSGviewer cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )

cisst_target_link_libraries( OSGPick cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )

cisst_target_link_libraries( OSGstereo cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )

cisst_target_link_libraries( OSGstereox2 cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )

cisst_target_link_libraries( OSGwam cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )

cisst_target_link_libraries( OSGlwr cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )


