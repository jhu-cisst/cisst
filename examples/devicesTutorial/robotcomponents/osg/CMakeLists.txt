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


find_package( OpenSceneGraph COMPONENTS osgDB osgGA osgViewer osgUtil )

if( OPENSCENEGRAPH_FOUND )

  project( osg )

  add_executable( OSGtest OSGtest.cpp )
  add_executable( OSGviewer OSGviewer.cpp )
  add_executable( OSGPick OSGpick.cpp )
  add_executable( OSGstereo OSGstereo.cpp )
  add_executable( OSGstereox2 OSGstereox2.cpp )
  add_executable( OSGwam OSGwam.cpp )
  add_executable( OSGlwr OSGlwr.cpp )
  add_executable( OSGvolume OSGvolume.cpp )
  add_executable( OSGtext OSGtext.cpp )

  cisst_target_link_libraries( OSGtest cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )  
  cisst_target_link_libraries( OSGvolume cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )  
  cisst_target_link_libraries( OSGviewer cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )
  cisst_target_link_libraries( OSGPick cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )
  cisst_target_link_libraries( OSGstereo cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )
  cisst_target_link_libraries( OSGstereox2 cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )
  cisst_target_link_libraries( OSGwam cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )
  cisst_target_link_libraries( OSGlwr cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )

  cisst_target_link_libraries( OSGtext cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )

#  find_package( OpenCV2 )

#  if( OpenCV2_FOUND )
#    cisst_add_to_internal( CISST_ADDITIONAL_LIBRARIES OpenCV2_LIBRARIES )
#    cisst_add_to_internal( CISST_ADDITIONAL_INCLUDE_DIRECTORIES OpenCV2_INCLUDE_DIRS )
    
#    add_executable( OSGcameracalibration OSGcameracalibration.cpp )
#    cisst_target_link_libraries( OSGcameracalibration cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )

#    add_executable( OSGEIH OSGEIH.cpp )
#    cisst_target_link_libraries( OSGEIH cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )

#  endif( OpenCV2_FOUND )


endif( OPENSCENEGRAPH_FOUND )

