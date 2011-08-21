
set( 
  HEADER_FILES 
  ../cisstCAN.h 
  ../cisstCANFrame.h )
set( 
  SOURCE_FILES 
  cisstCAN.cpp
  cisstCANFrame.cpp )


# Check for RT Socket CAN
find_package( RTSocketCAN )
if( RTSocketCANFound )

  set( 
    HEADER_FILES 
    ${HEADER_FILES} 
    ../osaRTSocketCAN.h )

  set( 
    SOURCE_FILES 
    ${SOURCE_FILES} 
    osaRTSocketCAN.cpp )

  add_definitions( "${RTDM_DEFINITIONS}" )
  include_directories( ${RTDM_INCLUDE_DIR} )

endif( RTSocketCANFound )

# Check for BitCtrl
find_package( BitCtrl )
if( BitCtrlFound )

  set( 
    HEADER_FILES 
    ${HEADER_FILES} 
    ../CiA301.h
    ../CiA402.h
    ../osaCANopen.h   
    ../osaBitCtrl.h )

  set( 
    SOURCE_FILES 
    ${SOURCE_FILES} 
    ../osaCANopen.cpp 
    ../osaBitCtrl.cpp )

  include_directories( ${BITCTRL_INCLUDE_DIR} )

endif( BitCtrlFound )


add_library( cisstCAN ${HEADER_FILES} ${SOURCE_FILES} )
cisst_target_link_libraries ( 
  cisstCAN
  cisstCommon
  cisstVector
  cisstOSAbstraction
  cisstMultiTask )


set( cisstCAN_INCLUDE_DIR "${cisstComponents_SOURCE_DIR}" )
set( cisstCAN_LIBRARY_DIR ${LIBRARY_OUTPUT_PATH} )
set( cisstCAN_LIBRARIES cisstCAN ${ciOPENNI_LIBRARIES})