
set( 
  HEADER_FILES 
  ../Barrett.h
  ../osaPuck.h
  ../osaGroup.h
  ../osaWAM.h
  ../mtsPuck.h
  ../mtsWAM.h )

set( 
  SOURCE_FILES 
  osaPuck.cpp
  osaGroup.cpp
  osaWAM.cpp
  mtsPuck.cpp
  mtsWAM.cpp )


add_library( cisstBarrett ${HEADER_FILES} ${SOURCE_FILES} )
target_link_libraries ( 
  cisstBarrett
  cisstCommon
  cisstVector
  cisstOSAbstraction
  cisstMultiTask
  cisstParameterTypes )

set( cisstBarrett_INCLUDE_DIR "${cisstComponents_SOURCE_DIR}" )
set( cisstBarrett_LIBRARY_DIR ${LIBRARY_OUTPUT_PATH} )
