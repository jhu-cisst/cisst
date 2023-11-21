#
# (C) Copyright 2005-2022 Johns Hopkins University (JHU), All Rights Reserved.
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
if (CISST_CATKIN_BUILT)
  option (CISST_HAS_CISSTNETLIB "Use the netlib.org sets of numerical methods in Fortran (provided on cisst.org)." ON)
else ()
  option (CISST_HAS_CISSTNETLIB "Use the netlib.org sets of numerical methods in Fortran (provided on cisst.org)." OFF)
endif ()

if (CISST_HAS_CISSTNETLIB)
  find_package (CisstNetlib REQUIRED)
  cisst_set_package_settings (cisstNumerical cisstNetlib INCLUDE_DIRECTORIES CISSTNETLIB_INCLUDE_DIR)
  cisst_set_package_settings (cisstNumerical cisstNetlib LIBRARIES           CISSTNETLIB_LIBRARIES)
else (CISST_HAS_CISSTNETLIB)
  cisst_unset_all_package_settings (cisstNumerical cisstNetlib)
endif (CISST_HAS_CISSTNETLIB)

add_subdirectory (code)

# SWIG Python wrappers and tests
cisst_offer_python (cisstNumerical)
cisst_offer_tests_python (cisstNumerical)

# build examples and tests, add an option to skip them
cisst_offer_examples (cisstNumerical)
cisst_offer_tests (cisstNumerical)
