#
# $Id$
#
# Author(s):  Anton Deguet
# Created on: 2004-02-18
#
# (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights
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
# try to find RTAI on UNIX systems.
#
# The following values are defined
#
# RTAI_INCLUDE_DIR     - include directories to use RTAI
# RTAI_LIBRARIES       - link against this to use RTAI (fullpath)
#

IF (UNIX)
  SET(RTAI_SEARCH_PATH
    /usr
    /usr/local
    /usr/realtime
  )

  FIND_PATH(RTAI_DIR
            include/rtai.h ${RTAI_SEARCH_PATH})

  IF(RTAI_DIR) 
    SET(RTAI_INCLUDE_DIR ${RTAI_DIR}/include)
    FIND_LIBRARY(RTAI_LIBRARIES lxrt ${RTAI_DIR}/lib)
  ENDIF(RTAI_DIR)

ENDIF (UNIX)

