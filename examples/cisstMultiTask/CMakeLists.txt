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

if (BUILD_LIBS_cisstMultiTask)
  add_subdirectory (PeriodicTask)
  add_subdirectory (CommandsAndEvents)
else (BUILD_LIBS_cisstMultiTask)
  message (WARNING " cisstMultiTask examples will not be compiled since they require BUILD_LIBS_cisstMultiTask")
endif (BUILD_LIBS_cisstMultiTask)