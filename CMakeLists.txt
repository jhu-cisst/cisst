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


# Backward compatibility
cmake_minimum_required (VERSION 2.6.2)

# cisst (Computer Integrated Surgical Systems and Technology)
project (cisst)
set (CISST_BINARY_DIR ${cisst_BINARY_DIR})
set (CISST_SOURCE_DIR ${cisst_SOURCE_DIR})

# Create some variables so that find_package (cisst) can be used in subdirectories (examples)
set (CURRENT_PROJECT_IS_CISST ON)

set (cisst_DIR ${CISST_BINARY_DIR} CACHE PATH "Set path so that find_package (cisst) works in this project")
mark_as_advanced (cisst_DIR)

# Option to make cisst CMake functions more verbose
option (CISST_HAS_CMAKE_DEBUG "Print extra messages when configuring with CMake" OFF)
mark_as_advanced (CISST_HAS_CMAKE_DEBUG)


# Function to expend a list without repetitions, forces the list to be internal
function (CISST_ADD_TO_INTERNAL list newElements)
  message (SEND_ERROR "cisst_add_to_internal is now deprecated, use cisst_set_package_settings instead")
endfunction (CISST_ADD_TO_INTERNAL)

# Where to find cisst defined CMake Macros/Modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${cisst_SOURCE_DIR}/libs/cmake)

include (cisstMacros)

# List all keywords allowed for cisst additional settings
set (CISST_PACKAGE_KEYWORDS INCLUDE_DIRECTORIES LIBRARIES LINK_DIRECTORIES PACKAGES CMAKE_FILES)

# Add dependency information for a given package used by cisst
# Usage example: cisst_set_package_settings (cisstCommon FLTK INCLUDE_DIRECTORIES FLTK_INCLUDE_DIR)
function (cisst_set_package_settings whoRequires package variable value)
  # todo, do not replicate lib debug/optimized (used for path?)
  # todo, can we check whoRequires to be a valid library?

  # make sure the variable is allowed
  list (FIND CISST_PACKAGE_KEYWORDS ${variable} VARIABLE_IS_A_KEYWORD)
  if (NOT ${VARIABLE_IS_A_KEYWORD} GREATER -1)
    message (SEND_ERROR "cisst_set_package_settings: third argument (${variable}) should be one of: ${CISST_PACKAGE_KEYWORDS}")
  endif (NOT ${VARIABLE_IS_A_KEYWORD} GREATER -1)
  # make sure a value has been provided
  if (${value})

    cisst_load_package_setting (${whoRequires})
    # Add to list of all external dependencies
    set (CISST_EXTERNAL_PACKAGES_FOR_${whoRequires} ${CISST_EXTERNAL_PACKAGES_FOR_${whoRequires}} ${package})
    list (REMOVE_DUPLICATES CISST_EXTERNAL_PACKAGES_FOR_${whoRequires})
    # Full variable name
    set (FULL_VARIABLE_NAME CISST_${variable}_FOR_${whoRequires}_USING_${package})
    # Keep previous value to compare if there is anything new
    set (OLD_VALUE ${${FULL_VARIABLE_NAME}})
    set (${FULL_VARIABLE_NAME} "${${value}}")
    # Save to file if needed
    if (NOT "${OLD_VALUE}" STREQUAL "${${FULL_VARIABLE_NAME}}")
      set (FILENAME "${cisst_BINARY_DIR}/cisst-dependencies/${whoRequires}${package}.cmake")
      file (WRITE  ${FILENAME} "# This file is generated automatically by CMake for cisst, DO NOT EDIT\n")
      file (APPEND ${FILENAME} "# Source: ${CMAKE_SOURCE_DIR}\n\n")
      # dump all in a config file, over and over
      foreach (keyword ${CISST_PACKAGE_KEYWORDS})
        set (FULL_VARIABLE_NAME CISST_${keyword}_FOR_${whoRequires}_USING_${package})
        if (${FULL_VARIABLE_NAME})
          file (APPEND ${FILENAME} "set (${FULL_VARIABLE_NAME} \"${${FULL_VARIABLE_NAME}}\")\n")
        endif (${FULL_VARIABLE_NAME})
      endforeach (keyword ${CISST_PACKAGE_KEYWORDS})
    endif (NOT "${OLD_VALUE}" STREQUAL "${${FULL_VARIABLE_NAME}}")
    # Also update the list of external packages for the given library
    set (FILENAME "${cisst_BINARY_DIR}/cisst-dependencies/${whoRequires}External.cmake")
    file (WRITE  ${FILENAME} "# This file is generated automatically by CMake for cisst, DO NOT EDIT\n")
    file (APPEND ${FILENAME} "# Source: ${CMAKE_SOURCE_DIR}\n\n")
    file (APPEND ${FILENAME} "set (CISST_EXTERNAL_PACKAGES_FOR_${whoRequires} \"${CISST_EXTERNAL_PACKAGES_FOR_${whoRequires}}\")\n")
  else (${value})
    message (SEND_ERROR "cisst_set_package_settings: value for variable ${variable} provided for package ${package} used by library ${whoRequires} is not properly defined")
  endif (${value})
endfunction (cisst_set_package_settings)


