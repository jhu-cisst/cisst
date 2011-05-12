
if( WIN32 )

  find_path( OPENNI_INCLUDE_DIR XnOpenNI.h 
    PATHS ENV OPEN_NI_INCLUDE PATH PATH_SUFFIXES Include )
  find_library( OPENNI_LIBRARY openNI PATHS ENV OPEN_NI_LIB )

else( WIN32 )
     
  if( APPLE )
    
  else( APPLE )
    
    find_file( OPENNI_INCLUDE_DIR XnOpenNI.h )
    find_library( OPENNI_LIBRARY OpenNI )

  endif (APPLE)

    
endif( WIN32 )

set( OPENNI_FOUND FALSE )

if( OPENNI_INCLUDE_DIR ) 
  if( OPENNI_LIBRARY )
    set( OPENNI_FOUND TRUE )
  endif( OPENNI_LIBRARY )
endif( OPENNI_INCLUDE_DIR )

mark_as_advanced( OPENNI_INCLUDE_DIR OPENNI_LIBRARY )


