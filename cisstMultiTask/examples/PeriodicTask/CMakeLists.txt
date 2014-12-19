#
# (C) Copyright 2005-2014 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---


# create a list of libraries needed for this project
set (REQUIRED_CISST_LIBRARIES cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)

# find cisst and make sure the required libraries have been compiled
find_package (cisst COMPONENTS ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND_AS_REQUIRED)

  # load cisst configuration
  include (${CISST_USE_FILE})

  # shared components used by either example
  include_directories (${CMAKE_CURRENT_SOURCE_DIR})
  add_subdirectory (components)
  add_subdirectory (collector)

  # FLTK examples
  if (CISST_HAS_FLTK)
    add_subdirectory (FLTK)
  else (CISST_HAS_FLTK)
    message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR}/FLTK will not be compiled, it requires CISST_HAS_FLTK")
  endif (CISST_HAS_FLTK)

  # Qt examples
  if (CISST_HAS_QT)
    add_subdirectory (Qt)
  else (CISST_HAS_QT)
    message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR}/Qt will not be compiled, it requires CISST_HAS_QT")
  endif (CISST_HAS_QT)

else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)
