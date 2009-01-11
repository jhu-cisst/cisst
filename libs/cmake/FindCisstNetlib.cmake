#
# $Id: FindCisstNetlib.cmake 8 2009-01-04 21:13:48Z adeguet1 $
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
# $Id: FindCisstNetlib.cmake 8 2009-01-04 21:13:48Z adeguet1 $
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

#
# $Log: FindCisstNetlib.cmake,v $
# Revision 1.5  2007/04/26 19:33:56  anton
# All files in libraries: Applied new license text, separate copyright and
# updated dates, added standard header where missing.
#
# Revision 1.4  2006/11/20 20:33:19  anton
# Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
# cisstInteractive, cisstImage and cisstOSAbstraction.
#
# Revision 1.3  2005/11/30 16:38:50  anton
# CMakeLists: Added support for libcisstNetlibGCC on Windows.
#
# Revision 1.2  2005/11/29 03:00:39  anton
# FindCisstNetlib.cmake: *: cisstNetlib was spelled cisstnetlib *: rename
# lib Fortran 95 to find the one provided with cisstNetlib.
#
# Revision 1.1  2005/11/20 21:36:45  kapoor
# LAPACK: Added CISST_HAS_CISSTCNETLIB flag to cmake, which can be used to
# Compile code using the newer LAPACK3E. CLAPACK is still supported. See ticket #193.
#
#
#
