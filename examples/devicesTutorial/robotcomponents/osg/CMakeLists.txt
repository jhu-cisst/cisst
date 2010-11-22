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


include( FindOpenSceneGraph )
if( OPENSCENEGRAPH_FOUND )

  project( osg )

  add_executable( OSGtest OSGtest.cpp )
  add_executable( OSGstereo OSGstereo.cpp )

  cisst_target_link_libraries( OSGtest cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )

  cisst_target_link_libraries( OSGstereo cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )

  if( BUILD_LIBS_cisstStereoVision )

    add_executable( OSGsvl OSGsvl.cpp )
    cisst_target_link_libraries( OSGsvl cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask cisstStereoVision)

  endif( BUILD_LIBS_cisstStereoVision )

endif( OPENSCENEGRAPH_FOUND )
