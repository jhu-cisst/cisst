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

# All cisstStereoVision libraries
project (cisstStereoVisionLibs)

add_subdirectory (code)

if (CISST_HAS_SWIG_PYTHON)
  add_subdirectory (codePython)
endif (CISST_HAS_SWIG_PYTHON)

if (CISST_HAS_QT)
  add_subdirectory (codeQt)
endif (CISST_HAS_QT)

if (CISST_BUILD_EXAMPLES)
  option (CISST_cisstStereoVision_EXAMPLES "Build examples for cisstStereoVision." ON)
  mark_as_advanced (CISST_cisstStereoVision_EXAMPLES)
  if (CISST_cisstStereoVision_EXAMPLES)
    add_subdirectory (examples)
  endif (CISST_cisstStereoVision_EXAMPLES)
endif (CISST_BUILD_EXAMPLES)
