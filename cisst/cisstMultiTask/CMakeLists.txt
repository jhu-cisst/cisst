#
# $Id$
#
# (C) Copyright 2005-2012 Johns Hopkins University (JHU), All Rights
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

# Determine 32 or 64 bit
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  set (CISST_MTS_64BIT TRUE)
else (CMAKE_SIZEOF_VOID_P EQUAL 8)
  set (CISST_MTS_64BIT FALSE)
endif (CMAKE_SIZEOF_VOID_P EQUAL 8)

# Support for Safety Framework plug-ins
option (CISST_HAS_SAFETY_PLUGINS "Enable support for Safety Framework plug-ins including fault detection and diagnosis" OFF)
if (CISST_HAS_SAFETY_PLUGINS)
  find_package(SafetyFramework REQUIRED)
  cisst_set_package_settings (cisstMultiTask SafetyFramework INCLUDE_DIRECTORIES SF_INCLUDE_DIRS)
  cisst_set_package_settings (cisstMultiTask SafetyFramework LIBRARIES SF_LIBRARIES)
  if (SF_FOUND)
    find_path(SF_BUILD_DIR NAMES CMakeCache.txt include DOC "Top location of build tree of the Safety Framework" NO_DEFAULT_PATH)
    if (${SF_BUILD_DIR} STREQUAL "SF_BUILD_DIR-NOTFOUND")
      message(FATAL_ERROR "Build tree of Safety Framework not found")
    else (${SF_BUILD_DIR} STREQUAL "SF_BUILD_DIR-NOTFOUND")
      # Include Safety Framework build tree
      include_directories ("${SF_BUILD_DIR}")
      include_directories ("${SF_BUILD_DIR}/include")
      # if SafetyFramework found, include JsonCpp
      set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SF_INCLUDE_DIR}/../cmake)
      find_package(JSONCPP REQUIRED)
      cisst_set_package_settings (cisstMultiTask JSONCPP INCLUDE_DIRECTORIES JSONCPP_INCLUDE_DIR)
      cisst_set_package_settings (cisstMultiTask JSONCPP LIBRARIES JSONCPP_LIBRARIES)
    endif (${SF_BUILD_DIR} STREQUAL "SF_BUILD_DIR-NOTFOUND")
  else (SF_FOUND)
    message(FATAL_ERROR "Safety Framework is not found.")
  endif (SF_FOUND)
else (CISST_HAS_SAFETY_PLUGINS)
  cisst_unset_all_package_settings (cisstMultiTask SafetyFramework)
  cisst_unset_all_package_settings (cisstMultiTask JSONCPP)
endif (CISST_HAS_SAFETY_PLUGINS)

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
    if (CISST_HAS_SAFETY_PLUGINS)
      set (ICE_LIBRARIES_ALL ${ICE_LIBRARIES_ALL} optimized IceStorm debug IceStormd)
    endif (CISST_HAS_SAFETY_PLUGINS)
    cisst_set_package_settings (cisstMultiTask ZeroCIce LIBRARIES ICE_LIBRARIES_ALL)
  else (WIN32)
    set (ICE_LIBRARIES ${ICE_LIBRARY_NAME} IceUtil)
    if (CISST_HAS_SAFETY_PLUGINS)
      set (ICE_LIBRARIES ${ICE_LIBRARIES} IceStorm)
    endif (CISST_HAS_SAFETY_PLUGINS)
    cisst_set_package_settings (cisstMultiTask ZeroCIce LIBRARIES ICE_LIBRARIES)
  endif (WIN32)
else (CISST_MTS_HAS_ICE)
  cisst_unset_all_package_settings (cisstMultiTask ZeroCIce)
endif (CISST_MTS_HAS_ICE)

add_subdirectory (code)

# Applications
add_subdirectory (applications)

# SWIG Python wrappers and tests
cisst_offer_python (cisstMultiTask)
cisst_offer_tests_python (cisstMultiTask)

if (CISST_HAS_QT)
  add_subdirectory (codeQt)
endif (CISST_HAS_QT)

# build examples and tests, add an option to skip them
cisst_offer_examples (cisstMultiTask)
cisst_offer_tests (cisstMultiTask)
