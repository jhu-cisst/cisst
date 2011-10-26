#
# $Id$
#
# (C) Copyright 2005-2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# All cisstMultiTask libraries
project (cisstMultiTaskLibs)

# try to find ZeroC Ice
find_package (Ice)
if (ICE_FOUND)
  option (CISST_MTS_HAS_ICE "Enable inter-process communication (IPC) feature of cisstMultiTask, requires ICE from ZeroC" ON)
  mark_as_advanced (CISST_MTS_HAS_ICE)
else (ICE_FOUND)
  option (CISST_MTS_HAS_ICE "Enable inter-process communication (IPC) feature of cisstMultiTask, requires ICE from ZeroC" OFF)
endif (ICE_FOUND)

if (CISST_MTS_HAS_ICE)
  find_package (Ice REQUIRED)
  cisst_set_package_settings (cisstMultiTask ZeroCIce INCLUDE_DIRECTORIES ICE_INCLUDE_DIR)
  cisst_set_package_settings (cisstMultiTask ZeroCIce LINK_DIRECTORIES ICE_LIBRARY_DIR)
  if (WIN32)
    set (ICE_LIBRARIES_ALL optimized Ice  optimized IceUtil
                           debug     Iced debug     IceUtild)
    cisst_set_package_settings (cisstMultiTask ZeroCIce LIBRARIES ICE_LIBRARIES_ALL)
  else (WIN32)
    set (ICE_LIBRARIES ${ICE_LIBRARY_NAME} IceUtil)
    cisst_set_package_settings (cisstMultiTask ZeroCIce LIBRARIES ICE_LIBRARIES)
  endif (WIN32)
else (CISST_MTS_HAS_ICE)
  cisst_unset_all_package_settings (cisstMultiTask ZeroCIce)
endif (CISST_MTS_HAS_ICE)

add_subdirectory (code)

# Applications
add_subdirectory (applications)

# SWIG Python wrappers and tests
if (CISST_HAS_SWIG_PYTHON)
  add_subdirectory (codePython)
  if (CISST_BUILD_TESTS)
    add_subdirectory (testsPython)
  endif (CISST_BUILD_TESTS)
endif (CISST_HAS_SWIG_PYTHON)

if (CISST_HAS_QT)
  add_subdirectory (codeQt)
endif (CISST_HAS_QT)

# build examples and tests, add an option to skip them
set (EXTRAS EXAMPLES TESTS)
foreach (extra ${EXTRAS})
  string (TOLOWER ${extra} extra_lower)
  if (CISST_BUILD_${extra})
    option (CISST_cisstMultiTask_${extra} "Build ${extra_lower} for cisstMultiTask." ON)
    mark_as_advanced (CISST_cisstMultiTask_{extra})
    if (CISST_cisstMultiTask_${extra})
      add_subdirectory (${extra_lower})
    endif (CISST_cisstMultiTask_${extra})
  endif (CISST_BUILD_${extra})
endforeach (extra)
