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

# shared components used by either example
if (CISST_HAS_FLTK OR CISST_HAS_QT)
  add_subdirectory (components)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR})
endif (CISST_HAS_FLTK OR CISST_HAS_QT)

# FLTK examples
if (CISST_HAS_FLTK)
  add_subdirectory (FLTK)
else (CISST_HAS_FLTK)
  message (WARNING " cisstMultiTask example PeriodicTask/FLTK will not be compiled since it requires CISST_HAS_FLTK")
endif (CISST_HAS_FLTK)

# Qt examples
if (CISST_HAS_QT)
  add_subdirectory (Qt)
else (CISST_HAS_QT)
  message (WARNING " cisstMultiTask example PeriodicTask/Qt will not be compiled since it requires CISST_HAS_QT")
endif (CISST_HAS_QT)
