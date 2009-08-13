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
set(LIBRARY cisstDevices)

# make sure all options are set for all devices to OFF
option(CISST_DEV_HAS_SENSABLEHD "Build Sensable Device using HD library" OFF)
option(CISST_DEV_HAS_LOPOMOCO "Build LoPoMoCo card extension" OFF)
option(CISST_DEV_HAS_OPENIGTLINK "Open IGT Link protocol" OFF)

# set dependencies
set(DEPENDENCIES
    cisstCommon
    cisstVector
    cisstOSAbstraction
    cisstMultiTask
    cisstParameterTypes
    )

set(HEADER_FILES
    devNull.h
    devNDiSerial.h
    devNDiSerialToolInformationGet.h
    devSartoriusSerial.h
    devExport.h
    )

set(SOURCE_FILES
    devNull.cpp
    devNDiSerial.cpp
    devNDiSerialToolInformationGet.cpp
    devSartoriusSerial.cpp
    )

if(CISST_DEV_HAS_SENSABLEHD)
  find_package(Sensable REQUIRED)
  include_directories(${SENSABLE_INCLUDE_DIR})
  set(SOURCE_FILES ${SOURCE_FILES}
      devSensableHD.cpp
      devSensableHDMasterSlave.cpp)
  set(HEADER_FILES ${HEADER_FILES}
      devSensableHD.h
      devSensableHDMasterSlave.h)
endif(CISST_DEV_HAS_SENSABLEHD)

if(CISST_DEV_HAS_LOPOMOCO) 
  set(SOURCE_FILES ${SOURCE_FILES} 
      devLoPoMoCo.cpp 
      devLoPoMoCoOffsets.h
      devLoPoMoCoBoardIO.h)
  set(HEADER_FILES ${HEADER_FILES} 
      devLoPoMoCo.h) 
endif(CISST_DEV_HAS_LOPOMOCO) 

if(CISST_DEV_HAS_OPENIGTLINK)
  find_package(OpenIGTLink REQUIRED)
  include(${OpenIGTLink_USE_FILE})
  set(SOURCE_FILES ${SOURCE_FILES}
      devOpenIGTLink.cpp)
  set(HEADER_FILES ${HEADER_FILES}
      devOpenIGTLink.h)
  set(DEV_ADDITIONAL_CMAKE_FILES
      "${DEV_ADDITIONAL_CMAKE_FILES}"
      ${OpenIGTLink_USE_FILE})
  set(DEV_ADDITIONAL_PACKAGES
      "${DEV_ADDITIONAL_PACKAGES}"
      OpenIGTLink)
  set(DEV_ADDITIONAL_LINK_DIRECTORIES
      "${DEV_ADDITIONAL_LINK_DIRECTORIES}"
      ${OpenIGTLink_LIBRARY_DIRS})
  set(DEV_ADDITIONAL_LIBRARIES
      "${DEV_ADDITIONAL_LIBRARIES}"
      OpenIGTLink)
endif(CISST_DEV_HAS_OPENIGTLINK)

# Exporting settings to applications that link against cisst
CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_PACKAGES DEV_ADDITIONAL_PACKAGES)
CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_CMAKE_FILES DEV_ADDITIONAL_CMAKE_FILES)
CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_LINK_DIRECTORIES DEV_ADDITIONAL_LINK_DIRECTORIES)
CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_LIBRARIES DEV_ADDITIONAL_LIBRARIES)

# Create the config file
SET(CISST_DEV_CONFIG_FILE ${cisst_BINARY_DIR}/libs/include/cisstDevices/devConfig.h)
CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/include/cisstDevices/devConfig.h.in
               ${CISST_DEV_CONFIG_FILE}
               @ONLY)
INSTALL_FILES(/include/cisstDevices
               ".h"
               ${CISST_DEV_CONFIG_FILE})

# Add the config file to the project
SET_SOURCE_FILES_PROPERTIES("${CISST_DEV_CONFIG_FILE}"
                            PROPERTIES GENERATED TRUE)
SET(ADDITIONAL_HEADER_FILES ${CISST_DEV_CONFIG_FILE})

# Create the CMake config file
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/cisstDevicesBuild.cmake.in
               ${cisst_BINARY_DIR}/cisstDevicesBuild.cmake
               @ONLY)

# Finally create the library
CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)

# Add libraries to link with based on options
if(CISST_DEV_HAS_SENSABLEHD)
  target_link_libraries(cisstDevices ${SENSABLE_LIBRARIES})
endif(CISST_DEV_HAS_SENSABLEHD)
