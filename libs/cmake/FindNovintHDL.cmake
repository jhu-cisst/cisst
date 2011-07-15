#
# $Id$
#
# Author(s):  Anton Deguet
# Created on: 2008-04-04
#
# (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
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
# Try to find NOVINT HDL library (aka 3DTouch)
#
# The following values are defined
#
# NOVINT_INCLUDE_DIR          - include directories to use
# NOVINT_LIBRARIES            - libraries needed
#
# Important notes:
# - Implemented for Windows only.

if (WIN32)
  set (NOVINT_SDK_DIR "C:/Program Files/Novint/HDAL_SDK_2.1.3")

  find_library (NOVINT_LIB_HDL hdl ${NOVINT_SDK_DIR}/lib)

  set (NOVINT_INCLUDE_DIR ${NOVINT_SDK_DIR}/include)

  set (NOVINT_LIBRARIES
       ${NOVINT_LIB_HDL})

  file (TO_CMAKE_PATH "${NOVINT_INCLUDE_DIR}" NOVINT_INCLUDE_DIR)
  file (TO_CMAKE_PATH "${NOVINT_LIBRARIES}" NOVINT_LIBRARIES)

  mark_as_advanced (NOVINT_LIB_HDL
                    NOVINT_INCLUDE_DIR
                    NOVINT_SDK_DIR
                    NOVINT_LIBRARIES)

endif (WIN32)