function (cisst_unset_all_package_settings whoRequires package)
  cisst_load_package_setting (${whoRequires})
  if (CISST_EXTERNAL_PACKAGES_FOR_${whoRequires})
    set (PACKAGE_FILE "${CISST_BINARY_DIR}/cisst-dependencies/${whoRequires}${package}.cmake")
    file (REMOVE ${PACKAGE_FILE})
    list (REMOVE_ITEM CISST_EXTERNAL_PACKAGES_FOR_${whoRequires} ${package})
    set (FILENAME "${cisst_BINARY_DIR}/cisst-dependencies/${whoRequires}External.cmake")
    file (REMOVE ${FILENAME})
    if (CISST_EXTERNAL_PACKAGES_FOR_${whoRequires})
      file (WRITE  ${FILENAME} "# This file is generated automatically by CMake for cisst, DO NOT EDIT\n")
      file (APPEND ${FILENAME} "# Source: ${CMAKE_SOURCE_DIR}\n\n")
      file (APPEND ${FILENAME} "set (CISST_EXTERNAL_PACKAGES_FOR_${whoRequires} \"${CISST_EXTERNAL_PACKAGES_FOR_${whoRequires}}\")\n")
    endif (CISST_EXTERNAL_PACKAGES_FOR_${whoRequires})
  endif (CISST_EXTERNAL_PACKAGES_FOR_${whoRequires})
endfunction (cisst_unset_all_package_settings)


# Output directories.
set (LIBRARY_OUTPUT_PATH ${cisst_BINARY_DIR}/libs/lib)
set (EXECUTABLE_OUTPUT_PATH ${cisst_BINARY_DIR}/libs/bin)

# Where to find afterwards
set (cisst_LIBRARY_PATH "${LIBRARY_OUTPUT_PATH}")
set (cisst_EXECUTABLE_PATH "${EXECUTABLE_OUTPUT_PATH}")

# On Windows, CMake uses a different compiler at configuration time
if (CMAKE_CL_64)
  set (CISST_CMAKE_COMPILER_IS_MSVC_64 ON)
else (CMAKE_CL_64)
  set (CISST_CMAKE_COMPILER_IS_MSVC_64 OFF)
endif (CMAKE_CL_64)

# Options
option (CISST_BUILD_UTILITIES "Build utility programs built using cisst." ON)
option (CISST_BUILD_TESTS "Build tests." OFF)
option (CISST_BUILD_EXAMPLES "Build examples." OFF)
option (CISST_BUILD_COMPONENTS "Build components." OFF)

if (UNIX)
  option (CISST_BUILD_DOC "Build the documentation." OFF)
endif (UNIX)

# Build static or dynamic libraries
option (CISST_BUILD_SHARED_LIBS "Build cisst with shared libraries." OFF)
mark_as_advanced (CISST_BUILD_SHARED_LIBS)
if (CISST_BUILD_SHARED_LIBS)
  set (IS_SHARED SHARED)
  set (BUILD_SHARED_LIBS SHARED)
  if (WIN32)
    add_definitions (-DCISST_DLL)
  endif (WIN32)
else (CISST_BUILD_SHARED_LIBS)
  set (IS_SHARED STATIC)
endif (CISST_BUILD_SHARED_LIBS)


# Wrapping with swig for Python and/or Java
option (CISST_HAS_SWIG_PYTHON "Build the Python wrappers for cisst with Swig." OFF)
mark_as_advanced (CISST_HAS_SWIG_PYTHON)
if (CISST_HAS_SWIG_PYTHON)
    # Find Swig
    set (CISST_SWIG_REQUIRED_VERSION 1.3.36)
    find_package (SWIG ${CISST_SWIG_REQUIRED_VERSION} REQUIRED)
    # It looks like version is not yet supported by CMake for SWIG so we are checking manually
    if (${SWIG_VERSION} VERSION_LESS ${CISST_SWIG_REQUIRED_VERSION})
        message ("SWIG version ${SWIG_VERSION} is too low, cisst requires ${CISST_SWIG_REQUIRED_VERSION}")
    else (${SWIG_VERSION} VERSION_LESS ${CISST_SWIG_REQUIRED_VERSION})
        include (${SWIG_USE_FILE})
    endif (${SWIG_VERSION} VERSION_LESS ${CISST_SWIG_REQUIRED_VERSION})
    # Note that SWIG_EXECUTABLE is saved in cisst-config.cmake (cisstBuild.cmake.in) for use
    # by external projects (see Usecisst.cmake).

    # Force to use exceptions
    set (CISST_CMN_THROW_DOES_ABORT OFF CACHE BOOL "cmnThrow uses abort() instead of throw" FORCE)
    set (CISST_CMN_ASSERT_THROWS_EXCEPTION ON CACHE BOOL "CMN_ASSERT throws an exception instead of abort" FORCE)

    # Find Python
    find_package (PythonLibs REQUIRED)
    # Some older CMake don't define the debug library
    if (NOT PYTHON_DEBUG_LIBRARIES)
        set (PYTHON_DEBUG_LIBRARIES ${PYTHON_LIBRARIES})
    endif (NOT PYTHON_DEBUG_LIBRARIES)
    find_package (PythonInterp REQUIRED)
    include_directories (${PYTHON_INCLUDE_PATH})
    cisst_set_package_settings (cisstPython Python INCLUDE_DIRECTORIES PYTHON_INCLUDE_PATH)
    set(PYTHON_ALL_LIBRARIES "debug;${PYTHON_DEBUG_LIBRARIES};optimized;${PYTHON_LIBRARIES}")
    cisst_set_package_settings (cisstPython Python LIBRARIES PYTHON_ALL_LIBRARIES)
    # We also need numpy if we have cisstVector
    if (CISST_BUILD_LIBS_cisstVector)
        find_package (Numpy REQUIRED)
        if (PYTHON_NUMPY_FOUND)
            include_directories (${PYTHON_NUMPY_INCLUDE_DIR})
            cisst_set_package_settings (cisstPython NumPy INCLUDE_DIRECTORIES PYTHON_NUMPY_INCLUDE_DIR)
            mark_as_advanced (PYTHON_NUMPY_INCLUDE_DIR)
        else (PYTHON_NUMPY_FOUND)
            cisst_unset_all_package_settings (cisstPython NumPy)
        endif (PYTHON_NUMPY_FOUND)
    endif (CISST_BUILD_LIBS_cisstVector)
