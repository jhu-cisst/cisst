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

if (CISST_cisstDevices)

  if (CISST_DEV_HAS_ROBOTCOMPONENTS)
    #add_subdirectory (robotcomponents)
  else (CISST_DEV_HAS_ROBOTCOMPONENTS)
      message ("Information: cisstDevices examples robotcomponents will not be compiled since they require CISST_DEV_HAS_ROBOTCOMPONENTS")
  endif (CISST_DEV_HAS_ROBOTCOMPONENTS)

  if (CISST_HAS_FLTK)
    # Sartorius scale example
    add_subdirectory(example2)
  else (CISST_HAS_FLTK)
    message ("Information: cisstDevices examples 1, 2, 3, 4 and 5 will not be compiled since they require CISST_HAS_FLTK")
  endif(CISST_HAS_FLTK)

  if (CISST_HAS_QT AND CISST_HAS_XML)
    if (CISST_DEV_HAS_MICRONTRACKER)
      add_subdirectory (example7)
    endif (CISST_DEV_HAS_MICRONTRACKER)
  else (CISST_HAS_QT AND CISST_HAS_XML)
    message ("Information: devicesTutorial examples 6 and 7 will not be compiled since they require CISST_HAS_XML and CISST_HAS_QT")
  endif (CISST_HAS_QT AND CISST_HAS_XML)

else (CISST_cisstDevices)
  message ("Information: devicesTutorial examples will not be compiled since they require CISST_Devices")
endif (CISST_cisstDevices)