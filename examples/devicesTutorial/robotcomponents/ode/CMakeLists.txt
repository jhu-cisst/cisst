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


find_package( OpenSceneGraph COMPONENTS osgDB osgGA osgViewer osgUtil osgVolume osgText )

if( OPENSCENEGRAPH_FOUND AND CISST_DEV_OPENSCENEGRAPH )

  find_package( ODE )
  if( ODE_FOUND )

    project( ode )

    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCISST_ODE_SUPPORT -DdDOUBLE" )

    add_executable( ODEtest ODEtest.cpp )
    add_executable( ODEstereo ODEstereo.cpp )
    add_executable( ODEwam ODEwam.cpp )
    add_executable( ODEbh ODEbh.cpp )
    add_executable( ODEwambh ODEwambh.cpp )
    add_executable( ODElwr ODElwr.cpp )
    add_executable( ODEgripper ODEgripper.cpp )
    add_executable( ODElwrgripper ODElwrgripper.cpp )

    cisst_target_link_libraries( ODEtest cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask cisstRobot cisstNumerical )

    cisst_target_link_libraries( ODEstereo cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask cisstRobot cisstNumerical )

    cisst_target_link_libraries( ODEwam cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask cisstRobot cisstNumerical )

    cisst_target_link_libraries( ODEbh cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask cisstRobot cisstNumerical )

    cisst_target_link_libraries( ODEwambh cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask cisstRobot cisstNumerical )

    cisst_target_link_libraries( ODElwr cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask cisstRobot cisstNumerical )

    cisst_target_link_libraries( ODEgripper cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask cisstRobot cisstNumerical )

    cisst_target_link_libraries( ODElwrgripper cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask cisstRobot cisstNumerical )

  endif( ODE_FOUND )

endif( OPENSCENEGRAPH_FOUND AND CISST_DEV_OPENSCENEGRAPH )
