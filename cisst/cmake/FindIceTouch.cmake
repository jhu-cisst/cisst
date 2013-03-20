#
# $Id $
#
# Author(s):  Min Yang Jung, Anton Deguet
# Created on: 2012
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

# Locate Ice home

# This module defines the following variables:
# ICE_FOUND : YES if Ice is found, NO otherwise
# ICE_HOME  : path where to find include, lib, bin, etc.
# ICE_INCLUDE_DIR
# ICE_LIBRARY_DIR
# ICE_SLICE_DIR
# ICE_SLICE2CPP
#

#
# IceTouch for C++
#


# Assumption: we look for Ice/Ice.h and assume that the rest is there.
# i.e. slice2cpp, libIce.so, etc.
# to be more robust we can look for all of those things individually.

# start with 'not found'
set (ICE_FOUND "NO" CACHE BOOL "Do we have Ice?" FORCE)

find_path (ICE_ICE_H_INCLUDE_DIR
		   NO_DEFAULT_PATH 
           NAMES Ice/Ice.h
           PATHS
             # installation selected by user
             ${ICE_HOME}/include
             $ENV{ICE_HOME}/include
             /Library/Developer/IceTouch-1.2.1/SDKs/Cpp/iphoneos.sdk/usr/local/include
            )

# NOTE: if ICE_HOME_INCLUDE_ICE is set to *-NOTFOUND it will evaluate to FALSE
if (ICE_ICE_H_INCLUDE_DIR)

  get_filename_component (ICE_HOME_STRING ${ICE_ICE_H_INCLUDE_DIR} PATH)
  set (ICE_HOME ${ICE_HOME_STRING} CACHE PATH "Ice home directory")

 

  # include and lib dirs are easy
  set (ICE_INCLUDE_DIR
       ${ICE_ICE_H_INCLUDE_DIR}
       )

  set (ICE_LIBRARY_DIR ${ICE_HOME}/lib)
  
  find_path (ICE_SLICE_DIR
             NAMES Ice/Identity.ice
             NO_DEFAULT_PATH
             PATHS 
               ${ICE_HOME}/slice
               $ENV{ICE_HOME}/slice
               /Library/Developer/IceTouch-1.2.1/SDKs/Cpp/slice
               )
               
  get_filename_component (ICE_SLICE_HOME_STRING ${ICE_SLICE_DIR} PATH)
  set (ICE_SLICE_HOME ${ICE_SLICE_HOME_STRING} CACHE PATH "Ice Slice home directory")
          
  set (ICE_LIBRARY_NAME IceCpp)
  message (STATUS "Setting ICE_SLICE_HOME to ${ICE_SLICE_HOME}")

  # find slice2cpp
  find_program (ICE_SLICE2CPP
                NAME slice2cpp
                PATHS "${ICE_SLICE_HOME}/bin"
                NO_DEFAULT_PATH)

  # set ICE_FOUND if enough is found
  if (ICE_HOME AND ICE_LIBRARY_NAME AND ICE_SLICE2CPP)
    set (ICE_FOUND "YES" CACHE BOOL "Do we have Ice?" FORCE)
  endif (ICE_HOME AND ICE_LIBRARY_NAME AND ICE_SLICE2CPP)

  # quiet things down a bit
  if (ICE_FOUND)
    mark_as_advanced (ICE_FOUND ICE_HOME
                      ICE_INCLUDE_DIR ICE_ICE_H_INCLUDE_DIR ICE_SLICE_DIR
                      ICE_LIBRARY_NAME ICE_SLICE2CPP)
  endif (ICE_FOUND)

endif (ICE_ICE_H_INCLUDE_DIR)
