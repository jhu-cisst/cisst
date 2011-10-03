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

# All cisstNumerical libraries
project (cisstNumericalLibs)

# Use Fortran NETLIB by default
option (CISST_HAS_CISSTNETLIB "Use the netlib.org sets of numerical methods in Fortran (provided on cisst.org)." ON)
mark_as_advanced (CISST_HAS_CISSTNETLIB)
if (CISST_HAS_CISSTNETLIB)
  find_package (CisstNetlib)
  cisst_set_package_settings (cisstNumerical cisstNetlib INCLUDE_DIRECTORIES CISSTNETLIB_INCLUDE_DIR)
  cisst_set_package_settings (cisstNumerical cisstNetlib LIBRARIES           CISSTNETLIB_LIBRARIES)
else (CISST_HAS_CISSTNETLIB)
  cisst_unset_all_package_settings (cisstNumerical cisstNetlib)
endif (CISST_HAS_CISSTNETLIB)

add_subdirectory (code)

if (CISST_HAS_SWIG_PYTHON)
  add_subdirectory (codePython)
endif (CISST_HAS_SWIG_PYTHON)

# build examples and tests, add an option to skip them
set (EXTRAS EXAMPLES TESTS)
foreach (extra ${EXTRAS})
  string (TOLOWER ${extra} extra_lower)
  if (CISST_BUILD_${extra})
    option (CISST_BUILD_cisstNumerical_${extra} "Build ${extra_lower} for cisstNumerical." ON)
    mark_as_advanced (CISST_BUILD_cisstNumerical_{extra})
    if (CISST_BUILD_cisstNumerical_${extra})
      add_subdirectory (${extra_lower})
    endif (CISST_BUILD_cisstNumerical_${extra})
  endif (CISST_BUILD_${extra})
endforeach (extra)
