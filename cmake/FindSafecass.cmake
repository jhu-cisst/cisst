#
# Author(s):  Min Yang Jung
# Created on: 2012-07-14
#
# (C) Copyright 2012-2015 Johns Hopkins University (JHU), All Rights
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
# FindSafecass.cmake
#
# Looks for SAFECASS, loads its configuartions, and defines 
# following cmake variables:
#
#   SC_FOUND
#   SC_CONFIG_FILE
#   SC_LIBRARY_FILE
#

find_file(SC_CONFIG_FILE SCConfig.cmake NO_DEFAULT_PATH 
          DOC "Path to SCConfig.cmake file in the build tree of SAFECASS")
find_library(SC_LIBRARY_FILE SCLib NO_DEFAULT_PATH
             DOC "Path to SAFECASS library (SCLib)")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SC DEFAULT_MSG SC_CONFIG_FILE SC_LIBRARY_FILE)

if (SC_FOUND)
    # MJ: PARENT_SCOPE appears to be redundant here
    #set (SC_CONFIG_FILE ${SC_CONFIG_FILE} PARENT_SCOPE)
    #set (SC_LIBRARY_FILE ${SC_LIBRARY_FILE} PARENT_SCOPE)
    include(${SC_CONFIG_FILE})
endif(SC_FOUND)
