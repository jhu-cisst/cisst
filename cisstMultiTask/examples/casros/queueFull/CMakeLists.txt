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

# include SAFECASS configuration.  This configuration file includes the 
# following cmake variables:
#
# Version
#   SC_VERSION_MAJOR
#   SC_VERSION_MINOR
#   SC_VERSION_PATCH
#   SC_VERSION
#
# Paths
#   SC_BUILD_ROOT
#   SC_BUILD_EXECUTABLE_PATH
#   SC_BUILD_LIBRARY_DIR
#
# For external project that uses SC
#   SC_SOURCE_DIR
#   SC_LIBRARY_DIR
#   SC_INCLUDE_DIR
#   SC_CMAKE_MODULE_PATH
#
# Logger option
#   SC_USE_G2LOG
#
# Compiler options (e.g., g2log requires c++11 standard)
#   SC_CXX_FLAGS
#
# Supported framework
#   SC_HAS_CISST
#
# External dependencies of Safety Framework
#   SC_BUILD_SHARED_LIBS
#   SC_LIB_DEPENDENCY_LIBS
#   SC_LIB_DEPENDENCY_INCLUDE_DIR
#
include (${SC_CONFIG_FILE})

if (NOT SC_HAS_CISST)
  message ("This version of SAFECASS example requires cisst")
  return()
endif()

# required casros libs (defined by casros build)
set (SAFECASS_LIBS ${SC_LIBRARY_FILE} ${SC_LIB_DEPENDENCY_LIBS})
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
set (REQUIRED_LIBS ${CISST_LIBS} ${SAFECASS_LIBS})

include_directories (${SC_INCLUDE_DIR} ${SC_LIB_DEPENDENCY_INCLUDE_DIR})
add_executable (casrosQueueFull main.cpp)
set_property (TARGET casrosQueueFull PROPERTY FOLDER "cisstMultiTask/applications")
target_link_libraries (casrosQueueFull ${REQUIRED_LIBS})