else (CISST_HAS_SWIG_PYTHON)
    cisst_unset_all_package_settings (cisstPython Python)
    cisst_unset_all_package_settings (cisstPython NumPy)
endif (CISST_HAS_SWIG_PYTHON)


# Support for OpenGL
find_package (OpenGL)
if (OPENGL_FOUND)
  option (CISST_HAS_OPENGL "Is OpenGL available." ON)
  mark_as_advanced (CISST_HAS_OPENGL)
else (OPENGL_FOUND)
  option (CISST_HAS_OPENGL "Is OpenGL available." OFF)
endif (OPENGL_FOUND)

if (CISST_HAS_OPENGL)
  find_package (OpenGL REQUIRED)
  if (OPENGL_INCLUDE_DIR)
    cisst_set_package_settings (cisstVector OpenGL INCLUDE_DIRECTORIES OPENGL_INCLUDE_DIR)
  endif (OPENGL_INCLUDE_DIR)
  cisst_set_package_settings (cisstVector OpenGL LIBRARIES OPENGL_LIBRARIES)
else (CISST_HAS_OPENGL)
  cisst_unset_all_package_settings (cisstVector OpenGL)
endif (CISST_HAS_OPENGL)

# Support for QT
set (QT_USE_QTOPENGL ON)
set (QT_USE_QTXML ON)
set (QT_USE_QTXMLPATTERNS ON)
find_package (Qt4)
if (QT_FOUND)
  option (CISST_HAS_QT "Is Qt available.  If not, Qt widgets and some examples won't be compiled" ON)
  mark_as_advanced (CISST_HAS_QT)
else (QT_FOUND)
  option (CISST_HAS_QT "Is Qt available.  If not, Qt widgets and some examples won't be compiled" OFF)
endif (QT_FOUND)

# if detected or if user requests it
if (CISST_HAS_QT)
  # OpenGL
  set (CISST_HAS_OPENGL ON CACHE BOOL "Is OpenGL available." FORCE)
  mark_as_advanced (CISST_HAS_OPENGL)
  # XML Qt based
  option (CISST_HAS_QT_XML "Use Qt XML and Patterns for cmnXMLPath" ON)
  mark_as_advanced (CISST_HAS_QT_XML)
  # Qt is now required
  find_package (Qt4 REQUIRED)
  include (${QT_USE_FILE})
  set (CISST_QT_PACKAGE "Qt4" CACHE STRING "Qt Package Name")
  mark_as_advanced (CISST_QT_PACKAGE)
  cisst_set_package_settings (cisstCommonQt Qt PACKAGES    CISST_QT_PACKAGE)
  cisst_set_package_settings (cisstCommonQt Qt CMAKE_FILES QT_USE_FILE)
  cisst_set_package_settings (cisstCommonQt Qt LIBRARIES   QT_LIBRARIES)
else (CISST_HAS_QT)
  cisst_unset_all_package_settings (cisstCommonQt Qt)
endif (CISST_HAS_QT)

# Support for FLTK
find_package (FLTK)
if (FLTK_FOUND)
  option (CISST_HAS_FLTK "Is FLTK available.  If not, some examples and utilities won't be compiled" ON)
  mark_as_advanced (CISST_HAS_FLTK)
else (FLTK_FOUND)
  option (CISST_HAS_FLTK "Is FLTK available.  If not, some examples and utilities won't be compiled" OFF)
endif (FLTK_FOUND)

if (CISST_HAS_FLTK)
  # OpenGL
  set (CISST_HAS_OPENGL ON CACHE BOOL "Is OpenGL available." FORCE)
  mark_as_advanced (CISST_HAS_OPENGL)
  # FLTK is now required
  find_package(FLTK REQUIRED)
  cisst_set_package_settings (cisstCommon FLTK INCLUDE_DIRECTORIES FLTK_INCLUDE_DIR)
  cisst_set_package_settings (cisstCommon FLTK LIBRARIES           FLTK_LIBRARIES)
  if (FLTK_LIBRARY_DIRS)
    # If FLTK was built using CMake (i.e., there is an FLTKConfig.cmake), the FLTK_LIBRARIES (above) does not include the path,
    # so we need to add FLTK_LIBRARY_DIRS (which is only defined if FLTK was built using CMake) to the link directories.
    cisst_set_package_settings (cisstCommon FLTK LINK_DIRECTORIES  FLTK_LIBRARY_DIRS)
  endif (FLTK_LIBRARY_DIRS)
else (CISST_HAS_FLTK)
  cisst_unset_all_package_settings (cisstCommon FLTK)
endif (CISST_HAS_FLTK)

# Use XML or not, using libxml2 over
find_package (LibXml2)
if (LIBXML2_FOUND AND NOT CISST_HAS_QT_XML)
  option (CISST_HAS_LIBXML2_XML "Use libxml2 for cmnXMLPath" ON)
else (LIBXML2_FOUND AND NOT CISST_HAS_QT_XML)
  option (CISST_HAS_LIBXML2_XML "Use libxml2 for cmnXMLPath" OFF)
endif (LIBXML2_FOUND AND NOT CISST_HAS_QT_XML)
mark_as_advanced (CISST_HAS_LIBXML2_XML)

if (CISST_HAS_LIBXML2_XML)
  find_package (LibXml2 REQUIRED)
  cisst_set_package_settings (cisstCommonXML Libxml2 LIBRARIES LIBXML2_LIBRARIES)
else (CISST_HAS_LIBXML2_XML)
  cisst_unset_all_package_settings (cisstCommonXML libxml2)
endif (CISST_HAS_LIBXML2_XML)

# Safety check
if (CISST_HAS_LIBXML2_XML AND CISST_HAS_QT_XML)
  message (SEND_ERROR "You must choose either CISST_HAS_LIBXML2_XML or CISST_HAS_QT_XML")
