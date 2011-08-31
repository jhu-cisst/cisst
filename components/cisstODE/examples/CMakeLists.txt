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


find_package( OpenSceneGraph 
  COMPONENTS osgDB osgGA osgViewer osgUtil osgVolume osgText )

find_package( ODE )

if( OPENSCENEGRAPH_FOUND AND ODE_FOUND )

  project( ode )

  add_executable( ODETest  ODETest.cpp )
  add_executable( ODEwam   ODEwam.cpp )
  add_executable( ODEbh    ODEbh.cpp )
  add_executable( ODEwambh ODEwambh.cpp )

  add_executable( mtsODEwam   mtsODEwam.cpp )
  add_executable( mtsODEbh    mtsODEbh.cpp )
  add_executable( mtsODEwambh mtsODEwambh.cpp )

  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCISST_ODE_SUPPORT -DdDOUBLE" )

  include_directories( ${OPENSCENEGRAPH_INCLUDE_DIRS} ${ODE_INCLUDE_DIR} )

  target_link_libraries( 
    ODETest
    cisstODE
    cisstOSG
    cisstCommon 
    cisstVector 
    ${OPENSCENEGRAPH_LIBRARIES}
    ${ODE_LIBRARY} )  

  target_link_libraries( 
    ODEwam
    cisstODE
    cisstOSG
    cisstCommon 
    cisstVector 
    ${OPENSCENEGRAPH_LIBRARIES}
    ${ODE_LIBRARY} )  

  target_link_libraries( 
    ODEbh
    cisstODE
    cisstOSG
    cisstCommon 
    cisstVector 
    ${OPENSCENEGRAPH_LIBRARIES}
    ${ODE_LIBRARY} )  

  target_link_libraries( 
    ODEwambh
    cisstODE
    cisstOSG
    cisstCommon 
    cisstVector 
    ${OPENSCENEGRAPH_LIBRARIES}
    ${ODE_LIBRARY} )  

  target_link_libraries( 
    mtsODEwam
    cisstODE
    cisstOSG
    cisstCommon 
    cisstVector 
    ${OPENSCENEGRAPH_LIBRARIES}
    ${ODE_LIBRARY} )  

  target_link_libraries( 
    mtsODEbh
    cisstODE
    cisstOSG
    cisstCommon 
    cisstVector 
    ${OPENSCENEGRAPH_LIBRARIES}
    ${ODE_LIBRARY} )  

  target_link_libraries( 
    mtsODEwambh
    cisstODE
    cisstOSG
    cisstCommon 
    cisstVector 
    ${OPENSCENEGRAPH_LIBRARIES}
    ${ODE_LIBRARY} )  

endif( OPENSCENEGRAPH_FOUND AND ODE_FOUND )

