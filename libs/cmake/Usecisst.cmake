#
# $Id$
#
# Author(s):  Anton Deguet
# Created on: 2010-08-11
#
# (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
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

# Extend CMake Module Path to find cisst defined Macros
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CISST_CMAKE_DIR})
include (cisstMacros)

# Add the include and lib paths for cisst
include_directories (${CISST_INCLUDE_DIR})
link_directories (${CISST_LIBRARY_DIR})
