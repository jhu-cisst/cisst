#
# (C) Copyright 2005-2026 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# All cisstCommon libraries
project (cisstCommonLibs)

# cisstDataGenerator must be compiled first
if (NOT CMAKE_CROSSCOMPILING)
  add_subdirectory (applications/cisstDataGenerator)
endif()

add_subdirectory (code)

cisst_offer_python (cisstCommon)
cisst_offer_tests_python (cisstCommon)

if (CISST_HAS_QT)
  add_subdirectory (codeQt)
endif (CISST_HAS_QT)

# if on, include cisstCommonXML and expose XML support to local tests/headers
if (CISST_cisstCommonXML)
  add_subdirectory (codeXML)
  set (CISST_HAS_XML ON PARENT_SCOPE) # to generate cisstConfig.h properly
  set (CISST_HAS_XML ON) # both seems needed so sub-directory "tests" also knows if CISST_HAS_XML

  # Add cisstCommonXML if needed
  list (FIND CISST_LIBRARIES cisstCommonXML _cisstCommonXML_index)
  if (CISST_HAS_XML AND ${_cisstCommonXML_index} EQUAL -1)
    set (CISST_LIBRARIES cisstCommonXML ${CISST_LIBRARIES} PARENT_SCOPE)
  endif ()
  unset (_cisstCommonXML_index)

else (CISST_cisstCommonXML)
  set (CISST_HAS_XML OFF PARENT_SCOPE)
  set (CISST_HAS_XML OFF)
endif (CISST_cisstCommonXML)

# build examples and tests, add an option to skip them
cisst_offer_examples (cisstCommon)
cisst_offer_tests (cisstCommon)
