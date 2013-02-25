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
# Looks for the Safety Framework, loads its configuartions, and defines 
# following cmake variables:
#
#   SF_FOUND
#   SF_CONFIG_FILE
#   SF_LIBRARY_FILE
#

find_file(SF_CONFIG_FILE SFConfig.cmake NO_DEFAULT_PATH 
          DOC "Path to SFConfig.cmake file in the build tree of Safety Framework")
find_library(SF_LIBRARY_FILE SFLib NO_DEFAULT_PATH
             DOC "Path to Safety Framework library (SFLib)")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SF DEFAULT_MSG SF_CONFIG_FILE SF_LIBRARY_FILE)

if (SF_FOUND)
    # MJ: PARENT_SCOPE appears to be redundant here
    #set (SF_CONFIG_FILE ${SF_CONFIG_FILE} PARENT_SCOPE)
    #set (SF_LIBRARY_FILE ${SF_LIBRARY_FILE} PARENT_SCOPE)
    include(${SF_CONFIG_FILE})
endif(SF_FOUND)
