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

project (casrosOverrun)

include (${SC_CONFIG_FILE})

if (NOT SC_HAS_CISST)
  message ("This version of CASROS example requires cisst")
  return()
endif()

# required casros libs (defined by casros build)
set (CASROS_LIBS ${SC_LIBRARY_FILE} ${SC_LIB_DEPENDENCY_LIBS})
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

include_directories (${SC_INCLUDE_DIR} ${SC_LIB_DEPENDENCY_INCLUDE_DIR})
add_executable (casrosOverrun main.cpp)
set_property (TARGET casrosOverrun PROPERTY FOLDER "cisstMultiTask/applications")
target_link_libraries (casrosOverrun ${REQUIRED_LIBS})
