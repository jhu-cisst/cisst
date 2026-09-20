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


# Offer the option to compile a given application or remove the option
# based on CISST_BUILD_APPLICATIONS
# For example: cisst_offer_application (cisstMultiTask ComponentGenerator ON)
macro (cisst_offer_application library application default)
  set (coa_OPTION_NAME CISST_${library}_${application})
  if (CISST_BUILD_APPLICATIONS)
    option (${coa_OPTION_NAME} "Build cisst${application}" ${default})
    mark_as_advanced (${coa_OPTION_NAME})
    if (${coa_OPTION_NAME})
      add_subdirectory (cisst${application})
    endif (${coa_OPTION_NAME})
  else (CISST_BUILD_APPLICATIONS)
    unset (${coa_OPTION_NAME} CACHE)
  endif (CISST_BUILD_APPLICATIONS)
endmacro (cisst_offer_application)


macro (cisst_do_not_offer_application library application)
  set (coa_OPTION_NAME CISST_${library}_${application})
  unset (${coa_OPTION_NAME} CACHE)
endmacro (cisst_do_not_offer_application)


# Offer the option to compile all examples in relative directory "examples"
# Default values are hard coded intentionally
macro (cisst_offer_examples library)
  set (coe_OPTION_NAME CISST_${library}_EXAMPLES)
  if (CISST_BUILD_EXAMPLES)
    option (${coe_OPTION_NAME} "Build ${library} examples" OFF)
    if (${coe_OPTION_NAME})
      add_subdirectory (examples)
    endif (${coe_OPTION_NAME})
  else (CISST_BUILD_EXAMPLES)
    unset (${coe_OPTION_NAME} CACHE)
  endif (CISST_BUILD_EXAMPLES)
endmacro (cisst_offer_examples)

# Offer the option to compile all tests in relative directory "tests"
# Default values are hard coded intentionally
macro (cisst_offer_tests library)
  set (cot_OPTION_NAME CISST_${library}_TESTS)
  if (CISST_BUILD_TESTS)
    option (${cot_OPTION_NAME} "Build ${library} tests" ON)
    mark_as_advanced (${cot_OPTION_NAME})
    if (${cot_OPTION_NAME})
      add_subdirectory (tests)
    endif (${cot_OPTION_NAME})
  else (CISST_BUILD_TESTS)
    unset (${cot_OPTION_NAME} CACHE)
  endif (CISST_BUILD_TESTS)
endmacro (cisst_offer_tests)

# Offer the option to compile all tests in relative directory "codePython"
# Default values are hard coded intentionally
macro (cisst_offer_python library)
  set (cop_OPTION_NAME CISST_${library}Python)
  if (CISST_HAS_SWIG_PYTHON AND CISST_SWIG_FOUND)
    option (${cop_OPTION_NAME} "Build ${library} Python" ON)
    mark_as_advanced (${cop_OPTION_NAME})
    if (${cop_OPTION_NAME})
      add_subdirectory (codePython)
    endif (${cop_OPTION_NAME})
  else (CISST_HAS_SWIG_PYTHON AND CISST_SWIG_FOUND)
    unset (${cop_OPTION_NAME} CACHE)
  endif (CISST_HAS_SWIG_PYTHON AND CISST_SWIG_FOUND)
endmacro (cisst_offer_python)

# Offer the option to compile all tests in relative directory "testsPython"
# Default values are hard coded intentionally
macro (cisst_offer_tests_python library)
  set (cotp_OPTION_NAME CISST_${library}Python_TESTS)
  if (CISST_${library}Python AND CISST_BUILD_TESTS)
    option (${cotp_OPTION_NAME} "Build ${library} Python tests" ON)
    mark_as_advanced (${cotp_OPTION_NAME})
    if (${cotp_OPTION_NAME})
      add_subdirectory (testsPython)
    endif (${cotp_OPTION_NAME})
  else (CISST_${library}Python AND CISST_BUILD_TESTS)
    unset (${cotp_OPTION_NAME} CACHE)
  endif (CISST_${library}Python AND CISST_BUILD_TESTS)
endmacro (cisst_offer_tests_python)
