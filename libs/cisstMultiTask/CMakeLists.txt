#
# $Id$
#
# (C) Copyright 2005-2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# All cisstMultiTask libraries
project (cisstMultiTaskLibs)

# try to find ZeroC Ice
find_package (Ice)
if (ICE_FOUND)
  option (CISST_MTS_HAS_ICE "Enable inter-process communication (IPC) feature of cisstMultiTask, requires ICE from ZeroC" ON)
  mark_as_advanced (CISST_MTS_HAS_ICE)
else (ICE_FOUND)
  option (CISST_MTS_HAS_ICE "Enable inter-process communication (IPC) feature of cisstMultiTask, requires ICE from ZeroC" OFF)
endif (ICE_FOUND)

if (CISST_MTS_HAS_ICE)
  find_package (Ice REQUIRED)
  cisst_set_package_settings (cisstMultiTask ZeroCIce INCLUDE_DIRECTORIES ICE_INCLUDE_DIR)
  cisst_set_package_settings (cisstMultiTask ZeroCIce LINK_DIRECTORIES ICE_LIBRARY_DIR)
  if (WIN32)
    set (ICE_LIBRARIES_ALL optimized Ice  optimized IceUtil
                           debug     Iced debug     IceUtild)
    cisst_set_package_settings (cisstMultiTask ZeroCIce LIBRARIES ICE_LIBRARIES_ALL)
  else (WIN32)
    set (ICE_LIBRARIES ${ICE_LIBRARY_NAME} IceUtil)
    cisst_set_package_settings (cisstMultiTask ZeroCIce LIBRARIES ICE_LIBRARIES)
    message ("------------ do we need to save this as package settings?  link_directories(${ICE_LIBRARY_DIR})")
  endif (WIN32)
else (CISST_MTS_HAS_ICE)
  cisst_unset_all_package_settings (cisstMultiTask ZeroCIce)
endif (CISST_MTS_HAS_ICE)

# Create the config file
set (CISST_MTS_CONFIG_FILE ${cisst_BINARY_DIR}/libs/include/cisstMultiTask/mtsConfig.h)
configure_file (${cisst_SOURCE_DIR}/libs/include/cisstMultiTask/mtsConfig.h.in
                ${CISST_MTS_CONFIG_FILE}
                @ONLY)
install_files (/include/cisstMultiTask
               ".h"
               ${CISST_MTS_CONFIG_FILE})

# Add the config file to the project
set_source_files_properties ("${CISST_MTS_CONFIG_FILE}"
                             PROPERTIES GENERATED TRUE)
set (ADDITIONAL_HEADER_FILES ${ADDITIONAL_HEADER_FILES} ${CISST_MTS_CONFIG_FILE})

# Create the CMake config file
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cisstMultiTaskBuild.cmake.in
                ${cisst_BINARY_DIR}/cisstMultiTaskBuild.cmake
                @ONLY)

add_subdirectory (code)

if (CISST_HAS_SWIG_PYTHON)
  add_subdirectory (codePython)
endif (CISST_HAS_SWIG_PYTHON)

if (CISST_HAS_QT)
  add_subdirectory (codeQt)
endif (CISST_HAS_QT)
