#
#
# (C) Copyright 2014 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (casrosQueueFull)

# include CASROS configuration.  This configuration file includes the 
# following cmake variables:
#
# Version
#   SF_VERSION_MAJOR
#   SF_VERSION_MINOR
#   SF_VERSION_PATCH
#   SF_VERSION
#
# Paths
#   SF_BUILD_ROOT
#   SF_BUILD_EXECUTABLE_PATH
#   SF_BUILD_LIBRARY_DIR
#
# For external project that uses SF
#   SF_SOURCE_DIR
#   SF_LIBRARY_DIR
#   SF_INCLUDE_DIR
#   SF_CMAKE_MODULE_PATH
#
# Logger option
#   SF_USE_G2LOG
#
# Compiler options (e.g., g2log requires c++11 standard)
#   SF_CXX_FLAGS
#
# Supported framework
#   SF_HAS_CISST
#
# External dependencies of Safety Framework
#   SF_BUILD_SHARED_LIBS
#   SF_LIB_DEPENDENCY_LIBS
#   SF_LIB_DEPENDENCY_INCLUDE_DIR
#
include (${SF_CONFIG_FILE})

if (NOT SF_HAS_CISST)
  message ("This version of CASROS example requires cisst")
  return()
endif()

# required casros libs (defined by casros build)
set (CASROS_LIBS ${SF_LIBRARY_FILE} ${SF_LIB_DEPENDENCY_LIBS})
# required cisst libs
set (CISST_LIBS cisstCommon cisstOSAbstraction cisstMultiTask)

# find cisst and make sure the required libraries have been compiled
find_package (cisst COMPONENTS ${CISST_LIBS} QUIET)
if (NOT cisst_FOUND_AS_REQUIRED)
  cisst_information_message_missing_libraries (${CISST_LIBS})
  return()
endif ()

# load cisst configuration
include (${CISST_USE_FILE})

# required libs
set (REQUIRED_LIBS ${CISST_LIBS} ${CASROS_LIBS})

include_directories (${SF_INCLUDE_DIR} ${SF_LIB_DEPENDENCY_INCLUDE_DIR})
add_executable (casrosQueueFull main.cpp)
set_property (TARGET casrosQueueFull PROPERTY FOLDER "cisstMultiTask/applications")
target_link_libraries (casrosQueueFull ${REQUIRED_LIBS})