endif (CISST_HAS_LIBXML2_XML AND CISST_HAS_QT_XML)

# Do we have some kind of XML?
if (CISST_HAS_LIBXML2_XML OR CISST_HAS_QT_XML)
  set (CISST_HAS_XML ON)
else (CISST_HAS_LIBXML2_XML OR CISST_HAS_QT_XML)
  set (CISST_HAS_XML OFF)
endif (CISST_HAS_LIBXML2_XML OR CISST_HAS_QT_XML)
mark_as_advanced (CISST_HAS_XML)

# Extra link flag if for Apple
if (CISST_HAS_CISSTNETLIB AND APPLE AND CISST_BUILD_SHARED_LIBS)
  set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -single_module")
endif (CISST_HAS_CISSTNETLIB AND APPLE AND CISST_BUILD_SHARED_LIBS)

# Check on Linux only
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

  # Check for RTAI
  option (CISST_HAS_LINUX_RTAI "Use the RTAI real time extensions for Linux." OFF)
  if (CISST_HAS_LINUX_RTAI)
    find_package (RTAI)
    cisst_set_package_settings (cisstCommon RTAI INCLUDE_DIRECTORIES RTAI_INCLUDE_DIR)
    cisst_set_package_settings (cisstCommon RTAI LIBRARIES RTAI_LIBRARIES)
  else (CISST_HAS_LINUX_RTAI)
    cisst_unset_all_package_settings (cisstCommon RTAI)
  endif (CISST_HAS_LINUX_RTAI)

  # Check for Xenomai
  option (CISST_HAS_LINUX_XENOMAI "Use the Xenomai real time extensions for Linux." OFF)
  if (CISST_HAS_LINUX_XENOMAI)
    find_package(XENOMAI)
    # add /usr/xenomai/include/posix. This is a *must* since we want to
    # to use xenomai/include/posix/pthread.h
    set (CISST_XENOMAI_INCLUDE_DIRECTORIES ${XENOMAI_INCLUDE_DIR} ${XENOMAI_INCLUDE_POSIX_DIR})
    cisst_set_package_settings (cisstCommon Xenomai INCLUDE_DIRECTORIES CISST_XENOMAI_INCLUDE_DIRECTORIES)
    # add libnative libxenomai librtdm libpthread_rt
    set (CISST_XENOMAI_LIBRARIES ${XENOMAI_LIBRARY_NATIVE}
                                 ${XENOMAI_LIBRARY_PTHREAD_RT}
                                 ${XENOMAI_LIBRARY_RTDM}
                                 ${XENOMAI_LIBRARY_XENOMAI})
    cisst_set_package_settings (cisstCommon Xenomai LIBRARIES CISST_XENOMAI_LIBRARIES)
    # add -D_GNU_SOURCE -D_REENTRANT -D__XENO__ -pipe
    add_definitions ("${XENOMAI_DEFINITIONS}")
    # add Xenomai wrappers for pthread
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${XENOMAI_EXE_LINKER_FLAGS}")
    set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${XENOMAI_EXE_LINKER_FLAGS}")

  else (CISST_HAS_LINUX_XENOMAI)
    cisst_unset_all_package_settings (cisstCommon Xenomai)
  endif (CISST_HAS_LINUX_XENOMAI)

endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# Determine if cmnThrow uses "throw" or "abort"
option (CISST_CMN_THROW_DOES_ABORT "cmnThrow uses abort() instead of throw" OFF)
mark_as_advanced (CISST_CMN_THROW_DOES_ABORT)
if (CISST_CMN_THROW_DOES_ABORT)
  add_definitions (-DCISST_CMN_THROW_DOES_ABORT)
endif (CISST_CMN_THROW_DOES_ABORT)

# Determine if CMN_ASSERT should be disabled
option (CISST_CMN_ASSERT_DISABLED "CMN_ASSERT is disabled, i.e. does not assert (for release mode)" OFF)
mark_as_advanced (CISST_CMN_ASSERT_DISABLED)
if (CISST_CMN_ASSERT_DISABLED)
  add_definitions (-DCISST_CMN_ASSERT_DISABLED)
endif (CISST_CMN_ASSERT_DISABLED)

# Determine if CMN_ASSERT should throw an exception instead of abort
option (CISST_CMN_ASSERT_THROWS_EXCEPTION "CMN_ASSERT throws an exception instead of abort" OFF)
mark_as_advanced (CISST_CMN_ASSERT_THROWS_EXCEPTION)
if (CISST_CMN_ASSERT_THROWS_EXCEPTION)
  add_definitions (-DCISST_CMN_ASSERT_THROWS_EXCEPTION)
endif (CISST_CMN_ASSERT_THROWS_EXCEPTION)


# Put "." in the rpath of all CISST shared libraries.
link_directories (. ${LIBRARY_OUTPUT_PATH})

# Determine if we want to use rpath, no by default
if (UNIX)
  option (CMAKE_SKIP_RPATH "Do NOT use rpath to hard code the path to dynamic libraries." ON)
  mark_as_advanced (FORCE CMAKE_SKIP_RPATH)
endif (UNIX)


# Add include directories
set (CISST_INCLUDE_DIR
     ${cisst_BINARY_DIR}/libs/include
     ${cisst_SOURCE_DIR}/libs/include)
if (CISST_BUILD_COMPONENTS)
  set (CISST_INCLUDE_DIR ${CISST_INCLUDE_DIR}
                         ${cisst_BINARY_DIR}/components
                         ${cisst_SOURCE_DIR}/components)
endif (CISST_BUILD_COMPONENTS)

include_directories (${CISST_INCLUDE_DIR}) # ${CISST_ADDITIONAL_INCLUDE_DIRECTORIES})

# To allow mixing static and dynamic libraries
if (CMAKE_COMPILER_IS_GNUCXX)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif (CMAKE_COMPILER_IS_GNUCXX)


