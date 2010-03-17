#
# $Id$
#
# (C) Copyright 2003-2009 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

project (cisstUtilities)

# Set the ouptut path for the libraries and executables
set (LIBRARY_OUTPUT_PATH ${cisstUtilities_BINARY_DIR}/lib)
set (EXECUTABLE_OUTPUT_PATH ${cisstUtilities_BINARY_DIR}/bin)

# List all utilities
set (CISST_UTILITIES 
     multiTaskGlobalComponentManager
     multiTaskGlobalComponentManagerFLTK)

# Which utilities to build
foreach (utility ${CISST_UTILITIES})
  # Build by default
  option (BUILD_UTILITIES_${utility} "Build ${utility}." OFF)
endforeach (utility)

# Set requirements per utility, user will be prompt if these requirements are not met
variable_requires(BUILD_UTILITIES_multiTaskGlobalComponentManager REQUIREMENT_MET BUILD_LIBS_cisstMultiTask)
variable_requires(BUILD_UTILITIES_multiTaskGlobalComponentManager REQUIREMENT_MET CISST_MTS_HAS_ICE)

variable_requires(BUILD_UTILITIES_multiTaskGlobalComponentManagerFLTK REQUIREMENT_MET BUILD_LIBS_cisstMultiTask)
variable_requires(BUILD_UTILITIES_multiTaskGlobalComponentManagerFLTK REQUIREMENT_MET CISST_MTS_HAS_ICE)
variable_requires(BUILD_UTILITIES_multiTaskGlobalComponentManagerFLTK REQUIREMENT_MET CISST_HAS_FLTK)

# Create targets for each utility
foreach (utility ${CISST_UTILITIES})
  if (BUILD_UTILITIES_${utility})
    add_subdirectory (${utility})
  endif (BUILD_UTILITIES_${utility})
endforeach (utility)
