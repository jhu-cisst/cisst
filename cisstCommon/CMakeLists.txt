#
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

# All cisstCommon libraries
project (cisstCommonLibs)

add_subdirectory (code)
add_subdirectory (applications)

cisst_offer_python (cisstCommon)
cisst_offer_tests_python (cisstCommon)

if (CISST_HAS_QT)
  add_subdirectory (codeQt)
endif (CISST_HAS_QT)

# set option to compile cisstCommonXML, default is on is either libxml2 or Qt has been found
if (CISST_FOUND_LIBXML2_XML OR CISST_FOUND_QT_XML)
  option (CISST_cisstCommonXML "Build cisstCommonXML" ON)
else (CISST_FOUND_LIBXML2_XML OR CISST_FOUND_QT_XML)
  option (CISST_cisstCommonXML "Build cisstCommonXML" OFF)
endif (CISST_FOUND_LIBXML2_XML OR CISST_FOUND_QT_XML)

# if on, include cisstCommonXML and let parent directory know for cisst-config.cmake
if (CISST_cisstCommonXML)
  add_subdirectory (codeXML)
  set (CISST_HAS_XML ON PARENT_SCOPE) # to generate cisstConfig.h properly
  set (CISST_HAS_XML ON) # both seems needed so sub-directory "tests" also knows if CISST_HAS_XML

  # Add cisstCommonXML if needed
  if (CISST_HAS_XML)
    set (CISST_LIBRARIES cisstCommonXML ${CISST_LIBRARIES} PARENT_SCOPE)
  endif (CISST_HAS_XML)

else (CISST_cisstCommonXML)
  set (CISST_HAS_XML OFF PARENT_SCOPE)
  set (CISST_HAS_XML OFF)
endif (CISST_cisstCommonXML)

# build examples and tests, add an option to skip them
cisst_offer_examples (cisstCommon)
cisst_offer_tests (cisstCommon)