# To have more warnings with gcc
if (CMAKE_COMPILER_IS_GNUCXX)
  include (CheckCXXCompilerFlag)
  check_cxx_compiler_flag ("-Wextra" CXX_SUPPORTS_WEXTRA)
  if (CXX_SUPPORTS_WEXTRA)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
  endif (CXX_SUPPORTS_WEXTRA)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif (CMAKE_COMPILER_IS_GNUCXX)


# Find native threads
if (NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "QNX")
  find_package (Threads REQUIRED)
  if (CMAKE_THREAD_LIBS_INIT)
    cisst_set_package_settings (cisstCommon Threads LIBRARIES CMAKE_THREAD_LIBS_INIT)
  endif (CMAKE_THREAD_LIBS_INIT)
endif (NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "QNX")

# Which libraries to build
set (CISST_LIBS_STABLE
     cisstCommon
     cisstVector
     cisstOSAbstraction
     cisstMultiTask
     )

set (CISST_LIBS_UNSTABLE
     cisstNumerical
     cisstInteractive
     cisstRobot
     cisstParameterTypes
     cisstStereoVision
     cisstDevices
     cisst3DUserInterface
     )

# All libraries
set (CISST_LIBS
     ${CISST_LIBS_STABLE}
     ${CISST_LIBS_UNSTABLE})

# Which librairies have Qt Widgets
set (CISST_LIBS_WITH_QT
     cisstCommon
     cisstVector
     cisstMultiTask
     cisstStereoVision)

# Which librairies have FLTK Widgets
set (CISST_LIBS_WITH_FLTK
     cisstVector)


# Build by default
foreach (lib ${CISST_LIBS_STABLE})
  option (CISST_BUILD_LIBS_${lib} "Build ${lib}." ON)
endforeach (lib)

foreach (lib ${CISST_LIBS_UNSTABLE})
  option (CISST_BUILD_LIBS_${lib} "Build ${lib}." OFF)
endforeach (lib)

# If OS abstraction is compiled, require dynamic loading libraries
if (CISST_BUILD_LIBS_cisstOSAbstraction OR CISST_BUILD_LIBS_cisstInteractive)
  if (CMAKE_DL_LIBS)
    cisst_set_package_settings (cisstOSAbstraction DL LIBRARIES CMAKE_DL_LIBS)
    cisst_set_package_settings (cisstInteractive DL LIBRARIES CMAKE_DL_LIBS)
  endif (CMAKE_DL_LIBS)
endif (CISST_BUILD_LIBS_cisstOSAbstraction OR CISST_BUILD_LIBS_cisstInteractive)

# If cisstInteractive in turned on, also force SWIG for Python and shared libraries
if (CISST_BUILD_LIBS_cisstInteractive)
  set (CISST_HAS_SWIG_PYTHON ON CACHE BOOL "Build the Python wrappers for cisst with Swig." FORCE)
endif (CISST_BUILD_LIBS_cisstInteractive)
if (CISST_HAS_SWIG_PYTHON)
  set (CISST_BUILD_SHARED_LIBS ON CACHE BOOL "Build cisst with shared libraries." FORCE)
endif (CISST_HAS_SWIG_PYTHON)

# If cisstNumerical is ON, turn CISST_HAS_CISSTNETLIB ON as well
if (CISST_BUILD_LIBS_cisstNumerical)
  # Use Fortran NETLIB by default
  option (CISST_HAS_CISSTNETLIB "Use the netlib.org sets of numerical methods in Fortran (provided on cisst.org)." ON)
  mark_as_advanced (CISST_HAS_CISSTNETLIB)
  if (CISST_HAS_CISSTNETLIB)
    find_package (CisstNetlib)
    cisst_set_package_settings (cisstNumerical cisstNetlib INCLUDE_DIRECTORIES CISSTNETLIB_INCLUDE_DIR)
    cisst_set_package_settings (cisstNumerical cisstNetlib LIBRARIES           CISSTNETLIB_LIBRARIES)
  else (CISST_HAS_CISSTNETLIB)
    cisst_unset_all_package_settings (cisstNumerical cisstNetlib)
  endif (CISST_HAS_CISSTNETLIB)
endif (CISST_BUILD_LIBS_cisstNumerical)

# List of libraries to be built
foreach (lib ${CISST_LIBS})
  if (CISST_BUILD_LIBS_${lib})
    set (CISST_LIBRARIES ${lib} ${CISST_LIBRARIES})
    # Add libraries with Qt
    if (CISST_HAS_QT)
      list (FIND CISST_LIBS_WITH_QT ${lib} ${lib}_HAS_QT)
      if (${${lib}_HAS_QT} GREATER -1)
        set (CISST_LIBRARIES ${lib}Qt ${CISST_LIBRARIES})
        set (CISST_BUILD_LIBS_${lib}Qt ON)
      else (${${lib}_HAS_QT} GREATER -1)
        set (CISST_BUILD_LIBS_${lib}Qt OFF)
      endif (${${lib}_HAS_QT} GREATER -1)
    endif (CISST_HAS_QT)
    # Add libraries with FLTK
    if (CISST_HAS_FLTK)
      list (FIND CISST_LIBS_WITH_FLTK ${lib} ${lib}_HAS_FLTK)
      if (${${lib}_HAS_FLTK} GREATER -1)
        set (CISST_LIBRARIES ${lib}FLTK ${CISST_LIBRARIES})
        set (CISST_BUILD_LIBS_${lib}FLTK ON)
      else (${${lib}_HAS_FLTK} GREATER -1)
        set (CISST_BUILD_LIBS_${lib}FLTK OFF)
      endif (${${lib}_HAS_FLTK} GREATER -1)
    endif (CISST_HAS_FLTK)
  endif (CISST_BUILD_LIBS_${lib})
