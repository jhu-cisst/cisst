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

# All cisstVector libraries
project (cisstVectorLibs)

add_subdirectory (code)

if (CISST_HAS_FLTK AND CISST_HAS_OPENGL)
  add_subdirectory (codeFLTK)
endif (CISST_HAS_FLTK AND CISST_HAS_OPENGL)

if (CISST_HAS_SWIG_PYTHON)
  add_subdirectory (codePython)
  if (CISST_BUILD_TESTS)
    add_subdirectory (testsPython)
  endif (CISST_BUILD_TESTS)
endif (CISST_HAS_SWIG_PYTHON)

if (CISST_HAS_QT AND CISST_HAS_OPENGL)
  add_subdirectory (codeQt)
endif (CISST_HAS_QT AND CISST_HAS_OPENGL)

# build examples and tests, add an option to skip them
set (EXTRAS EXAMPLES TESTS)
foreach (extra ${EXTRAS})
  string (TOLOWER ${extra} extra_lower)
  if (CISST_BUILD_${extra})
    option (CISST_BUILD_cisstVector_${extra} "Build ${extra_lower} for cisstVector." ON)
    mark_as_advanced (CISST_BUILD_cisstVector_{extra})
    if (CISST_BUILD_cisstVector_${extra})
      add_subdirectory (${extra_lower})
    endif (CISST_BUILD_cisstVector_${extra})
  endif (CISST_BUILD_${extra})
endforeach (extra)
