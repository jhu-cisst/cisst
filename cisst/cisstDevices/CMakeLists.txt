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

# All cisstDevices libraries
project (cisstDevicesLibs)

add_subdirectory (code)

if (CISST_HAS_SWIG_PYTHON)
  add_subdirectory (codePython)
endif (CISST_HAS_SWIG_PYTHON)

if (CISST_BUILD_EXAMPLES)
  option (CISST_cisstDevices_EXAMPLES "Build examples for cisstDevices." ON)
  mark_as_advanced (CISST_cisstDevices_EXAMPLES)
  if (CISST_cisstDevices_EXAMPLES)
    add_subdirectory (examples)
  endif (CISST_cisstDevices_EXAMPLES)
endif (CISST_BUILD_EXAMPLES)