endforeach (lib)

# Add cisstCommonXML if needed
if (CISST_BUILD_LIBS_cisstCommon AND CISST_HAS_XML)
  set (CISST_BUILD_LIBS_cisstCommonXML ON CACHE BOOL "Build cisstCommonXML" FORCE)
  set (CISST_LIBRARIES cisstCommonXML ${CISST_LIBRARIES})
else (CISST_BUILD_LIBS_cisstCommon AND CISST_HAS_XML)
  set (CISST_BUILD_LIBS_cisstCommonXML OFF CACHE BOOL "Build cisstCommonXML" FORCE)
endif (CISST_BUILD_LIBS_cisstCommon AND CISST_HAS_XML)

# Once all variables have been defined, use cisstMacros
#include (cisstMacros)

# Finally, create lib targets
add_subdirectory (libs)
cisst_cmake_debug ("Libraries to be built: ${CISST_LIBRARIES}")

# Conditional builds
if (CISST_BUILD_UTILITIES)
  add_subdirectory (utilities)
endif (CISST_BUILD_UTILITIES)

if (CISST_BUILD_TESTS)
  # Settings for CTest
  enable_testing ()
  mark_as_advanced (BUILD_TESTING) # CMake variable for CTest
  include (Dart)
  mark_as_advanced (DART_ROOT)
  # cisst Tests
  add_subdirectory (tests)
endif (CISST_BUILD_TESTS)

if (CISST_BUILD_EXAMPLES)
  add_subdirectory (examples)
endif (CISST_BUILD_EXAMPLES)

if (CISST_BUILD_COMPONENTS)
  add_subdirectory (components)
endif (CISST_BUILD_COMPONENTS)

if (CISST_BUILD_DOC)
  add_subdirectory (doc)
endif (CISST_BUILD_DOC)

# create equivalent for cisstInteractive
set (CISST_HAS_IRE ${CISST_BUILD_LIBS_cisstInteractive})

# Create misc. config files
set (CISST_BUILD_INCLUDE_DIRECTORIES ${CISST_INCLUDE_DIR})
set (CISST_BUILD_DOXYGEN_DIRECTORY ${cisst_SOURCE_DIR}/libs/include)
set (CISST_BUILD_LINK_DIRECTORIES ${cisst_LIBRARY_PATH})
set (CISST_BUILD_CISSTNETLIB_LIBRARIES ${CISSTNETLIB_LIBRARIES} ${CISSTNETLIB_F95_LIBRARIES})
if (NOT UNIX)
    set (CISST_BUILD_CISSTNETLIB_LIBRARIES ${CISST_BUILD_CISSTNETLIB_LIBRARIES} ${CISSTNETLIB_GCC_LIBRARIES})
endif (NOT UNIX)
# Preserved for backward compatibility
configure_file (${cisst_SOURCE_DIR}/libs/cmake/cisstBuild.cmake.in
                ${cisst_BINARY_DIR}/cisstBuild.cmake
                @ONLY)
# Very similar file but usable with CMake find_package NO_MODULE (if
# the user builds against the cisst build tree)
configure_file (${cisst_SOURCE_DIR}/libs/cmake/cisstBuild.cmake.in
                ${cisst_BINARY_DIR}/cisst-config.cmake
                @ONLY)

# Generate cisst-config.cmake to work with find_package NO_MODULE (if
# the user builds against an "installed" version of cisst) -- not
# supported yet
set (CISST_INSTALLED_INCLUDE_DIRECTORIES ${CMAKE_INSTALL_PREFIX}/include)
set (CISST_INSTALLED_DOXYGEN_DIRECTORY ${CMAKE_INSTALL_PREFIX}/include)
set (CISST_INSTALLED_LINK_DIRECTORIES ${CMAKE_INSTALL_PREFIX}/lib)
configure_file (${cisst_SOURCE_DIR}/libs/cmake/cisstInstalled.cmake.in
                ${cisst_BINARY_DIR}/cisstInstalled.cmake
                @ONLY)
install_files (/share/cisst/cmake
               ".cmake"
               ${cisst_BINARY_DIR}/cisstInstalled.cmake)


# Configuration file
set (CISST_SOURCE_DIR ${cisst_SOURCE_DIR})
configure_file (${cisst_SOURCE_DIR}/libs/include/cisstConfig.h.in
                ${cisst_BINARY_DIR}/libs/include/cisstConfig.h)
install_files (/include
               ".h"
               ${cisst_BINARY_DIR}/libs/include/cisstConfig.h)


