#
# $Id$
#
# CMakeLists for wrapping tests
#
# (C) Copyright 2004-2010 Johns Hopkins University (JHU), All Rights
# Reserved.
# 
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---


PROJECT(cisstWrappingTests)


# Check what seems available
FOREACH(lib ${CISST_LIBRARIES})

  # Make sure there is actually a directory of test case
  IF(EXISTS ${cisstWrappingTests_SOURCE_DIR}/${lib})
    ADD_SUBDIRECTORY(${lib})
  ENDIF(EXISTS ${cisstWrappingTests_SOURCE_DIR}/${lib})

ENDFOREACH(lib)

