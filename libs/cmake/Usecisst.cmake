#
# $Id$
#
# Author(s):  Anton Deguet
# Created on: 2010-08-11
#
# (C) Copyright 2010-2011 Johns Hopkins University (JHU), All Rights
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

# Test if this file has been included within the cisst project itself
if (NOT CURRENT_PROJECT_IS_CISST)

  # Extend CMake Module Path to find cisst defined Macros
  set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CISST_CMAKE_DIR})
  include (cisstMacros)

  # Add the include and lib paths for cisst
  include_directories (${CISST_INCLUDE_DIR})
  link_directories (${CISST_LIBRARY_DIR})

  # Load all settings for external dependencies
  cisst_load_package_setting (${CISST_LIBRARIES_REQUIRED_INTERNAL})

endif (NOT CURRENT_PROJECT_IS_CISST)

# Add link and include directories based on required libraries
cisst_set_directories (${CISST_LIBRARIES_REQUIRED_INTERNAL})

# This macro should be called from external projects that need to
# use SWIG to wrap for Python. It relies on CISST_SWIG_EXECUTABLE
# to be defined in cisst-config.cmake.
# Then, call the cisst_add_swig_module function for each Python module.
macro (cisst_setup_swig_python)
   if (CISST_HAS_SWIG_PYTHON)
     set(SWIG_EXECUTABLE ${CISST_SWIG_EXECUTABLE})
     find_package(SWIG REQUIRED)
     include(${SWIG_USE_FILE})
   else (CISST_HAS_SWIG_PYTHON)
     message(SEND_ERROR "cisst_setup_swig_python requires CISST_HAS_SWIG_PYTHON")
   endif (CISST_HAS_SWIG_PYTHON)
endmacro (cisst_setup_swig_python)

# This macro should be called from external projects that want to
# use the same version of FLTK that cisst was compiled with.
# This assumes that FindFLTK.cmake will work properly if just
# FLTK_INCLUDE_DIR is set (this is the case for CMake 2.6 and 2.8)
macro (cisst_setup_fltk)
   if (CISST_HAS_FLTK)
     set(FLTK_INCLUDE_DIR ${CISST_FLTK_DIR})
     find_package(FLTK REQUIRED)
   else (CISST_HAS_FLTK)
     message(SEND_ERROR "cisst_setup_fltk requires CISST_HAS_FLTK")
   endif (CISST_HAS_FLTK)
endmacro (cisst_setup_fltk)


