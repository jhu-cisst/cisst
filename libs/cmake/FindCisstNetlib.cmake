#
# $Id$
#
# Author(s):  Anton Deguet
# Created on: 2005-11-20
#
# (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
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
# Try to find CNetlib
# (code from www.netlib.org, ERC CISST customized version)
#
# The following values are defined
#
# CISSTNETLIB_INCLUDE_DIR     - include directories to use CISSTNETLIB
# CISSTNETLIB_LIBRARIES       - link against this to use CISSTNETLIB (fullpath)
#
# $Id$
#

SET(CISSTNETLIB_SEARCH_PATH
  /usr
  /usr/local
  /home/erc/cisstNetlib
)

FIND_PATH(CISSTNETLIB_DIR
          include/cisstNetlib.h ${CISSTNETLIB_SEARCH_PATH})

IF(CISSTNETLIB_DIR) 
  SET(CISSTNETLIB_INCLUDE_DIR ${CISSTNETLIB_DIR}/include)
  FIND_LIBRARY(CISSTNETLIB_LIBRARIES cisstNetlib ${CISSTNETLIB_DIR}/lib ${CISSTNETLIB_DIR})
  FIND_LIBRARY(CISSTNETLIB_F95_LIBRARIES cisstNetlibF95 ${CISSTNETLIB_DIR}/lib ${CISSTNETLIB_DIR})
  IF(WIN32)
    FIND_LIBRARY(CISSTNETLIB_GCC_LIBRARIES cisstNetlibGCC ${CISSTNETLIB_DIR}/lib ${CISSTNETLIB_DIR})
  ENDIF(WIN32)
ENDIF(CISSTNETLIB_DIR)
