#
# $Id$
#
# (C) Copyright 2005-2008 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

if(CISST_HAS_FLTK)
  add_subdirectory(example2)

  if (CISST_DEV_HAS_SENSABLEHD OR CISST_DEV_HAS_NOVINTHDL OR CISST_MTS_HAS_ICE)
    add_subdirectory (example1)
  endif (CISST_DEV_HAS_SENSABLEHD OR CISST_DEV_HAS_NOVINTHDL OR CISST_MTS_HAS_ICE)

  if(CISST_DEV_HAS_SENSABLEHD)
    add_subdirectory(example3)
    if(CISST_DEV_HAS_NOVINTHDL)
      add_subdirectory(example5)
    endif(CISST_DEV_HAS_NOVINTHDL)
  endif(CISST_DEV_HAS_SENSABLEHD)

  if(CISST_DEV_HAS_OPENIGTLINK)
    add_subdirectory(example4)
  endif(CISST_DEV_HAS_OPENIGTLINK)

else(CISST_HAS_FLTK)
  message(WARNING " cisstDevices example example2 will not be compiled since it requires CISST_HAS_FLTK")
endif(CISST_HAS_FLTK)

if(CISST_HAS_QT)

  if (CISST_HAS_XML)
    add_subdirectory(example6)

    if(CISST_DEV_HAS_MICRONTRACKER)
      add_subdirectory(example7)
    endif(CISST_DEV_HAS_MICRONTRACKER)
  endif(CISST_HAS_XML)
endif(CISST_HAS_QT)
