#
# $Id$
#
# (C) Copyright 2005-2010 Johns Hopkins University (JHU), All Rights
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
set (BUILDNAME_TMP ${CMAKE_SYSTEM_NAME})

# Try to define a short name for generator
if (${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
  set (BUILDNAME_TMP ${BUILDNAME_TMP}-Make)
else (${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
  if (${CMAKE_GENERATOR} STREQUAL "NMake Makefiles")
    set (BUILDNAME_TMP ${BUILDNAME_TMP}-NMake)
  else (${CMAKE_GENERATOR} STREQUAL "NMake Makefiles")
    if (${MSVC70})
      set (BUILDNAME_TMP ${BUILDNAME_TMP}-VC7.0)
    else (${MSVC70})
      if (${MSVC71})
        set (BUILDNAME_TMP ${BUILDNAME_TMP}-VC7.1)
      else (${MSVC71})
        if (${MSVC80})
          set (BUILDNAME_TMP ${BUILDNAME_TMP}-VC8.0)
        else (${MSVC80})
          set (BUILDNAME_TMP ${BUILDNAME_TMP}-${CMAKE_GENERATOR})
        endif (${MSVC80})
      endif (${MSVC71})
    endif (${MSVC70})
  endif (${CMAKE_GENERATOR} STREQUAL "NMake Makefiles")
endif (${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")

# Try to define a good name for compiler
if (CMAKE_COMPILER_IS_GNUCXX)
  exec_program (${CMAKE_CXX_COMPILER} ARGS -dumpversion
                OUTPUT_VARIABLE CISST_GNUCXX_VERSION)
  set (BUILDNAME_TMP ${BUILDNAME_TMP}-gcc-${CISST_GNUCXX_VERSION})
endif (CMAKE_COMPILER_IS_GNUCXX)

# Debug, Release, ...
if (CMAKE_BUILD_TYPE)
  set (BUILDNAME_TMP ${BUILDNAME_TMP}-${CMAKE_BUILD_TYPE})
endif (CMAKE_BUILD_TYPE)

# Tells which libraries have been compiled and assumed tests have been as well
if (BUILD_LIBS_cisstCommon)
  set (BUILDNAME_TMP ${BUILDNAME_TMP}-Co)
endif (BUILD_LIBS_cisstCommon)

if (BUILD_LIBS_cisstVector)
  set (BUILDNAME_TMP ${BUILDNAME_TMP}Ve)
endif (BUILD_LIBS_cisstVector)

if (BUILD_LIBS_cisstNumerical)
  set (BUILDNAME_TMP ${BUILDNAME_TMP}Nu)
endif (BUILD_LIBS_cisstNumerical)

if (BUILD_LIBS_cisstInteractive)
  set (BUILDNAME_TMP ${BUILDNAME_TMP}In)
endif (BUILD_LIBS_cisstInteractive)

if (BUILD_LIBS_cisstOSAbstraction)
  set (BUILDNAME_TMP ${BUILDNAME_TMP}Os)
endif (BUILD_LIBS_cisstOSAbstraction)

if (BUILD_LIBS_cisstStereoVision)
  set (BUILDNAME_TMP ${BUILDNAME_TMP}Sv)
endif (BUILD_LIBS_cisstStereoVision)

if (BUILD_LIBS_cisstMultiTask)
  set (BUILDNAME_TMP ${BUILDNAME_TMP}Mt)
endif (BUILD_LIBS_cisstMultiTask)

if (BUILD_LIBS_cisstDevices)
  set (BUILDNAME_TMP ${BUILDNAME_TMP}Dv)
endif (BUILD_LIBS_cisstDevices)

# Tells if Python was used
if (CISST_HAS_SWIG_PYTHON)
  set (BUILDNAME_TMP ${BUILDNAME_TMP}-Py)
endif (CISST_HAS_SWIG_PYTHON)

# Tells if cisstNetlib was used
if (CISST_HAS_CISSTNETLIB)
  set (BUILDNAME_TMP ${BUILDNAME_TMP}-CiNe)
endif (CISST_HAS_CISSTNETLIB)


# Save to cache
set (BUILDNAME ${BUILDNAME_TMP}
    CACHE STRING "Full name of build configuration." FORCE)
mark_as_advanced (BUILDNAME)


# Options for CDash
set (CTEST_PROJECT_NAME "cisst")
set (CTEST_DROP_METHOD "http")
set (CTEST_DROP_SITE "www.cisst.org")
set (CTEST_DROP_LOCATION "/cisst/CDash/submit.php?project=cisst")
# set (CTEST_TRIGGER_SITE "")

# Set maximum execution time
set (DART_TESTING_TIMEOUT 2000)

# the specified NIGHLY_START_TIME. Time is specified in 24 hour format.
set (CTEST_NIGHTLY_START_TIME "00:00:00 EST")
