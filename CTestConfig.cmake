#
# $Id$
#
# (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---


# Set a long and somewhat informative build name for Dart
SET(BUILDNAME_TMP ${CMAKE_SYSTEM_NAME})

# Try to define a short name for generator
IF(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}-Make)
ELSE(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
  IF(${CMAKE_GENERATOR} STREQUAL "NMake Makefiles")
    SET(BUILDNAME_TMP ${BUILDNAME_TMP}-NMake)
  ELSE(${CMAKE_GENERATOR} STREQUAL "NMake Makefiles")
    IF(${MSVC70})
      SET(BUILDNAME_TMP ${BUILDNAME_TMP}-VC7.0)
    ELSE(${MSVC70})
      IF(${MSVC71})
        SET(BUILDNAME_TMP ${BUILDNAME_TMP}-VC7.1)
      ELSE(${MSVC71})
        IF(${MSVC80})
          SET(BUILDNAME_TMP ${BUILDNAME_TMP}-VC8.0)
        ELSE(${MSVC80})
          SET(BUILDNAME_TMP ${BUILDNAME_TMP}-${CMAKE_GENERATOR})
        ENDIF(${MSVC80})
      ENDIF(${MSVC71})
    ENDIF(${MSVC70})
  ENDIF(${CMAKE_GENERATOR} STREQUAL "NMake Makefiles")
ENDIF(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")

# Try to define a good name for compiler
IF(CMAKE_COMPILER_IS_GNUCXX)
  EXEC_PROGRAM(${CMAKE_CXX_COMPILER} ARGS -dumpversion
               OUTPUT_VARIABLE CISST_GNUCXX_VERSION)
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}-gcc-${CISST_GNUCXX_VERSION})
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

# Debug, Release, ...
IF(CMAKE_BUILD_TYPE)
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}-${CMAKE_BUILD_TYPE})
ENDIF(CMAKE_BUILD_TYPE)

# Tells which libraries have been compiled and assumed tests have been as well
IF(BUILD_LIBS_cisstCommon)
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}-Co)
ENDIF(BUILD_LIBS_cisstCommon)

IF(BUILD_LIBS_cisstVector)
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}Ve)
ENDIF(BUILD_LIBS_cisstVector)

IF(BUILD_LIBS_cisstNumerical)
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}Nu)
ENDIF(BUILD_LIBS_cisstNumerical)

IF(BUILD_LIBS_cisstInteractive)
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}In)
ENDIF(BUILD_LIBS_cisstInteractive)

IF(BUILD_LIBS_cisstOSAbstraction)
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}Os)
ENDIF(BUILD_LIBS_cisstOSAbstraction)

IF(BUILD_LIBS_cisstStereoVision)
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}Sv)
ENDIF(BUILD_LIBS_cisstStereoVision)

IF(BUILD_LIBS_cisstMultiTask)
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}Mt)
ENDIF(BUILD_LIBS_cisstMultiTask)

IF(BUILD_LIBS_cisstDevices)
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}Dv)
ENDIF(BUILD_LIBS_cisstDevices)

# Tells if Python was used
IF(CISST_HAS_SWIG_PYTHON)
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}-Py)
ENDIF(CISST_HAS_SWIG_PYTHON)

# Tells if cisstNetlib was used
IF(CISST_HAS_CISSTNETLIB)
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}-CiNe)
ENDIF(CISST_HAS_CISSTNETLIB)


# Save to cache
SET(BUILDNAME ${BUILDNAME_TMP}
    CACHE STRING "Full name of build configuration." FORCE)
MARK_AS_ADVANCED(BUILDNAME)


# Options for CDash
SET(CTEST_PROJECT_NAME "cisst")
SET(CTEST_DROP_METHOD "http")
SET(CTEST_DROP_SITE "www.cisst.org")
SET(CTEST_DROP_LOCATION "/cisst/CDash/submit.php?project=cisst")
# SET(CTEST_TRIGGER_SITE "")

# Set maximum execution time
SET(DART_TESTING_TIMEOUT 2000)

# the specified NIGHLY_START_TIME. Time is specified in 24 hour format.
SET(CTEST_NIGHTLY_START_TIME "00:00:00 EST")


