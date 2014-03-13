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

if( UNIX )

  # set the search paths
  set( XENOMAI_SEARCH_PATH /usr/local/xenomai /usr/xenomai /usr/include/xenomai)
  
  # find xeno-config.h
  find_path( XENOMAI_DIR
    NAMES include/xeno_config.h xeno_config.h
    PATHS ${XENOMAI_SEARCH_PATH} )

  # did we find xeno_config.h?
  if( XENOMAI_DIR ) 
    MESSAGE(STATUS "xenomai found: \"${XENOMAI_DIR}\"")
    
    # set the include directory
    if( "${XENOMAI_DIR}" MATCHES "/usr/include/xenomai" )
      # on ubuntu linux, xenomai install is not rooted to a single dir
      set( XENOMAI_INCLUDE_DIR ${XENOMAI_DIR} )
      set( XENOMAI_INCLUDE_POSIX_DIR ${XENOMAI_DIR}/posix )
    else( "${XENOMAI_DIR}" MATCHES "/usr/include/xenomai")
      # elsewhere, xenomai install is packaged
      set( XENOMAI_INCLUDE_DIR ${XENOMAI_DIR}/include )
      set( XENOMAI_INCLUDE_POSIX_DIR ${XENOMAI_DIR}/include/posix )
    endif( "${XENOMAI_DIR}" MATCHES "/usr/include/xenomai")
    

    # find the xenomai library
    find_library( XENOMAI_LIBRARY_NATIVE  native  ${XENOMAI_DIR}/lib )
    find_library( XENOMAI_LIBRARY_XENOMAI xenomai ${XENOMAI_DIR}/lib )

    # add compile/preprocess options
    set(XENOMAI_DEFINITIONS "-D_GNU_SOURCE -D_REENTRANT -Wall -pipe -D__XENO__")

  else( XENOMAI_DIR )
    MESSAGE(STATUS "xenomai NOT found. (${XENOMAI_SEARCH_PATH})")
  endif( XENOMAI_DIR )

endif( UNIX )

