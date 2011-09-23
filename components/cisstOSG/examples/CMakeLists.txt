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


find_package( OpenSceneGraph 
  COMPONENTS osgDB osgGA osgViewer osgUtil osgVolume osgText )

if( OPENSCENEGRAPH_FOUND )

  project( osg )

  find_package( OpenCV2 )

  add_executable( OSGtest   OSGtest.cpp )
  add_executable( OSGstereo OSGstereo.cpp )
  add_executable( OSGwam    OSGwam.cpp )
  add_executable( OSGbh     OSGbh.cpp )
  add_executable( OSGpick   OSGpick.cpp )

  add_executable( mtsOSGTest   mtsOSGTest.cpp )
  add_executable( mtsOSGstereo mtsOSGstereo.cpp )
  add_executable( mtsOSGwam    mtsOSGwam.cpp )
  add_executable( mtsOSGbh     mtsOSGbh.cpp )

  include_directories( 
    ${INCLUDE_OUTPUT_PATH}
    ${OPENSCENEGRAPH_INCLUDE_DIRS} 
    ${OpenCV2_INCLUDE_DIRS} 
    )

  set( OSG_EXE_LIBRARIES 
    cisstCommon 
    cisstVector 
    cisstOSAbstraction
    cisstMultiTask
    cisstParameterTypes
    cisstRobot
    cisstNumerical
    )  
    
  target_link_libraries( OSGtest      cisstOSG ) 
  target_link_libraries( OSGstereo    cisstOSG ) 
  target_link_libraries( OSGwam       cisstOSG ) 
  target_link_libraries( OSGbh        cisstOSG ) 
  target_link_libraries( OSGpick      cisstOSG ) 

  target_link_libraries( mtsOSGTest   cisstOSG ) 
  target_link_libraries( mtsOSGstereo cisstOSG ) 
  target_link_libraries( mtsOSGwam    cisstOSG ) 
  target_link_libraries( mtsOSGbh     cisstOSG ) 
  
  # IMHO I don't think we need this since cisstOSG already has 
  # cisst_target_link_libraries
  cisst_target_link_libraries( OSGtest      ${OSG_EXE_LIBRARIES} ) 
  cisst_target_link_libraries( OSGstereo    ${OSG_EXE_LIBRARIES} ) 
  cisst_target_link_libraries( OSGwam       ${OSG_EXE_LIBRARIES} ) 
  cisst_target_link_libraries( OSGbh        ${OSG_EXE_LIBRARIES} ) 
  cisst_target_link_libraries( OSGpick      ${OSG_EXE_LIBRARIES} ) 

  cisst_target_link_libraries( mtsOSGTest   ${OSG_EXE_LIBRARIES} ) 
  cisst_target_link_libraries( mtsOSGstereo ${OSG_EXE_LIBRARIES} ) 
  cisst_target_link_libraries( mtsOSGwam    ${OSG_EXE_LIBRARIES} ) 
  cisst_target_link_libraries( mtsOSGbh     ${OSG_EXE_LIBRARIES} ) 

endif( OPENSCENEGRAPH_FOUND )

