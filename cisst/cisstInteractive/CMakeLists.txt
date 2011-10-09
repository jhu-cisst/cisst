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

# All cisstInteractive libraries
project (cisstInteractiveLibs)

add_subdirectory (code)

if (CISST_BUILD_EXAMPLES)
  option (CISST_BUILD_cisstInteractive_EXAMPLES "Build examples for cisstInteractive." ON)
  mark_as_advanced (CISST_BUILD_cisstInteractive_EXAMPLES)
  if (CISST_BUILD_cisstInteractive_EXAMPLES)
    add_subdirectory (examples)
  endif (CISST_BUILD_cisstInteractive_EXAMPLES)
endif (CISST_BUILD_EXAMPLES)
