#
# $Id$
#
# CMakeLists for wrapping
#
# (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
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

PROJECT(cisstWrapping)

# We define some new inline classes for the wrappers only
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# Wrap each library
FOREACH(lib ${CISST_LIBS})
  IF(BUILD_LIBS_${lib})
    IF(EXISTS ${cisstWrapping_SOURCE_DIR}/${lib})
      ADD_SUBDIRECTORY(${cisstWrapping_SOURCE_DIR}/${lib})
    ENDIF(EXISTS ${cisstWrapping_SOURCE_DIR}/${lib})
  ENDIF(BUILD_LIBS_${lib})
ENDFOREACH(lib)