# Environment variables for Windows
IF(WIN32)
  # Script to set cisst environment variables
  FILE(TO_NATIVE_PATH ${cisst_BINARY_DIR} cisst_BINARY_DIR_NATIVE)
  CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/utils/cisstvars.bat.in
                 ${cisst_BINARY_DIR}/cisstvars.bat)
  # Script to set Microsoft compiler environment variables
  GET_FILENAME_COMPONENT(BUILD_TOOL_DIRECTORY ${CMAKE_MAKE_PROGRAM} PATH)
  FIND_PROGRAM(VSVARS32_BAT
               NAMES vsvars32.bat
               PATHS ${BUILD_TOOL_DIRECTORY}/../../Common7/Tools)
  MARK_AS_ADVANCED(VSVARS32_BAT)
  CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/utils/cisstvsvars.bat.in
                 ${cisst_BINARY_DIR}/cisstvsvars.bat)

  # If Visual Studio
  IF(CMAKE_GENERATOR MATCHES "Visual Studio*" )
    FOREACH(config ${CMAKE_CONFIGURATION_TYPES})
      CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/utils/visual-studio.bat.in
                     ${cisst_BINARY_DIR}/visual-studio-${config}.bat)
    ENDFOREACH(config)
  ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio*" )

  # Offer to register the cisst paths
  OPTION(CISST_PATH_REGISTER "Register the paths to cisst libraries" OFF)
  IF(CISST_PATH_REGISTER)

    # Set and test configuration (i.e. Debug, Release, ...)
    SET(CISST_PATH_CONFIGURATION CACHE STRING "Configuration used for the paths")
    SET(CISST_PATH_CONFIGURATION_VALID FALSE)

    FOREACH(config ${CMAKE_CONFIGURATION_TYPES})
      IF("${config}" STREQUAL "CISST_PATH_CONFIGURATION")
        SET(CISST_PATH_CONFIGURATION_VALID TRUE)
      ENDIF("${config}" STREQUAL "CISST_PATH_CONFIGURATION")
    ENDFOREACH(config)

    # Configuration is valid, build the path and compare it to the current one
    IF(CISST_PATH_CONFIGURATION_VALID)

      # Initial settings for paths
      SET(CISST_PATH_CMAKE_FORMAT
          "${EXECUTABLE_OUTPUT_PATH}/${CISST_PATH_CONFIGURATION}"
          "${LIBRARY_OUTPUT_PATH}/${CISST_PATH_CONFIGURATION}")
      SET(CISST_PYTHONPATH_CMAKE_FORMAT
          "${EXECUTABLE_OUTPUT_PATH}" "${EXECUTABLE_OUTPUT_PATH}/${CISST_PATH_CONFIGURATION}"
          "${LIBRARY_OUTPUT_PATH}" "${LIBRARY_OUTPUT_PATH}/${CISST_PATH_CONFIGURATION}")

      # To find tests programs and libraries
      IF(CISST_BUILD_TESTS)
        SET(CISST_PATH_CMAKE_FORMAT
            ${CISST_PATH_CMAKE_FORMAT}
            "${cisst_BINARY_DIR}/tests/lib/${CISST_PATH_CONFIGURATION}"
            "${cisst_BINARY_DIR}/tests/bin/${CISST_PATH_CONFIGURATION}")
        SET(CISST_PYTHONPATH_CMAKE_FORMAT
            ${CISST_PYTHONPATH_CMAKE_FORMAT}
            "${cisst_BINARY_DIR}/tests/lib" "${cisst_BINARY_DIR}/tests/lib/${CISST_PATH_CONFIGURATION}"
            "${cisst_BINARY_DIR}/tests/bin" "${cisst_BINARY_DIR}/tests/bin/${CISST_PATH_CONFIGURATION}")
      ENDIF(CISST_BUILD_TESTS)

      # To find examples programs and libraries
      IF(CISST_BUILD_EXAMPLES)
        SET(CISST_PATH_CMAKE_FORMAT
            ${CISST_PATH_CMAKE_FORMAT}
            "${cisst_BINARY_DIR}/examples/lib/${CISST_PATH_CONFIGURATION}"
            "${cisst_BINARY_DIR}/examples/bin/${CISST_PATH_CONFIGURATION}")
        SET(CISST_PYTHONPATH_CMAKE_FORMAT
            ${CISST_PYTHONPATH_CMAKE_FORMAT}
            "${cisst_BINARY_DIR}/examples/lib" "${cisst_BINARY_DIR}/examples/lib/${CISST_PATH_CONFIGURATION}"
            "${cisst_BINARY_DIR}/examples/bin" "${cisst_BINARY_DIR}/examples/bin/${CISST_PATH_CONFIGURATION}")
      ENDIF(CISST_BUILD_EXAMPLES)

      # To find utilities programs and libraries
      IF(CISST_BUILD_UTILITIES)
        SET(CISST_PATH_CMAKE_FORMAT
            ${CISST_PATH_CMAKE_FORMAT}
            "${cisst_BINARY_DIR}/utilities/lib/${CISST_PATH_CONFIGURATION}"
            "${cisst_BINARY_DIR}/utilities/bin/${CISST_PATH_CONFIGURATION}")
        SET(CISST_PYTHONPATH_CMAKE_FORMAT
            ${CISST_PYTHONPATH_CMAKE_FORMAT}
            "${cisst_BINARY_DIR}/utilities/lib" "${cisst_BINARY_DIR}/utilities/lib/${CISST_PATH_CONFIGURATION}"
            "${cisst_BINARY_DIR}/utilities/bin" "${cisst_BINARY_DIR}/utilities/bin/${CISST_PATH_CONFIGURATION}")
      ENDIF(CISST_BUILD_UTILITIES)

      # To find components (libraries) and example programs
      IF(BUILD_COMPONENTS)
        SET(CISST_PATH_CMAKE_FORMAT
            ${CISST_PATH_CMAKE_FORMAT}
            "${cisst_BINARY_DIR}/components/lib/${CISST_PATH_CONFIGURATION}"
            "${cisst_BINARY_DIR}/components/bin/${CISST_PATH_CONFIGURATION}")
        SET(CISST_PYTHONPATH_CMAKE_FORMAT
            ${CISST_PYTHONPATH_CMAKE_FORMAT}
            "${cisst_BINARY_DIR}/components/lib/${CISST_PATH_CONFIGURATION}"
            "${cisst_BINARY_DIR}/components/bin/${CISST_PATH_CONFIGURATION}")
      ENDIF(BUILD_COMPONENTS)

      # Convert to native format to be able to compare
      FILE(TO_NATIVE_PATH "${CISST_PATH_CMAKE_FORMAT}" CISST_PATH)
      FILE(TO_NATIVE_PATH "${CISST_PYTHONPATH_CMAKE_FORMAT}" CISST_PYTHONPATH)

      # Retrieve current path, little trick to read register key/value
      SET(CISST_PATH_REG_KEY "HKEY_CURRENT_USER\\Environment;CISST_PATH")
      GET_FILENAME_COMPONENT(CURRENT_CISST_PATH_CMAKE_FORMAT
                             "[HKEY_CURRENT_USER\\Environment;CISST_PATH]\\dummy.dum"
                             PATH)
      FILE(TO_NATIVE_PATH "${CURRENT_CISST_PATH_CMAKE_FORMAT}" CURRENT_CISST_PATH)

      SET(CISST_PYTHONPATH_REG_KEY "HKEY_CURRENT_USER\\Environment;CISST_PYTHONPATH")
      GET_FILENAME_COMPONENT(CURRENT_CISST_PYTHONPATH_CMAKE_FORMAT
                             "[HKEY_CURRENT_USER\\Environment;CISST_PYTHONPATH]\\dummy.dum"
                             PATH)
      FILE(TO_NATIVE_PATH "${CURRENT_CISST_PYTHONPATH_CMAKE_FORMAT}" CURRENT_CISST_PYTHONPATH)

      # If there is no existing path, add it
      IF("${CURRENT_CISST_PATH}" STREQUAL "\\registry")
        # not defined, add it to register using cmake -E
        EXEC_PROGRAM(${CMAKE_COMMAND}
                     ARGS "-E write_regv \"${CISST_PATH_REG_KEY}\" \"${CISST_PATH}\"")
      ELSE("${CURRENT_CISST_PATH}" STREQUAL "\\registry")
        IF("${CISST_PATH}" STREQUAL "${CURRENT_CISST_PATH}")
          # MESSAGE("CISST_PATH is the same")
        ELSE("${CISST_PATH}" STREQUAL "${CURRENT_CISST_PATH}")
          # The variable already exists and is different
          OPTION(CISST_PATH_OVERWRITE "Overwrite existing CISST_PATH" OFF)
          IF(CISST_PATH_OVERWRITE)
            # add it to register using cmake -E
            EXEC_PROGRAM(${CMAKE_COMMAND}
                         ARGS "-E write_regv \"${CISST_PATH_REG_KEY}\" \"${CISST_PATH}\"")
            # Set overwrite back to OFF by default.
            SET(CISST_PATH_OVERWRITE OFF CACHE BOOL "Overwrite existing CISST_PATH" FORCE)
          ELSE(CISST_PATH_OVERWRITE)
            # Display a message of information
            MESSAGE("A different CISST_PATH exists:\n  ${CURRENT_CISST_PATH}\nTurn CISST_PATH_OVERWRITE \"ON\" or CISST_PATH_REGISTER \"OFF\"")
          ENDIF(CISST_PATH_OVERWRITE)
        ENDIF("${CISST_PATH}" STREQUAL "${CURRENT_CISST_PATH}")

      ENDIF("${CURRENT_CISST_PATH}" STREQUAL "\\registry")

      # Do the same for PYTHONPATH if wrappers are compiled
      IF(CISST_HAS_SWIG_PYTHON)
        # If there is no existing path, add it
        IF("${CURRENT_CISST_PYTHONPATH}" STREQUAL "\\registry")
          # not defined, add it to register using cmake -E
          EXEC_PROGRAM(${CMAKE_COMMAND}
                       ARGS "-E write_regv \"${CISST_PYTHONPATH_REG_KEY}\" \"${CISST_PYTHONPATH}\"")
        ELSE("${CURRENT_CISST_PYTHONPATH}" STREQUAL "\\registry")
          IF("${CISST_PYTHONPATH}" STREQUAL "${CURRENT_CISST_PYTHONPATH}")
            # MESSAGE("CISST_PATH is the same")
          ELSE("${CISST_PYTHONPATH}" STREQUAL "${CURRENT_CISST_PYTHONPATH}")
            # The variable already exists and is different
            OPTION(CISST_PYTHONPATH_OVERWRITE "Overwrite existing CISST_PYTHONPATH" OFF)
            IF(CISST_PYTHONPATH_OVERWRITE)
              # Add it to register using cmake -E
              EXEC_PROGRAM(${CMAKE_COMMAND}
                           ARGS "-E write_regv \"${CISST_PYTHONPATH_REG_KEY}\" \"${CISST_PYTHONPATH}\"")
              # Set overwrite back to OFF by default.
              SET(CISST_PYTHONPATH_OVERWRITE OFF CACHE BOOL "Overwrite existing CISST_PYTHONPATH" FORCE)
            ELSE(CISST_PYTHONPATH_OVERWRITE)
              # Display a message of information
              MESSAGE("A different CISST_PYTHONPATH exists:\n  ${CURRENT_CISST_PYTHONPATH}\nTurn CISST_PYTHONPATH_OVERWRITE \"ON\" or CISST_PATH_REGISTER \"OFF\"")
            ENDIF(CISST_PYTHONPATH_OVERWRITE)
          ENDIF("${CISST_PYTHONPATH}" STREQUAL "${CURRENT_CISST_PYTHONPATH}")

        ENDIF("${CURRENT_CISST_PYTHONPATH}" STREQUAL "\\registry")
      ENDIF(CISST_HAS_SWIG_PYTHON)

    ELSE(CISST_PATH_CONFIGURATION_VALID)
      MESSAGE("CISST_PATH_CONFIGURATION must be one of ${CMAKE_CONFIGURATION_TYPES}")
    ENDIF(CISST_PATH_CONFIGURATION_VALID)

  ENDIF(CISST_PATH_REGISTER)

ENDIF(WIN32)

# Environment variables for Unix
IF(UNIX)
  # Script to set cisst environment variables
  CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/utils/cisstvars.csh.in
                 ${cisst_BINARY_DIR}/cisstvars.csh
                 @ONLY)
  # Script to set cisst environment variables
  CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/utils/cisstvars.sh.in
                 ${cisst_BINARY_DIR}/cisstvars.sh
                 @ONLY)
ENDIF(UNIX)

