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

  find_package( OpenCV2 )

  add_executable( ODETest  ODETest.cpp )
  add_executable( ODEwam   ODEwam.cpp )
  add_executable( ODEbh    ODEbh.cpp )
  add_executable( ODEwambh ODEwambh.cpp )

  add_executable( mtsODEwam   mtsODEwam.cpp )
  add_executable( mtsODEbh    mtsODEbh.cpp )
  add_executable( mtsODEwambh mtsODEwambh.cpp )

  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCISST_ODE_SUPPORT -DdDOUBLE" )

  include_directories( 
    ${INCLUDE_OUTPUT_PATH}
    ${OPENSCENEGRAPH_INCLUDE_DIRS} 
    ${OpenCV2_INCLUDE_DIRS} 
    )

  set( OSG_EXE_LIBRARIES 
    cisstODE
    cisstOSG
    cisstCommon 
    cisstVector 
    cisstOSAbstraction
    cisstMultiTask
    cisstParameterTypes
    cisstRobot
    cisstNumerical
    ${OPENSCENEGRAPH_LIBRARIES}
    ${ODE_LIBRARY}
    ${OpenCV2_LIBRARIES}
    )  

  target_link_libraries( ODETest ${OSG_EXE_LIBRARIES} )
  target_link_libraries( ODEwam ${OSG_EXE_LIBRARIES} )
  target_link_libraries( ODEbh ${OSG_EXE_LIBRARIES} )
  target_link_libraries( ODEwambh ${OSG_EXE_LIBRARIES} )
  target_link_libraries( mtsODEwam ${OSG_EXE_LIBRARIES} )
  target_link_libraries( mtsODEbh ${OSG_EXE_LIBRARIES} )
  target_link_libraries( mtsODEwambh ${OSG_EXE_LIBRARIES} )

endif( OPENSCENEGRAPH_FOUND AND ODE_FOUND )

