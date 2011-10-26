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

# All cisstCommon libraries
project (cisstCommonLibs)

add_subdirectory (code)
add_subdirectory (applications)

if (CISST_HAS_SWIG_PYTHON)
  add_subdirectory (codePython)
  if (CISST_BUILD_TESTS)
    add_subdirectory (testsPython)
  endif (CISST_BUILD_TESTS)
endif (CISST_HAS_SWIG_PYTHON)

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
  set (CISST_HAS_XML ON PARENT_SCOPE) 
else (CISST_cisstCommonXML)
  set (CISST_HAS_XML OFF PARENT_SCOPE)
endif (CISST_cisstCommonXML)

# build examples and tests, add an option to skip them
cisst_offer_examples (cisstCommon)
cisst_offer_tests (cisstCommon)