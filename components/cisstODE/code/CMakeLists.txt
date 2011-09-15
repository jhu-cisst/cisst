
find_package( OpenSceneGraph 
  COMPONENTS osgDB osgGA osgViewer osgUtil osgVolume osgText )

find_package( ODE )

if( OPENSCENEGRAPH_FOUND AND ODE_FOUND )

  SET( 
    HEADER_FILES 
    ../cisstODEBH.h
    ../cisstODEBody.h
    ../cisstODEJoint.h
    ../cisstODEManipulator.h
    ../cisstODEServoMotor.h
    ../cisstODEWorld.h

    ../mtsODEManipulatorTask.h
    ../mtsODEWorld.h

    ../mtsODEManipulator.h
    ../mtsODEBH.h
    )
    
  SET(
    SOURCE_FILES
    cisstODEBH.cpp
    cisstODEBody.cpp
    cisstODEJoint.cpp
    cisstODEManipulator.cpp
    cisstODEServoMotor.cpp
    cisstODEWorld.cpp

    mtsODEManipulatorTask.cpp
    )

  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCISST_ODE_SUPPORT -DdDOUBLE" )

  include_directories( ${OPENSCENEGRAPH_INCLUDE_DIRS}  ${ODE_INCLUDE_DIR} )
  
  add_library( cisstODE ${HEADER_FILES} ${SOURCE_FILES} )

  set (CISST_LIBRARIES_NEEDED 
    cisstCommon
    cisstCommonXML
    cisstVector
    cisstNumerical
    cisstOSAbstraction
    cisstMultiTask
    cisstParameterTypes
    cisstRobot)

  cisst_set_directories( ${CISST_LIBRARIES_NEEDED} )
  
  cisst_target_link_libraries( cisstODE ${CISST_LIBRARIES_NEEDED} )


endif( OPENSCENEGRAPH_FOUND AND ODE_FOUND )

