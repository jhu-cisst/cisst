
project( CAN )

find_package( RTSocketCAN )
find_package( Xenomai )

if( RTSocketCANFound AND XenomaiFound )

  add_executable( RTSocketCANSend RTSocketCANSend.cpp )
  add_executable( RTSocketCANRecv RTSocketCANRecv.cpp )

  add_executable( mtsRTSocketCANTest mtsRTSocketCANTest.cpp )

  add_definitions( "${XENOMAI_DEFINITIONS}" )
  include_directories( ${RTDM_INCLUDE_DIR} )

  target_link_libraries( 
    RTSocketCANSend 
    cisstCAN 
    cisstCommon
    cisstVector 
    cisstOSAbstraction 
    cisstMultiTask
    ${XENOMAI_LIBRARY_RTDM} 
    ${XENOMAI_LIBRARY_XENOMAI}
    ${XENOMAI_LIBRARY_NATIVE} )

  target_link_libraries( 
    RTSocketCANRecv 
    cisstCAN 
    cisstCommon
    cisstVector 
    cisstOSAbstraction 
    cisstMultiTask
    ${XENOMAI_LIBRARY_RTDM}
    ${XENOMAI_LIBRARY_XENOMAI}
    ${XENOMAI_LIBRARY_NATIVE} )

  target_link_libraries( 
    mtsRTSocketCANTest
    cisstCAN 
    cisstCommon
    cisstVector 
    cisstOSAbstraction 
    cisstMultiTask
    ${XENOMAI_LIBRARY_RTDM}
    ${XENOMAI_LIBRARY_XENOMAI}
    ${XENOMAI_LIBRARY_NATIVE} )

endif( RTSocketCANFound AND XenomaiFound )



