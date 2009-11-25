#
# $Id$
#
# (C) Copyright 2005-2009 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

option(CISST_HAS_FLTK "cisst has support for FLTK" OFF)

if(CISST_HAS_FLTK)
  find_package(FLTK REQUIRED)
  include_directories(${FLTK_INCLUDE_DIR})

  add_subdirectory(PeriodicTaskFLTK)
  add_subdirectory(CommandsAndEventsFLTK)
endif(CISST_HAS_FLTK)

if(CISST_HAS_QT)
  add_subdirectory(PeriodicTaskQt)
  add_subdirectory(CommandsAndEventsQt)
endif(CISST_HAS_QT)
