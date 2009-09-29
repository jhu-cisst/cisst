# Locate Ice home

# This module defines the following variables:
# ICE_FOUND : 1 if Ice is found, 0 otherwise
# ICE_HOME  : path where to find include, lib, bin, etc.
# ICE_INCLUDE_DIR
# ICE_LIBRARY_DIR 
# ICE_SLICE_DIR
#
#

#
# Ice for C++
#

# Assumption: we look for Ice.h and assume that the rest is there.
# i.e. slice2cpp, libIce.so, etc.
# to be more robust we can look for all of those things individually.

# start with 'not found'
SET( ICE_FOUND 0 CACHE BOOL "Do we have Ice?" )

FIND_PATH( ICE_HOME_INCLUDE_ICE Ice.h
  # rational for this search order:
  #    source install w/env.var -> source install
  #    package -> package
  #    package + source install w/env.var -> source install
  #    package + source install w/out env.var -> package 
  #
  # installation selected by user
  ${ICE_HOME}/include/Ice
  $ENV{ICE_HOME}/include/Ice
  # debian package installs Ice here
  /usr/include/Ice
  # MacPort
  /opt/local/include/Ice
  # Test standard installation points: generic symlinks first, then standard dirs, newer first
  /opt/Ice/include/Ice
  /opt/Ice-4/include/Ice
  /opt/Ice-4.0/include/Ice
  /opt/Ice-3/include/Ice
  /opt/Ice-3.5/include/Ice
  /opt/Ice-3.4/include/Ice
  /opt/Ice-3.3/include/Ice
  # some people may manually choose to install Ice here
  /usr/local/include/Ice
  # windows
  C:/Ice-3.4.0-VC80/include/Ice
  C:/Ice-3.4.0/include/Ice
  C:/Ice-3.3.0-VC80/include/Ice
  C:/Ice-3.3.0/include/Ice
  )
# MESSAGE( STATUS "DEBUG: Ice.h is apparently found in : ${ICE_HOME_INCLUDE_ICE}" )

# NOTE: if ICE_HOME_INCLUDE_ICE is set to *-NOTFOUND it will evaluate to FALSE

IF( ICE_HOME_INCLUDE_ICE )

    SET( ICE_FOUND 1 CACHE BOOL "Do we have Ice?" FORCE )

    # strip 'file' twice to get rid off 'include/Ice'
#     MESSAGE( STATUS "DEBUG: ICE_HOME_INCLUDE_ICE=" ${ICE_HOME_INCLUDE_ICE} )
    GET_FILENAME_COMPONENT( ICE_HOME_INCLUDE ${ICE_HOME_INCLUDE_ICE} PATH )
    #MESSAGE( STATUS "DEBUG: ICE_HOME_INCLUDE=" ${ICE_HOME_INCLUDE} )
    GET_FILENAME_COMPONENT( ICE_HOME ${ICE_HOME_INCLUDE} PATH CACHE )

    MESSAGE( STATUS "Setting ICE_HOME to ${ICE_HOME}" )

    # include and lib dirs are easy
    SET( ICE_INCLUDE_DIR ${ICE_HOME}/include ${ICE_HOME}/share/ice/slice )
    SET( ICE_LIBRARY_DIR ${ICE_HOME}/lib )
    
    # debian package splits off slice files into a different place
    IF( ICE_HOME MATCHES /usr )
        SET( ICE_SLICE_DIR /usr/share/slice )
#         MESSAGE( STATUS "This is a Debian Ice installation. Slice files are in ${ice_slice_dir}" )
    ELSE ( ICE_HOME MATCHES /usr )
        SET( ICE_SLICE_DIR ${ICE_HOME}/slice )
#         MESSAGE( STATUS "This is NOT a Debian Ice installation. Slice files are in ${ice_slice_dir}" )
    ENDIF( ICE_HOME MATCHES /usr )

    # some libs only care about IceUtil, we tell them to find IceUtil in the same place as Ice.
    SET( ICEUTIL_HOME ${ICE_HOME} )
    MESSAGE( STATUS "Setting ICEUTIL_HOME to ${ICEUTIL_HOME}" )
    MESSAGE( STATUS "Setting ICE_LIBRARY_DIR to ${ICE_LIBRARY_DIR}" )

    # try to figure if the ice library is libIce or libZeroCIce on Mac OS with MacPort
    if(APPLE)
        find_library(ICE_LIBRARY_NAME_ZEROC_ICE NAMES ZeroCIce PATHS ${ICE_LIBRARY_DIR} NO_DEFAULT_PATH)
	if(ICE_LIBRARY_NAME_ZEROC_ICE)
            set(ICE_LIBRARY_NAME ZeroCIce)
	else(ICE_LIBRARY_NAME_ZEROC_ICE)
	    set(ICE_LIBRARY_NAME Ice)
	endif(ICE_LIBRARY_NAME_ZEROC_ICE)
    else(APPLE)
        set(ICE_LIBRARY_NAME Ice)
    endif(APPLE)
    message(STATUS "Ice library name is ${ICE_LIBRARY_NAME}")

ENDIF( ICE_HOME_INCLUDE_ICE )