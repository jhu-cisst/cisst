
find_package( OpenSceneGraph 
  COMPONENTS osgDB osgGA osgViewer osgUtil osgVolume osgText )

if( OPENSCENEGRAPH_FOUND )

  find_package( OpenCV2 )
  if( OpenCV2_FOUND )
    set( CISST_OSG_OPENCV ON )
  endif( OpenCV2_FOUND )
  
  SET( 
    HEADER_FILES 
    ${HEADER_FILES} 

    ../cisstOSGBH.h
    ../cisstOSGBody.h
    ../cisstOSGCamera.h
    ../cisstOSGManipulator.h
    ../cisstOSGMono.h
    ../cisstOSGStereo.h
    ../cisstOSGWorld.h
    
    ../mtsOSGBody.h

    ../mtsOSGCameraTask.h
    ../mtsOSGMono.h
    ../mtsOSGStereo.h

    ../mtsOSGBH.h
    ../mtsOSGManipulator.h
    ../mtsOSGManipulatorTask.h
    )
    
    SET(
      SOURCE_FILES
      ${SOURCE_FILES} 

      cisstOSGBH.cpp
      cisstOSGBody.cpp
      cisstOSGCamera.cpp
      cisstOSGManipulator.cpp
      cisstOSGMono.cpp
      cisstOSGStereo.cpp
      cisstOSGWorld.cpp

      mtsOSGBody.cpp

      mtsOSGCameraTask.cpp

      mtsOSGManipulatorTask.cpp
      )
    
    if( ${OPENSCENEGRAPH_VERSION} VERSION_GREATER 2.9.15 )
      SET( 
	HEADER_FILES ${HEADER_FILES} 
#	../cisstOSGText.h 
#	../cisstOSGVolume.h 
	)
      SET( 
	SOURCE_FILES ${SOURCE_FILES} 
#	cisstOSGText.cpp
#	cisstOSGVolume.cpp
         )
    endif( ${OPENSCENEGRAPH_VERSION} VERSION_GREATER 2.9.15 )

    include_directories( 
      ${INCLUDE_OUTPUT_PATH}
      ${OPENSCENEGRAPH_INCLUDE_DIRS} 
      ${OpenCV2_INCLUDE_DIRS} 
      )

    add_library( cisstOSG ${HEADER_FILES} ${SOURCE_FILES} )

    cisst_target_link_libraries ( 
      cisstOSG
      cisstCommon
      cisstVector
      cisstOSAbstraction
      cisstRobot
      cisstNumerical
      cisstMultiTask
      cisstParameterTypes )

    set( 
      CISST_OSG_CONFIG_FILE 
      ${INCLUDE_OUTPUT_PATH}/cisstOSG/cisstOSGConfig.h 
      )

    configure_file(
      ../cisstOSGConfig.h.in
      ${CISST_OSG_CONFIG_FILE}
      @ONLY)
    


    mark_as_advanced( 
      OPENTHREADS_INCLUDE_DIR
      OPENTHREADS_LIBRARY
      OPENTHREADS_LIBRARY_DEBUG
      OSGDB_INCLUDE_DIR
      OSGDB_LIBRARY
      OSGDB_LIBRARY_DEBUG
      OSGGA_INCLUDE_DIR
      OSGGA_LIBRARY
      OSGGA_LIBRARY_DEBUG
      OSGTEXT_INCLUDE_DIR
      OSGTEXT_LIBRARY
      OSGTEXT_LIBRARY_DEBUG
      OSGUTIL_INCLUDE_DIR
      OSGUTIL_LIBRARY
      OSGUTIL_LIBRARY_DEBUG
      OSGVIEWER_INCLUDE_DIR
      OSGVIEWER_LIBRARY
      OSGVIEWER_LIBRARY_DEBUG
      OSGVOLUME_INCLUDE_DIR
      OSGVOLUME_LIBRARY
      OSGVOLUME_LIBRARY_DEBUG
      OSG_INCLUDE_DIR
      OSG_LIBRARY
      OSG_LIBRARY_DEBUG
      )

  mark_as_advanced( FORCE ${OpenCV2_OPTIONS})

endif( OPENSCENEGRAPH_FOUND )

