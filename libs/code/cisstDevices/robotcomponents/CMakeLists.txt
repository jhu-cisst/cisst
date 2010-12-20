
option( CISST_DEV_HAS_ODE "Use support for ODE." OFF )
option( CISST_DEV_HAS_GLUT "Use support for GLUT." OFF )

set( HEADER_FILES

  robotcomponents/devRobotComponent.h
  
  robotcomponents/controllers/devController.h
  robotcomponents/controllers/devGravityCompensation.h
  robotcomponents/controllers/devComputedTorque.h
  robotcomponents/controllers/devPDGC.h

  robotcomponents/trajectories/devTrajectory.h
  robotcomponents/trajectories/devLinearR3.h
  robotcomponents/trajectories/devLinearRn.h
  robotcomponents/trajectories/devQLQRn.h
  robotcomponents/trajectories/devSLERP.h
  robotcomponents/trajectories/devLinearSE3.h
  robotcomponents/trajectories/devLinearIK.h

  robotcomponents/trajectories/devSetPoints.h

  robotcomponents/trajectories/devOMNItoWAM.h
  robotcomponents/trajectories/devOMNItoBH.h

  robotcomponents/manipulators/devManipulator.h

  )

set( SOURCE_FILES

  robotcomponents/devRobotComponent.cpp

  robotcomponents/controllers/devController.cpp
  robotcomponents/controllers/devComputedTorque.cpp
  robotcomponents/controllers/devGravityCompensation.cpp
  robotcomponents/controllers/devPDGC.cpp

  robotcomponents/trajectories/devTrajectory.cpp

  robotcomponents/trajectories/devLinearR3.cpp
  robotcomponents/trajectories/devLinearRn.cpp
  robotcomponents/trajectories/devQLQRn.cpp
  robotcomponents/trajectories/devSLERP.cpp
  robotcomponents/trajectories/devLinearSE3.cpp
  robotcomponents/trajectories/devLinearIK.cpp
  
  robotcomponents/trajectories/devSetPoints.cpp
  robotcomponents/trajectories/devOMNItoWAM.cpp
  robotcomponents/trajectories/devOMNItoBH.cpp
     
  robotcomponents/manipulators/devManipulator.cpp
  
  )


if( CISST_DEV_HAS_RTSOCKETCAN )

  SET( HEADER_FILES

    ${HEADER_FILES} 

    robotcomponents/manipulators/Barrett/devWAM.h
    robotcomponents/manipulators/Barrett/devPuck.h
    robotcomponents/manipulators/Barrett/devGroup.h
    robotcomponents/manipulators/Barrett/devProperties.h

    )

  SET( SOURCE_FILES

    ${SOURCE_FILES} 

    robotcomponents/manipulators/Barrett/devWAM.cpp
    robotcomponents/manipulators/Barrett/devPuck.cpp
    robotcomponents/manipulators/Barrett/devGroup.cpp

    )

endif( CISST_DEV_HAS_RTSOCKETCAN )

find_package( OpenSceneGraph COMPONENTS osgDB osgGA osgViewer )

if( OPENSCENEGRAPH_FOUND )

  SET( 
    HEADER_FILES 
    ${HEADER_FILES} 
    robotcomponents/osg/devOSGBody.h
    robotcomponents/osg/devOSGCamera.h
    robotcomponents/osg/devOSGManipulator.h
    robotcomponents/osg/devOSGStereo.h
    robotcomponents/osg/devOSGWorld.h
    )

  SET(
    SOURCE_FILES
    ${SOURCE_FILES} 
    robotcomponents/osg/devOSGBody.cpp
    robotcomponents/osg/devOSGCamera.cpp
    robotcomponents/osg/devOSGManipulator.cpp
    robotcomponents/osg/devOSGStereo.cpp
    robotcomponents/osg/devOSGWorld.cpp
    )

  cisst_add_to_internal(CISST_ADDITIONAL_LIBRARIES OPENSCENEGRAPH_LIBRARIES)

  find_package( ODE )
  if( ODE_FOUND )

    set( CMAKE_CXX_FLAGS 
      "${CMAKE_CXX_FLAGS} -DCISST_ODE_SUPPORT -DdDOUBLE" 
      PARENT_SCOPE )
    cisst_add_to_internal( CISST_ADDITIONAL_LIBRARIES ODE_LIBRARIES )
    
    SET( HEADER_FILES 
      
      ${HEADER_FILES} 
      
      robotcomponents/ode/devODEBody.h
      robotcomponents/ode/devODEJoint.h
      robotcomponents/ode/devODEServoMotor.h
      #      robotcomponents/ode/devODEManipulator.h
      
      #      robotcomponents/ode/devODEBH.h
      #      robotcomponents/ode/devODEBHF1.h
      #      robotcomponents/ode/devODEBHF2.h
      #      robotcomponents/ode/devODEBHF3.h
      
      )

    SET( 
      SOURCE_FILES
      
      ${SOURCE_FILES} 
      
      robotcomponents/ode/devODEBody.cpp
      robotcomponents/ode/devODEJoint.cpp
      robotcomponents/ode/devODEServoMotor.cpp
      #      robotcomponents/ode/devODEManipulator.cpp
      
      #      robotcomponents/ode/devODEBH.cpp
      #      robotcomponents/ode/devODEBHF1.cpp
      #      robotcomponents/ode/devODEBHF2.cpp
      #      robotcomponents/ode/devODEBHF3.cpp
      
      robotcomponents/ode/devODEWorld.cpp 
      
      )
    
  endif( ODE_FOUND )
    
endif( OPENSCENEGRAPH_FOUND )

# Check for GLUT
if( CISST_DEV_HAS_GLUT )

  include( FindOpenGL )
  if( OPENGL_FOUND )
    cisst_add_to_internal( CISST_ADDITIONAL_LIBRARIES OPENGL_LIBRARIES )
  endif( OPENGL_FOUND )

  include( FindGLUT )
  if( GLUT_FOUND )
    cisst_add_to_internal( CISST_ADDITIONAL_LIBRARIES GLUT_LIBRARIES )
  endif( GLUT_FOUND )

  SET( HEADER_FILES

    ${HEADER_FILES} 

    robotcomponents/glut/devGeometry.h
    robotcomponents/glut/devGLUT.h
    robotcomponents/glut/devGLUTManipulator.h
    robotcomponents/glut/devMeshTriangular.h

    )
  
  SET( SOURCE_FILES

    ${SOURCE_FILES} 

    robotcomponents/glut/devGeometry.cpp
    robotcomponents/glut/devGLUT.cpp
    robotcomponents/glut/devGLUTManipulator.cpp
    robotcomponents/glut/devMeshTriangular.cpp

    )

endif( CISST_DEV_HAS_GLUT )

set( ROBOTCOMPONENTS_HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )
set( ROBOTCOMPONENTS_SOURCE_FILES ${SOURCE_FILES} PARENT_SCOPE )

