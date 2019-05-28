#
#
# (C) Copyright 2005-2013 Johns Hopkins University (JHU), All Rights
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

# Determine 32 or 64 bit
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  set (CISST_MTS_64BIT TRUE)
else (CMAKE_SIZEOF_VOID_P EQUAL 8)
  set (CISST_MTS_64BIT FALSE)
endif (CMAKE_SIZEOF_VOID_P EQUAL 8)

# New features (use with caution)
option (CISST_MTS_NEW "Enable new cisstMultiTask features" OFF)
mark_as_advanced (CISST_MTS_NEW)

# ZeroC ICE
option (CISST_MTS_HAS_ICE "Enable inter-process communication (IPC) feature of cisstMultiTask, requires ICE from ZeroC" OFF)
mark_as_advanced (CISST_MTS_HAS_ICE)

if (CISST_MTS_HAS_ICE)
  if (CISST_HAS_IOS)
    find_package (IceTouch REQUIRED)
    cisst_set_package_settings (cisstMultiTask ZeroCIce INCLUDE_DIRECTORIES ICE_INCLUDE_DIR)
    cisst_set_package_settings (cisstMultiTask ZeroCIce LINK_DIRECTORIES ICE_LIBRARY_DIR)
    set (ICE_LIBRARIES ${ICE_LIBRARY_NAME})
    cisst_set_package_settings (cisstMultiTask ZeroCIce LIBRARIES ICE_LIBRARIES)
  else (CISST_HAS_IOS)
    find_package (Ice REQUIRED)
    cisst_set_package_settings (cisstMultiTask ZeroCIce INCLUDE_DIRECTORIES ICE_INCLUDE_DIR)
    cisst_set_package_settings (cisstMultiTask ZeroCIce LINK_DIRECTORIES ICE_LIBRARY_DIR)
    if (WIN32)
      set (ICE_LIBRARIES_ALL optimized Ice  optimized IceUtil
                             debug     Iced debug     IceUtild)
      cisst_set_package_settings (cisstMultiTask ZeroCIce LIBRARIES ICE_LIBRARIES_ALL)
    else (WIN32)
      set (ICE_LIBRARIES ${ICE_LIBRARY_NAME} ${ICEUTIL_LIBRARY_NAME})
      cisst_set_package_settings (cisstMultiTask ZeroCIce LIBRARIES ICE_LIBRARIES)
    endif (WIN32)
  endif (CISST_HAS_IOS)
else (CISST_MTS_HAS_ICE)
  cisst_unset_all_package_settings (cisstMultiTask ZeroCIce)
endif (CISST_MTS_HAS_ICE)

add_subdirectory (code)

# Applications
add_subdirectory (applications)

# SWIG Python wrappers and tests
cisst_offer_python (cisstMultiTask)
cisst_offer_tests_python (cisstMultiTask)

if (CISST_HAS_QT)
  add_subdirectory (codeQt)
endif (CISST_HAS_QT)

# build examples and tests, add an option to skip them
cisst_offer_examples (cisstMultiTask)
cisst_offer_tests (cisstMultiTask)

