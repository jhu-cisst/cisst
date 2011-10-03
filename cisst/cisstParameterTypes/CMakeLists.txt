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

# All cisstParameterTypes libraries
project (cisstParameterTypesLibs)

add_subdirectory (code)

if (CISST_HAS_SWIG_PYTHON)
  add_subdirectory (codePython)
endif (CISST_HAS_SWIG_PYTHON)

# build tests, add an option to skip them
set (EXTRAS TESTS)
foreach (extra ${EXTRAS})
  string (TOLOWER ${extra} extra_lower)
  if (CISST_BUILD_${extra})
    option (CISST_BUILD_cisstParameterTypes_${extra} "Build ${extra_lower} for cisstParameterTypes." ON)
    mark_as_advanced (CISST_BUILD_cisstParameterTypes_{extra})
    if (CISST_BUILD_cisstParameterTypes_${extra})
      add_subdirectory (${extra_lower})
    endif (CISST_BUILD_cisstParameterTypes_${extra})
  endif (CISST_BUILD_${extra})
endforeach (extra)
