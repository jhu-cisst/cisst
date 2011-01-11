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


if (BUILD_LIBS_cisstMultiTask)

  # FLTK examples
  if (CISST_HAS_FLTK)
    add_subdirectory (PeriodicTaskFLTK)
    add_subdirectory (CommandsAndEventsFLTK)
    add_subdirectory (ComponentGenerator)
  else (CISST_HAS_FLTK AND BUILD_LIBS_cisstMultiTask)
    message (WARNING " cisstMultiTask examples PeriodicTaskFLTK, CommandsAndEventsFLTK and ComponentGenerator will not be compiled since they require CISST_HAS_FLTK")
  endif (CISST_HAS_FLTK)

  # Qt examples
  if (CISST_HAS_QT)
    add_subdirectory (PeriodicTaskQt)
    add_subdirectory (CommandsAndEventsQt)
  else (CISST_HAS_QT)
    message (WARNING " cisstMultiTask examples PeriodicTaskQt and CommandsAndEventsQt will not be compiled since they require CISST_HAS_QT")
  endif (CISST_HAS_QT)

else (BUILD_LIBS_cisstMultiTask)
  message (WARNING " cisstMultiTask examples will not be compiled since they require BUILD_LIBS_cisstMultiTask")
endif (BUILD_LIBS_cisstMultiTask)