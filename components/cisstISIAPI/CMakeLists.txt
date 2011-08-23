#
# $Id$
#
# Author(s):  Anton Deguet
# Created on: 2011-01-20
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

# To find isi_api-config.cmake
find_package (isi_api REQUIRED)

if (isi_api_FOUND)
  include_directories (${CMAKE_CURRENT_BINARY_DIR} ${isi_api_INCLUDE_DIR})
  add_subdirectory (code)
  add_subdirectory (examples)
endif (isi_api_FOUND)
