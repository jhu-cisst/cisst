#
# $Id$
#
# CMakeLists for cisstDevice
#
# (C) Copyright 2008-2009 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (cisstDevices)

# set the library name
set (LIBRARY cisstDevices)
set (DEPENDENCIES cisstCommon cisstVector cisstOSAbstraction cisstNumerical cisstMultiTask cisstParameterTypes cisstRobot)

# make sure all options are set for all devices to OFF
option( CISST_DEV_HAS_BITCTRL "Use support BitCtrl." OFF )
option( CISST_DEV_HAS_RTSOCKETCAN "Use support RT Socket CAN." OFF )
option( CISST_DEV_HAS_ACCELEGLOVE "Use support for AcceleGlove." OFF )
#option( CISST_DEV_HAS_ROBOTCOMPONENTS "Build robot components." OFF )

set (HEADER_FILES
     devSartoriusSerial.h
     devExport.h
     can/devCAN.h
     )

set (SOURCE_FILES
     devSartoriusSerial.cpp
     can/devCAN.cpp
     )

#if( CISST_DEV_HAS_ROBOTCOMPONENTS )
#  add_subdirectory( robotcomponents )
#  set( HEADER_FILES ${HEADER_FILES} ${ROBOTCOMPONENTS_HEADER_FILES} )
#  set( SOURCE_FILES ${SOURCE_FILES} ${ROBOTCOMPONENTS_SOURCE_FILES} )
#endif( CISST_DEV_HAS_ROBOTCOMPONENTS )

# Check for BitCtrl
if( CISST_DEV_HAS_BITCTRL )
  find_package( BitCtrl )
  if( BITCTRL_FOUND )
    include_directories( ${BITCTRL_INCLUDE_DIR} )
    set( SOURCE_FILES ${SOURCE_FILES} can/devCANopen.cpp
                                      can/devBitCtrl.cpp )
    set( HEADER_FILES ${HEADER_FILES} can/CiA301.h
                                      can/CiA402.h
                                      can/devCANopen.h
                                      can/devBitCtrl.h )
  endif( BITCTRL_FOUND )
endif( CISST_DEV_HAS_BITCTRL )

# Check for RT Socket CAN
if( CISST_DEV_HAS_RTSOCKETCAN )
  set( SOURCE_FILES ${SOURCE_FILES} can/devRTSocketCAN.cpp )
  set( HEADER_FILES ${HEADER_FILES} can/devRTSocketCAN.h )
endif( CISST_DEV_HAS_RTSOCKETCAN )

if( CISST_DEV_HAS_ACCELEGLOVE )
  set(SOURCE_FILES ${SOURCE_FILES} devAcceleGlove.cpp)
  set(HEADER_FILES ${HEADER_FILES} devAcceleGlove.h)
endif( CISST_DEV_HAS_ACCELEGLOVE )

# Create the config file
set (CISST_DEV_CONFIG_FILE ${cisst_BINARY_DIR}/include/cisstDevices/devConfig.h)
configure_file (${cisstDevicesLibs_SOURCE_DIR}/devConfig.h.in
                ${CISST_DEV_CONFIG_FILE}
                @ONLY)
install (FILES ${CISST_DEV_CONFIG_FILE}
         DESTINATION include/cisstDevices
         COMPONENT cisstDevices)

# Add the config file to the project
set_source_files_properties ("${CISST_DEV_CONFIG_FILE}" PROPERTIES GENERATED TRUE)

# Create the CMake config file
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cisstDevicesBuild.cmake.in
                ${CISST_CMAKE_BINARY_DIR}/cisstDevicesBuild.cmake
                @ONLY)
install (FILES ${CISST_CMAKE_BINARY_DIR}/cisstDevicesBuild.cmake
         DESTINATION ${CISST_CMAKE_INSTALL_SUFFIX}
         COMPONENT cisstDevices)

# Finally create the library
cisst_add_library (LIBRARY cisstDevices
                   DEPENDENCIES ${DEPENDENCIES}
                   SOURCE_FILES ${SOURCE_FILES}
                   HEADER_FILES ${HEADER_FILES}
                   ADDITIONAL_SOURCE_FILES ${ADDITIONAL_SOURCE_FILES}
                   ADDITIONAL_HEADER_FILES ${CISST_DEV_CONFIG_FILE}
                   )

# Add libraries to link with based on options
if (CISST_DEV_HAS_MICRONTRACKER)
  target_link_libraries (cisstDevices ${MicronTracker_LIBRARIES})
endif (CISST_DEV_HAS_MICRONTRACKER)
