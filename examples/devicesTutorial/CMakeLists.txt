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

if (CISST_BUILD_cisstDevices)

  if (CISST_DEV_HAS_ROBOTCOMPONENTS)
    add_subdirectory (robotcomponents)
  else (CISST_DEV_HAS_ROBOTCOMPONENTS)
      message (WARNING " cisstDevices examples robotcomponents will not be compiled since they require CISST_DEV_HAS_ROBOTCOMPONENTS")
  endif (CISST_DEV_HAS_ROBOTCOMPONENTS)

  if (CISST_HAS_FLTK)
    add_subdirectory(example2)

    if (CISST_DEV_HAS_SENSABLEHD OR CISST_DEV_HAS_NOVINTHDL OR CISST_MTS_HAS_ICE)
      add_subdirectory (example1)
    else (CISST_DEV_HAS_SENSABLEHD OR CISST_DEV_HAS_NOVINTHDL OR CISST_MTS_HAS_ICE)
      message (WARNING " cisstDevices example 1 will not be compiled since it requires CISST_DEV_HAS_SENSABLEHD or CISST_MTS_HAS_ICE")
    endif (CISST_DEV_HAS_SENSABLEHD OR CISST_DEV_HAS_NOVINTHDL OR CISST_MTS_HAS_ICE)

    if (CISST_DEV_HAS_SENSABLEHD)
      add_subdirectory (example3)
      if (CISST_DEV_HAS_NOVINTHDL)
        add_subdirectory (example5)
      else (CISST_DEV_HAS_NOVINTHDL)
        message (WARNING " cisstDevices example 5 will not be compiled since it requires CISST_DEV_HAS_NOVINTHDL")
      endif (CISST_DEV_HAS_NOVINTHDL)
    else (CISST_DEV_HAS_SENSABLEHD)
      message (WARNING " cisstDevices examples 3 and 5 will not be compiled since they require CISST_DEV_HAS_SENSABLEHD")
    endif (CISST_DEV_HAS_SENSABLEHD)

    if (CISST_DEV_HAS_OPENIGTLINK)
      add_subdirectory (example4)
	else (CISST_DEV_HAS_OPENIGTLINK)
	   message (WARNING " cisstDevices example 4 will not be compiled since it requires CISST_DEV_HAS_OPENIGTLINK")
    endif(CISST_DEV_HAS_OPENIGTLINK)

  else (CISST_HAS_FLTK)
    message (WARNING " cisstDevices examples 1, 2, 3, 4 and 5 will not be compiled since they require CISST_HAS_FLTK")
  endif(CISST_HAS_FLTK)

  if (CISST_HAS_QT AND CISST_HAS_XML)
    add_subdirectory(example6)
    if (CISST_DEV_HAS_MICRONTRACKER)
      add_subdirectory (example7)
    endif (CISST_DEV_HAS_MICRONTRACKER)
  else (CISST_HAS_QT AND CISST_HAS_XML)
    message (WARNING " devicesTutorial examples 6 and 7 will not be compiled since they require CISST_HAS_XML and CISST_HAS_QT")
  endif (CISST_HAS_QT AND CISST_HAS_XML)

else (CISST_BUILD_cisstDevices)
  message (WARNING " devicesTutorial examples will not be compiled since they require CISST_BUILD_cisstDevices")
endif (CISST_BUILD_cisstDevices)