
if( WIN32 )
  
  # Find the file XnOpenNI.h
  # For this to work properly you must have the environment variable OPEN_NI_INCLUDE set
  # to the OpenNI include directory (set by OpenNI self-extraction)
  find_path( OPENNI_INCLUDE_DIR XnOpenNI.h 
  PATHS ENV OPEN_NI_INCLUDE PATH PATH_SUFFIXES Include )

  # Find the base openNI library
  find_library( OPENNI_LIBRARY openNI PATHS ENV OPEN_NI_LIB )

else( WIN32 )
     
  if( APPLE )
    find_path( OPENNI_INCLUDE_DIR XnOpenNI.h PATH_SUFFIXES ni )
    find_library( OPENNI_LIBRARY           OpenNI )
	
  else( APPLE )
    
    # Find the file XnOpenNI.h
    # This searches in the standard include paths and check for "ni" directory
    # this works for Fedora 14 package
    find_path( OPENNI_INCLUDE_DIR XnOpenNI.h PATH_SUFFIXES ni )
    find_library( OPENNI_LIBRARY           OpenNI )
    find_library( XNCORE_LIBRARY           XnCore )
    find_library( XNFORMATS_LIBRARY        XnFormats  )
    find_library( XNDEVICESENSORV2_LIBRARY XnDeviceSensorV2 )
    find_library( XNDDK_LIBRARY            XnDDK  )
    
  endif (APPLE)

    
endif( WIN32 )

set( OPENNI_FOUND FALSE )

if( OPENNI_INCLUDE_DIR ) 

  if( OPENNI_LIBRARY )
    set( OPENNI_FOUND TRUE )
  endif( OPENNI_LIBRARY )

endif( OPENNI_INCLUDE_DIR )


set( OPENNI_INCLUDE_DIRS ${OPENNI_INCLUDE_DIR} )

set( 
  OPENNI_LIBRARIES 
  ${OPENNI_LIBRARY} 
  ${XNCORE_LIBRARY}
  ${XNFORMATS_LIBRARY}
  ${XNDEVICESENSORV2_LIBRARY}
  ${XNDDK_LIBRARY}
  )

mark_as_advanced( 
  OPENNI_INCLUDE_DIRS 
  OPENNI_INCLUDE_DIR 
  OPENNI_LIBRARIES
  OPENNI_LIBRARY 
  XNCORE_LIBRARY
  XNFORMATS_LIBRARY
  XNDEVICESENSORV2_LIBRARY
  XNDDK_LIBRARY
  )



