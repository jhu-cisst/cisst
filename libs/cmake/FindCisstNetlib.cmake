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
# CISSTNETLIB_LIBRARIES       - link against these to use CISSTNETLIB (fullpath)
#
# $Id$
#

set(CISSTNETLIB_SEARCH_PATH
  /usr
  /usr/local
  /home/erc/cisstNetlib
)

find_path(CISSTNETLIB_DIR
          include/cisstNetlib.h ${CISSTNETLIB_SEARCH_PATH})

if(CISSTNETLIB_DIR) 
  MARK_AS_ADVANCED(FORCE CISSTNETLIB_DIR) # Balazs 7/17/2009
  set(CISSTNETLIB_INCLUDE_DIR ${CISSTNETLIB_DIR}/include)
  find_library(CISSTNETLIB_LIBRARY cisstNetlib ${CISSTNETLIB_DIR}/lib ${CISSTNETLIB_DIR})
  find_library(CISSTNETLIB_F95_LIBRARY cisstNetlibF95 ${CISSTNETLIB_DIR}/lib ${CISSTNETLIB_DIR})
  set(CISSTNETLIB_LIBRARIES ${CISSTNETLIB_LIBRARY} ${CISSTNETLIB_F95_LIBRARY})
  mark_as_advanced(CISSTNETLIB_LIBRARY CISSTNETLIB_F95_LIBRARY)
  if(WIN32)
    FIND_LIBRARY(CISSTNETLIB_GCC_LIBRARY cisstNetlibGCC ${CISSTNETLIB_DIR}/lib ${CISSTNETLIB_DIR})
    set(CISSTNETLIB_LIBRARIES ${CISSTNETLIB_LIBRARIES} ${CISSTNETLIB_GCC_LIBRARY})
    mark_as_advanced(CISSTNETLIB_GCC_LIBRARY)
  endif(WIN32)
else(CISSTNETLIB_DIR) # Balazs 7/17/2009
  MARK_AS_ADVANCED(CLEAR CISSTNETLIB_DIR) # Balazs 7/17/2009
endif(CISSTNETLIB_DIR)


