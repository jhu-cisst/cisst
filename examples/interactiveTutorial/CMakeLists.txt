#
# $Id$
#
# (C) Copyright 2005-2010 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

if (CISST_BUILD_LIBS_cisstInteractive)
  add_subdirectory (pythonEmbedded)
  add_subdirectory (pythonEmbeddedIRE)
else (CISST_BUILD_LIBS_cisstInteractive)
  message (WARNING " interactiveTutorial examples will not be compiled since they require CISST_BUILD_LIBS_cisstInteractive")
endif (CISST_BUILD_LIBS_cisstInteractive)