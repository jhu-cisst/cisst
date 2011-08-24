
set( 
  HEADER_FILES 
  ../Barrett.h
  ../osaPuck.h
  ../osaGroup.h
  ../osaWAM.h
  ../mtsPuck.h )

set( 
  SOURCE_FILES 
  osaPuck.cpp
  osaGroup.cpp
  osaWAM.cpp
  mtsPuck.cpp )


add_library( cisstBarrett ${HEADER_FILES} ${SOURCE_FILES} )
target_link_libraries ( 
  cisstBarrett
  cisstCommon
  cisstVector
  cisstOSAbstraction
  cisstMultiTask )


set( cisstBarrett_INCLUDE_DIR "${cisstComponents_SOURCE_DIR}" )
set( cisstBarrett_LIBRARY_DIR ${LIBRARY_OUTPUT_PATH} )
set( cisstBarrett_LIBRARIES cisstCAN ${ciOPENNI_LIBRARIES})