#
# $Id: FindMicroScribe.cmake 3034 2011-10-09 01:53:36Z adeguet1 $
#
# Author(s):  Min Yang Jung
# Created on: 2011-10-06
#
# (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---
#
# Find the MicroScribe SDK includes and library
#
# This module defines
# MICROSCRIBE_INCLUDE_DIR, where to find tiff.h, etc.
# MICROSCRIBE_FOUND, If false, do not try to use MicroScribe.
# MICROSCRIBE_LIBRARY, where to find the MicroScribe library.

# MicroScribe SDK supports Windows and Mac OS X but the current version
# of SAW component only supports Windows.
if (WIN32)
  find_path (MICROSCRIBE_INCLUDE_DIR
             NAMES armdll32.h
             PATHS # Windows
                   "C:/Program Files/Immersion Corporation/MicroScribe SDK/inc"
             )

  find_library (MICROSCRIBE_LIBRARY armdll32
                ${MICROSCRIBE_INCLUDE_DIR}/../lib)
  if (MICROSCRIBE_INCLUDE_DIR)
    if (MICROSCRIBE_LIBRARY)
        set (MICROSCRIBE_FOUND "YES")
        set (MicroScribe_LIBRARIES ${MICROSCRIBE_LIBRARY})
        mark_as_advanced (MicroScribe_LIBRARIES MICROSCRIBE_LIBRARY
                          MICROSCRIBE_FOUND MICROSCRIBE_INCLUDE_DIR)
    endif (MICROSCRIBE_LIBRARY)
  endif (MICROSCRIBE_INCLUDE_DIR)
else (WIN32)
  message("Skipped building MicroScribe digitizer component: Current version supports Windows only")
endif (WIN32)


