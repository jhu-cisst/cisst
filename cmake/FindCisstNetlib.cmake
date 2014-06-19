#
# $Id$
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
# CISSTNETLIB_LIBRARIES       - link against these to use CISSTNETLIB (fullpath)
#
# $Id$
#

# try to find automatically
set (CISSTNETLIB_SEARCH_PATH
  /usr
  /usr/local
)

find_path (CISSTNETLIB_DIR
           include/cisstNetlib.h ${CISSTNETLIB_SEARCH_PATH})

# offer to download if not found
if (NOT CISSTNETLIB_DIR)

  mark_as_advanced (CLEAR CISSTNETLIB_DIR)
  option (CISSTNETLIB_DOWNLOAD_NOW "Download cisstNetlib and install in your build tree" OFF)
  if (CISSTNETLIB_DOWNLOAD_NOW)
    # Darwin
    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
      set (CISSTNETLIB_DOWNLOAD_PREFIX "cisstNetlib-2.0-MacOS-intel")
    endif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # Linux
    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
      set (CISSTNETLIB_DOWNLOAD_ARCHITECTURE "undefined" CACHE STRING "Choose \"i686\" for 32 bits Intel or \"x86_64\" for 64 bits Intel")
      if (${CISSTNETLIB_DOWNLOAD_ARCHITECTURE} MATCHES "i686")
        set (CISSTNETLIB_DOWNLOAD_PREFIX "cisstNetlib-Linux-i686-2007-04-09")
      else (${CISSTNETLIB_DOWNLOAD_ARCHITECTURE} MATCHES "i686")
        if (${CISSTNETLIB_DOWNLOAD_ARCHITECTURE} MATCHES "x86_64")
          # set (CISSTNETLIB_DOWNLOAD_PREFIX "cisstNetlib-Linux-x86_64-2007-04-09")
          set (CISSTNETLIB_DOWNLOAD_PREFIX "cisstNetlib-Fortran-3.0.0-Linux-x86_64")
        else (${CISSTNETLIB_DOWNLOAD_ARCHITECTURE} MATCHES "x86_64")
	  message (SEND_ERROR "CISSTNETLIB_DOWNLOAD_ARCHITECTURE must be either \"i686\" or \"x86_64\", not ${CISSTNETLIB_DOWNLOAD_ARCHITECTURE}")
          set (CISSTNETLIB_DOWNLOAD_ARCHITECTURE "undefined" CACHE STRING "Choose \"i686\" for 32 bits Intel or \"x86_64\" for 64 bits Intel" FORCE)
	  set (CISSTNETLIB_DOWNLOAD_PREFIX "")
        endif (${CISSTNETLIB_DOWNLOAD_ARCHITECTURE} MATCHES "x86_64")
      endif (${CISSTNETLIB_DOWNLOAD_ARCHITECTURE} MATCHES "i686")
    endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Windows
    if (WIN32)
      if (CISST_CMAKE_COMPILER_IS_MSVC_64)
        set (CISSTNETLIB_DOWNLOAD_PREFIX "cisstNetlib-C-3.0.0-Windows-x86_64")
      else (CISST_CMAKE_COMPILER_IS_MSVC_64)
        set (CISSTNETLIB_DOWNLOAD_PREFIX "cisstNetlib-Windows-i686-2007-04-09")
      endif (CISST_CMAKE_COMPILER_IS_MSVC_64)
    endif (WIN32)

    # download files once prefix is set
    if (NOT CISSTNETLIB_DOWNLOAD_PREFIX STREQUAL "")
      # download tgz file
      message ("Downloading: http://unittest.lcsr.jhu.edu/cisst/downloads/cisstNetlib/uncompressed/${CISSTNETLIB_DOWNLOAD_PREFIX}.tgz")
      file (DOWNLOAD
            http://unittest.lcsr.jhu.edu/cisst/downloads/cisstNetlib/uncompressed/${CISSTNETLIB_DOWNLOAD_PREFIX}.tgz
            "${CMAKE_CURRENT_BINARY_DIR}/${CISSTNETLIB_DOWNLOAD_PREFIX}.tgz"
            LOG log
            SHOWPROGRESS)
      message ("Downloading cisstNetlib result:" ${log})
      # uncompress using cmake -E tar
      execute_process (COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_CURRENT_BINARY_DIR}/${CISSTNETLIB_DOWNLOAD_PREFIX}.tgz"
                       WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
      # turn download option off as we just downloaded it
      set (CISSTNETLIB_DOWNLOAD_NOW OFF CACHE BOOL "Download cisstNetlib and install in your build tree" FORCE)
      # hide options not used anymore
      mark_as_advanced (CISSTNETLIB_DOWNLOAD_NOW CISSTNETLIB_DOWNLOAD_ARCHITECTURE CISSTNETLIB_DIR)
      # tell where to find the newly downloaded cisstNetlib
      set (CISSTNETLIB_DIR "${CMAKE_CURRENT_BINARY_DIR}/${CISSTNETLIB_DOWNLOAD_PREFIX}" CACHE PATH "Path were cisstNetlib has been downloaded" FORCE)
      message ("You need to configure once more to use the newly downloaded cisstNetlib")
    endif (NOT CISSTNETLIB_DOWNLOAD_PREFIX STREQUAL "")

  endif (CISSTNETLIB_DOWNLOAD_NOW)
endif (NOT CISSTNETLIB_DIR)

# when directory is defined, find libraries
if (CISSTNETLIB_DIR)

  mark_as_advanced (FORCE CISSTNETLIB_DIR)
  mark_as_advanced (CISSTNETLIB_DOWNLOAD_NOW) # hide this option since we have found cisstNetlib
  set (CISSTNETLIB_CONFIG_FILE "${CISSTNETLIB_DIR}/cmake/CisstNetlibConfig.cmake")
  if (EXISTS ${CISSTNETLIB_CONFIG_FILE})
    include (${CISSTNETLIB_CONFIG_FILE})
  else (EXISTS ${CISSTNETLIB_CONFIG_FILE})
    set (CISSTNETLIB_INCLUDE_DIR "${CISSTNETLIB_DIR}/include")
    find_library (CISSTNETLIB_LIBRARY cisstNetlib "${CISSTNETLIB_DIR}/lib" "${CISSTNETLIB_DIR}")
    find_library (CISSTNETLIB_G95_LIBRARY
                  NAMES cisstNetlibgfortran cisstNetlibF95
                  PATHS "${CISSTNETLIB_DIR}/lib" "${CISSTNETLIB_DIR}")
    set (CISSTNETLIB_LIBRARIES ${CISSTNETLIB_LIBRARY} ${CISSTNETLIB_G95_LIBRARY})
    mark_as_advanced (CISSTNETLIB_LIBRARY CISSTNETLIB_G95_LIBRARY)
    if (WIN32)
      find_library (CISSTNETLIB_GCC_LIBRARY cisstNetlibGCC "${CISSTNETLIB_DIR}/lib" "${CISSTNETLIB_DIR}")
      set (CISSTNETLIB_LIBRARIES ${CISSTNETLIB_LIBRARIES} ${CISSTNETLIB_GCC_LIBRARY})
      mark_as_advanced (CISSTNETLIB_GCC_LIBRARY)
    endif (WIN32)
  endif (EXISTS ${CISSTNETLIB_CONFIG_FILE})

endif (CISSTNETLIB_DIR)


