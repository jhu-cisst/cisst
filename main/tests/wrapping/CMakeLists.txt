#
# $Id: CMakeLists.txt 87 2009-02-25 21:12:04Z adeguet1 $
#
# CMakeLists for wrapping tests
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


CMAKE_MINIMUM_REQUIRED(VERSION 2.0)
PROJECT(cisstWrappingTests)


# Check what seems available
FOREACH(lib ${CISST_LIBRARIES})

  # Make sure there is actually a directory of test case
  IF(EXISTS ${cisstWrappingTests_SOURCE_DIR}/${lib})
    ADD_SUBDIRECTORY(${lib})
  ENDIF(EXISTS ${cisstWrappingTests_SOURCE_DIR}/${lib})

ENDFOREACH(lib)

