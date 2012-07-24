#
# $Id $
#
# Author(s):  Min Yang Jung
# Created on: 2012-07-14
#
# (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
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
# FindSafetyFramework.cmake
#
# Looks for the Safety Framework and defines following cmake variables:
#
#   SF_FOUND
#   SF_INCLUDE_DIR
#   SF_INCLUDE_DIRS
#   SF_LIBRARIES
#

find_path(SF_INCLUDE_DIR NAMES /usr/local/include DOC "Location of libs folder of the Safety Framework")
find_library(SF_LIBRARY NAMES SFLib DOC "Path to Safety Framework library")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SF DEFAULT_MSG SF_LIBRARY SF_INCLUDE_DIR)

if(SF_FOUND)
  set(SF_LIBRARIES ${SF_LIBRARY})
  set(SF_INCLUDE_DIR ${SF_INCLUDE_DIR})
  set(SF_INCLUDE_DIRS ${SF_INCLUDE_DIR}
                      ${SF_INCLUDE_DIR}/FDD 
                      ${SF_INCLUDE_DIR}/adapters/cisst
                      ${SF_INCLUDE_DIR}/common
                      ${SF_INCLUDE_DIR}/monitor)
endif(SF_FOUND)
