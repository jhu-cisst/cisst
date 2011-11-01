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
option (CISST_DEV_HAS_LOPOMOCO "Build LoPoMoCo card extension" OFF)
option (CISST_DEV_HAS_OPENIGTLINK "Open IGT Link protocol" OFF)
option (CISST_DEV_HAS_MICRONTRACKER "Add support for Claron MicronTracker." OFF)
option( CISST_DEV_HAS_BITCTRL "Use support BitCtrl." OFF )
option( CISST_DEV_HAS_RTSOCKETCAN "Use support RT Socket CAN." OFF )
option( CISST_DEV_HAS_ACCELEGLOVE "Use support for AcceleGlove." OFF )
option( CISST_DEV_HAS_ROBOTCOMPONENTS "Build robot components." OFF )

set (HEADER_FILES
     devNull.h
     devSartoriusSerial.h
     devExport.h
     can/devCAN.h
     )

set (SOURCE_FILES
     devNull.cpp
     devSartoriusSerial.cpp
     can/devCAN.cpp
     )

if( CISST_DEV_HAS_ROBOTCOMPONENTS )
  add_subdirectory( robotcomponents )
  set( HEADER_FILES ${HEADER_FILES} ${ROBOTCOMPONENTS_HEADER_FILES} )
  set( SOURCE_FILES ${SOURCE_FILES} ${ROBOTCOMPONENTS_SOURCE_FILES} )
endif( CISST_DEV_HAS_ROBOTCOMPONENTS )

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

if (CISST_DEV_HAS_LOPOMOCO)
  set (SOURCE_FILES ${SOURCE_FILES}
       devLoPoMoCo.cpp
       devLoPoMoCoOffsets.h
       devLoPoMoCoBoardIO.h)
  set (HEADER_FILES ${HEADER_FILES}
       devLoPoMoCo.h)
endif (CISST_DEV_HAS_LOPOMOCO)

# OpenIGTLink
if (CISST_DEV_HAS_OPENIGTLINK)
  find_package (OpenIGTLink REQUIRED)
  include (${OpenIGTLink_USE_FILE})
  set (SOURCE_FILES
       ${SOURCE_FILES}
       devOpenIGTLink.cpp)
  set (HEADER_FILES
       ${HEADER_FILES}
       devOpenIGTLink.h)
  cisst_set_package_settings (cisstDevices OpenIGTLink CMAKE_FILES      OpenIGTLink_USE_FILE)
  set (OpenIGTLink_PACKAGE_NAME "OpenIGTLink")
  cisst_set_package_settings (cisstDevices OpenIGTLink PACKAGES         OpenIGTLink_PACKAGE_NAME)
  cisst_set_package_settings (cisstDevices OpenIGTLink LINK_DIRECTORIES OpenIGTLink_LIBRARY_DIRS)
  set (OpenIGTLink_LIBRARY_NAME "OpenIGTLink")
  cisst_set_package_settings (cisstDevices OpenIGTLink LIBRARIES        OpenIGTLink_LIBRARY_NAME)
else (CISST_DEV_HAS_OPENIGTLINK)
  cisst_unset_all_package_settings (cisstDevices OpenIGTLink)
endif (CISST_DEV_HAS_OPENIGTLINK)

if (CISST_HAS_XML)
  set (DEPENDENCIES ${DEPENDENCIES} cisstCommonXML)

  if (CISST_DEV_HAS_MICRONTRACKER)
    list (APPEND DEPENDENCIES cisstStereoVision)
    find_path (MicronTracker_INCLUDE_DIR
               NAMES MTC.h
               PATHS "C:/Program Files/Claron Technology/MicronTracker/Dist"
                     "C:/Program Files (x86)/Claron Technology/MicronTracker/Dist")
    include_directories (${MicronTracker_INCLUDE_DIR})
    find_library (MicronTracker_MTC_LIBRARY
                  NAMES MTC
                  HINTS ${MicronTracker_INCLUDE_DIR})
    set (MicronTracker_LIBRARIES
         ${MicronTracker_MTC_LIBRARY})
    if (UNIX)
      find_library (MicronTracker_dc1394_LIBRARY
                    NAMES dc1394
                    HINTS ${MicronTracker_INCLUDE_DIR})
      find_library (raw1394_LIBRARY
                    NAMES raw1394)
      find_library (vnl_LIBRARY
                NAMES vnl)
      find_library (vnl_algo_LIBRARY
                NAMES vnl_algo)
      find_library (vcl_LIBRARY
                NAMES vcl)
      find_library (v3p_netlib_LIBRARY
                NAMES v3p_netlib)
      list (APPEND MicronTracker_LIBRARIES
            ${MicronTracker_dc1394_LIBRARY}
            ${raw1394_LIBRARY}
            ${vnl_LIBRARY}
            ${vnl_algo_LIBRARY}
            ${vcl_LIBRARY}
            ${v3p_netlib_LIBRARY})
    endif (UNIX)
    list (APPEND SOURCE_FILES devMicronTracker.cpp)
    list (APPEND HEADER_FILES devMicronTracker.h)
    list (APPEND DEV_ADDITIONAL_INCLUDE_DIR ${MicronTracker_INCLUDE_DIR})
    list (APPEND DEV_ADDITIONAL_LIBRARIES ${MicronTracker_LIBRARIES})
  endif (CISST_DEV_HAS_MICRONTRACKER)
endif (CISST_HAS_XML)

# Create the config file
set (CISST_DEV_CONFIG_FILE ${cisstLibs_BINARY_DIR}/include/cisstDevices/devConfig.h)
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
                ${cisstLibs_BINARY_DIR}/CMakeFiles/cisstDevicesBuild.cmake
                @ONLY)

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
