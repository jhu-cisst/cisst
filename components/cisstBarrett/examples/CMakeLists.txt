
project( Puck )


# Need to add the module path to search for FindRTSocket.cmake
set( 
  CMAKE_MODULE_PATH 
  ${CMAKE_MODULE_PATH} 
  ${CMAKE_CURRENT_SOURCE_DIR}/../../cisstCAN/code/cmake/Modules
  )

find_package( RTSocketCAN )
find_package( Xenomai )

if( RTSocketCANFound AND XenomaiFound )

  add_executable( PuckTest    PuckTest.cpp )
  add_executable( GroupTest   GroupTest.cpp )
  add_executable( WAMTest     WAMTest.cpp )

  add_executable( mtsPuckTest mtsPuckTest.cpp )
  add_executable( mtsWAMTest  mtsWAMTest.cpp )

  add_definitions( "${XENOMAI_DEFINITIONS}" )
  include_directories( ${RTDM_INCLUDE_DIR} )
  
  target_link_libraries( 
    PuckTest
    cisstBarrett
    cisstCAN
    ${XENOMAI_LIBRARY_RTDM} 
    ${XENOMAI_LIBRARY_XENOMAI}
    ${XENOMAI_LIBRARY_NATIVE} )

  target_link_libraries( 
    GroupTest
    cisstBarrett
    cisstCAN
    ${XENOMAI_LIBRARY_RTDM} 
    ${XENOMAI_LIBRARY_XENOMAI}
    ${XENOMAI_LIBRARY_NATIVE} )

  target_link_libraries( 
    WAMTest
    cisstBarrett
    cisstCAN
    ${XENOMAI_LIBRARY_RTDM} 
    ${XENOMAI_LIBRARY_XENOMAI}
    ${XENOMAI_LIBRARY_NATIVE} )

  target_link_libraries( 
    mtsPuckTest
    cisstBarrett
    cisstCAN
    ${XENOMAI_LIBRARY_RTDM} 
    ${XENOMAI_LIBRARY_XENOMAI}
    ${XENOMAI_LIBRARY_NATIVE} )

  target_link_libraries( 
    mtsWAMTest
    cisstBarrett
    cisstCAN
    ${XENOMAI_LIBRARY_RTDM} 
    ${XENOMAI_LIBRARY_XENOMAI}
    ${XENOMAI_LIBRARY_NATIVE} )

endif( RTSocketCANFound AND XenomaiFound )
