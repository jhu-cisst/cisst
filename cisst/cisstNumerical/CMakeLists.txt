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

if (CISST_BUILD_EXAMPLES)
  option (CISST_BUILD_cisstNumerical_EXAMPLES "Build examples for cisstNumerical." ON)
  mark_as_advanced (CISST_BUILD_cisstNumerical_EXAMPLES)
  if (CISST_BUILD_cisstNumerical_EXAMPLES)
    add_subdirectory (examples)
  endif (CISST_BUILD_cisstNumerical_EXAMPLES)
endif (CISST_BUILD_EXAMPLES)
