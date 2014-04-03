#
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

if (CISST_HAS_FLTK)
  add_subdirectory (FLTK)
else (CISST_HAS_FLTK)
  message("Information: code in ${CMAKE_CURRENT_SOURCE_DIR}/FLTK will not be compiled, it requires CISST_HAS_FLTK")
endif (CISST_HAS_FLTK)

if (CISST_HAS_QT)
  add_subdirectory (Qt)
else (CISST_HAS_QT)
  message("Information: code in ${CMAKE_CURRENT_SOURCE_DIR}/Qt will not be compiled, it requires CISST_HAS_QT")
endif (CISST_HAS_QT)
