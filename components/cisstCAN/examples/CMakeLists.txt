
project( CAN )

find_package( RTSocketCAN )
find_package( Xenomai )

if( RTSocketCANFound AND XenomaiFound )

  add_executable( RTSocketCANSend RTSocketCANSend.cpp )
  add_executable( RTSocketCANRecv RTSocketCANRecv.cpp )

  add_definitions( "${XENOMAI_DEFINITIONS}" )
  include_directories( ${RTDM_INCLUDE_DIR} )

  target_link_libraries( 
    RTSocketCANSend 
    cisstCAN 
    ${XENOMAI_LIBRARY_RTDM} 
    ${XENOMAI_LIBRARY_XENOMAI}
    ${XENOMAI_LIBRARY_NATIVE} )
  target_link_libraries( 
    RTSocketCANRecv 
    cisstCAN 
    ${XENOMAI_LIBRARY_RTDM}
    ${XENOMAI_LIBRARY_XENOMAI}
    ${XENOMAI_LIBRARY_NATIVE} )

  cisst_target_link_libraries( 
    RTSocketCANSend 
    cisstCommon
    cisstVector 
    cisstOSAbstraction 
    cisstMultiTask )

  cisst_target_link_libraries( 
    RTSocketCANRecv
    cisstCommon
    cisstVector 
    cisstOSAbstraction 
    cisstMultiTask )

endif( RTSocketCANFound )


