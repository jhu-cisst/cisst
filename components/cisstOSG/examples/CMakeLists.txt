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

if( OPENSCENEGRAPH_FOUND )

  project( osg )

  add_executable( OSGtest   OSGtest.cpp )
  add_executable( OSGstereo OSGstereo.cpp )
  add_executable( OSGwam    OSGwam.cpp )
  add_executable( OSGbh     OSGbh.cpp )

  add_executable( mtsOSGTest   mtsOSGTest.cpp )
  add_executable( mtsOSGstereo mtsOSGstereo.cpp )
  add_executable( mtsOSGwam    mtsOSGwam.cpp )
  add_executable( mtsOSGbh     mtsOSGbh.cpp )

  include_directories( ${OPENSCENEGRAPH_INCLUDE_DIRS} )

  target_link_libraries( 
    OSGtest 
    cisstOSG
    cisstCommon 
    cisstVector 
    cisstOSAbstraction
    ${OPENSCENEGRAPH_LIBRARIES} )  

  target_link_libraries( 
    OSGstereo
    cisstOSG
    cisstCommon 
    cisstVector 
    cisstOSAbstraction
    ${OPENSCENEGRAPH_LIBRARIES} )  

  target_link_libraries( 
    OSGwam
    cisstOSG
    cisstCommon 
    cisstVector 
    cisstOSAbstraction
    cisstRobot
    cisstNumerical
    ${OPENSCENEGRAPH_LIBRARIES} )  

  target_link_libraries( 
    OSGbh
    cisstOSG
    cisstCommon 
    cisstVector 
    cisstOSAbstraction
    cisstRobot
    cisstNumerical
    ${OPENSCENEGRAPH_LIBRARIES} )  

  target_link_libraries( 
    mtsOSGTest
    cisstOSG
    cisstCommon 
    cisstVector 
    cisstOSAbstraction
    cisstRobot
    cisstNumerical
    cisstMultiTask 
    cisstParameterTypes
    ${OPENSCENEGRAPH_LIBRARIES} )  

  target_link_libraries( 
    mtsOSGstereo
    cisstOSG
    cisstCommon 
    cisstVector 
    cisstOSAbstraction
    cisstRobot
    cisstNumerical
    cisstMultiTask 
    cisstParameterTypes
    ${OPENSCENEGRAPH_LIBRARIES} )  

  target_link_libraries( 
    mtsOSGwam
    cisstOSG
    cisstCommon 
    cisstVector 
    cisstOSAbstraction
    cisstRobot
    cisstNumerical
    cisstMultiTask 
    cisstParameterTypes
    ${OPENSCENEGRAPH_LIBRARIES} )  

  target_link_libraries( 
    mtsOSGbh
    cisstOSG
    cisstCommon 
    cisstVector 
    cisstOSAbstraction
    cisstRobot
    cisstNumerical
    cisstMultiTask 
    cisstParameterTypes
    ${OPENSCENEGRAPH_LIBRARIES} )  

endif( OPENSCENEGRAPH_FOUND )

