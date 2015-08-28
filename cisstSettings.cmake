#
# $Id$
#
# (C) Copyright 2005-2014 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# To allow mixing static and dynamic libraries
if (CMAKE_COMPILER_IS_GNUCXX AND NOT MINGW)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif (CMAKE_COMPILER_IS_GNUCXX AND NOT MINGW)

# To have more warnings (mostly for GCC and CLANG)
if (NOT MSVC)
  include (CheckCXXCompilerFlag)
  check_cxx_compiler_flag ("-Wextra" CXX_SUPPORTS_WEXTRA)
  if (CXX_SUPPORTS_WEXTRA)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
  endif (CXX_SUPPORTS_WEXTRA)

  check_cxx_compiler_flag ("-Wall" CXX_SUPPORTS_WALL)
  if (CXX_SUPPORTS_WALL)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
  endif (CXX_SUPPORTS_WALL)
endif (NOT MSVC)

# To disable annoying warnings about sprintf, sscanf, etc. (Visual Studio)
if (MSVC)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
endif (MSVC)

if(MSVC)
   ADD_DEFINITIONS(-DNOMINMAX)
endif()
