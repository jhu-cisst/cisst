#
# (C) Copyright 2005-2018 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# All cisstMultiTask libraries
project (cisstMultiTaskLibs)

# Determine 32 or 64 bit
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  set (CISST_MTS_64BIT TRUE)
else (CMAKE_SIZEOF_VOID_P EQUAL 8)
  set (CISST_MTS_64BIT FALSE)
endif (CMAKE_SIZEOF_VOID_P EQUAL 8)

# New features (use with caution)
option (CISST_MTS_NEW "Enable new cisstMultiTask features" ON)
mark_as_advanced (CISST_MTS_NEW)

add_subdirectory (code)

# Applications
add_subdirectory (applications)

# SWIG Python wrappers and tests
cisst_offer_python (cisstMultiTask)
cisst_offer_tests_python (cisstMultiTask)

if (CISST_HAS_QT)
  add_subdirectory (codeQt)
endif (CISST_HAS_QT)

# build examples and tests, add an option to skip them
cisst_offer_examples (cisstMultiTask)
cisst_offer_tests (cisstMultiTask)
