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


# set the library name
set (LIBRARY cisstDevices)
set (LIBRARY_INCLUDE_DIR ${cisstLibs_SOURCE_DIR}/include/${LIBRARY})
set (DEPENDENCIES cisstCommon cisstVector cisstOSAbstraction cisstNumerical cisstMultiTask cisstParameterTypes cisstRobot)

# make sure all options are set for all devices to OFF
option (CISST_DEV_HAS_SENSABLEHD "Build Sensable Device using HD library" OFF)
option (CISST_DEV_HAS_LOPOMOCO "Build LoPoMoCo card extension" OFF)
option (CISST_DEV_HAS_OPENIGTLINK "Open IGT Link protocol" OFF)
option (CISST_DEV_HAS_NOVINTHDL "Build Novint Device using HDL library" OFF)
option (CISST_DEV_HAS_MICRONTRACKER "Add support for Claron MicronTracker." OFF)
option( CISST_DEV_HAS_BITCTRL "Use support BitCtrl." OFF )
option( CISST_DEV_HAS_RTSOCKETCAN "Use support RT Socket CAN." OFF )
option( CISST_DEV_HAS_ACCELEGLOVE "Use support for AcceleGlove." OFF )
option( CISST_DEV_HAS_ROBOTCOMPONENTS "Build robot components." OFF )

set (HEADER_FILES
     devNull.h
     devSartoriusSerial.h
     devExport.h
     devKeyboard.h
     can/devCAN.h
     )

set (SOURCE_FILES
     devNull.cpp
     devSartoriusSerial.cpp
     devKeyboard.cpp
     can/devCAN.cpp
     )

if( CISST_DEV_HAS_ROBOTCOMPONENTS )
  add_subdirectory( robotcomponents )
  set( HEADER_FILES ${HEADER_FILES} ${ROBOTCOMPONENTS_HEADER_FILES} )
  set( SOURCE_FILES ${SOURCE_FILES} ${ROBOTCOMPONENTS_SOURCE_FILES} )
endif( CISST_DEV_HAS_ROBOTCOMPONENTS )

# Sensable Omni
if (CISST_DEV_HAS_SENSABLEHD)
  find_package (Sensable REQUIRED)
  include_directories (${SENSABLE_INCLUDE_DIR})
  set (SOURCE_FILES ${SOURCE_FILES}
      devSensableHD.cpp
      devSensableHDMasterSlave.cpp)
  set (HEADER_FILES ${HEADER_FILES}
      devSensableHD.h
      devSensableHDMasterSlave.h)
endif (CISST_DEV_HAS_SENSABLEHD)

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

# Novint Falcon
if (CISST_DEV_HAS_NOVINTHDL)
  find_package (NovintHDL REQUIRED)
  include_directories (${NOVINT_INCLUDE_DIR})
  set (SOURCE_FILES ${SOURCE_FILES}
       devNovintHDL.cpp
       devNovintHDLMasterSlave.cpp)
  set (HEADER_FILES ${HEADER_FILES}
       devNovintHDL.h
       devNovintHDLMasterSlave.h)
endif (CISST_DEV_HAS_NOVINTHDL)

if (CISST_HAS_XML)
  list (APPEND SOURCE_FILES devNDISerial.cpp)
  list (APPEND HEADER_FILES devNDISerial.h)
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

MACRO (CISST_QT4_WRAP_UI outfiles )
  QT4_EXTRACT_OPTIONS (ui_files ui_options ${ARGN})

  FOREACH (it ${ui_files})
    GET_FILENAME_COMPONENT (outfile ${it} NAME_WE)
    GET_FILENAME_COMPONENT (infile ${it} ABSOLUTE)
# the only thing we modified ...
    SET (outfile ${cisst_BINARY_DIR}/libs/include/cisstDevices/${outfile}.h)
    ADD_CUSTOM_COMMAND (OUTPUT ${outfile}
                        COMMAND ${QT_UIC_EXECUTABLE}
                        ARGS ${ui_options} -o ${outfile} ${infile}
                        MAIN_DEPENDENCY ${infile})
    SET (${outfiles} ${${outfiles}} ${outfile})
  ENDFOREACH (it)
ENDMACRO (CISST_QT4_WRAP_UI)

# Qt widgets
if (CISST_HAS_QT)
  include_directories (${CMAKE_CURRENT_BINARY_DIR})
  qt4_wrap_cpp (CISST_DEV_NDISERIAL_QDEVICES_GENERATED
                ${LIBRARY_INCLUDE_DIR}/devNDISerialControllerQDevice.h
                ${LIBRARY_INCLUDE_DIR}/devNDISerialToolQDevice.h
                )
  set (CISST_DEV_NDISERIAL_QWIDGETS_UI
       devNDISerialControllerQWidget.ui
       devNDISerialToolQWidget.ui)
  cisst_qt4_wrap_ui (CISST_DEV_NDISERIAL_QWIDGETS_GENERATED
                     ${CISST_DEV_NDISERIAL_QWIDGETS_UI}
                     )

  set (ADDITIONAL_SOURCE_FILES
       ${ADDITIONAL_SOURCE_FILES}
       ${CISST_DEV_NDISERIAL_QDEVICES_GENERATED}
       ${CISST_DEV_NDISERIAL_QWIDGETS_GENERATED})
  set (SOURCE_FILES
       ${SOURCE_FILES}
       devNDISerialControllerQDevice.cpp
       devNDISerialToolQDevice.cpp)
  set (HEADER_FILES
       ${HEADER_FILES}
       devNDISerialControllerQDevice.h
       devNDISerialToolQDevice.h)

endif (CISST_HAS_QT)

# Create the config file
set (CISST_DEV_CONFIG_FILE ${cisst_BINARY_DIR}/libs/include/cisstDevices/devConfig.h)
configure_file (${cisst_SOURCE_DIR}/libs/include/cisstDevices/devConfig.h.in
                ${CISST_DEV_CONFIG_FILE}
                @ONLY)
install_files (/include/cisstDevices
               ".h"
               ${CISST_DEV_CONFIG_FILE})

# Add the config file to the project
set_source_files_properties ("${CISST_DEV_CONFIG_FILE}" PROPERTIES GENERATED TRUE)

# Create the CMake config file
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cisstDevicesBuild.cmake.in
                ${cisst_BINARY_DIR}/cisstDevicesBuild.cmake
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
if (CISST_DEV_HAS_SENSABLEHD)
  target_link_libraries (cisstDevices ${SENSABLE_LIBRARIES})
endif (CISST_DEV_HAS_SENSABLEHD)

if (CISST_DEV_HAS_NOVINTHDL)
  target_link_libraries (cisstDevices ${NOVINT_LIBRARIES})
endif (CISST_DEV_HAS_NOVINTHDL)

if (CISST_DEV_HAS_MICRONTRACKER)
  target_link_libraries (cisstDevices ${MicronTracker_LIBRARIES})
endif (CISST_DEV_HAS_MICRONTRACKER)

if (CISST_HAS_QT)
  target_link_libraries (cisstDevices ${QT_LIBRARIES})
endif (CISST_HAS_QT)

# Python wrappers
if (CISST_HAS_SWIG_PYTHON)
  cisst_add_swig_module (MODULE cisstDevices
                         INTERFACE_DIRECTORY ../../include/cisstDevices
                         CISST_LIBRARIES cisstDevices
                                         cisstParameterTypes
                                         cisstMultiTask
                                         cisstOSAbstraction
                                         cisstVector
                                         cisstCommon)
endif (CISST_HAS_SWIG_PYTHON)
