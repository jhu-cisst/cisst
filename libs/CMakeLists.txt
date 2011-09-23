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

# cisst libraries (Computer Integrated Surgical Systems and Technology)
project (cisstLibs)

# Create some variables so that find_package (cisst) can be used in subdirectories (examples)
set (CURRENT_PROJECT_IS_CISST ON)

# Option to make cisst CMake functions more verbose
option (CISST_HAS_CMAKE_DEBUG "Print extra messages when configuring with CMake" OFF)
mark_as_advanced (CISST_HAS_CMAKE_DEBUG)


# Function to expend a list without repetitions, forces the list to be internal
function (CISST_ADD_TO_INTERNAL list newElements)
  message (SEND_ERROR "cisst_add_to_internal is now deprecated, use cisst_set_package_settings instead")
endfunction (CISST_ADD_TO_INTERNAL)

# Where to find cisst defined CMake Macros/Modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${cisstLibs_SOURCE_DIR}/cmake)

# The upper case variables have been used in the past and need to be preserved
set (CISST_BINARY_DIR ${cisst_BINARY_DIR})
set (CISST_SOURCE_DIR ${cisst_SOURCE_DIR})

# Include all useful CMake macros and functions for cisst 
include (cisstMacros)
include (cisstInternalMacros)

# Output directories.
set (LIBRARY_OUTPUT_PATH ${cisstLibs_BINARY_DIR}/lib)
set (EXECUTABLE_OUTPUT_PATH ${cisstLibs_BINARY_DIR}/bin)

# Where to find afterwards
set (cisst_LIBRARY_PATH "${LIBRARY_OUTPUT_PATH}")
set (cisst_EXECUTABLE_PATH "${EXECUTABLE_OUTPUT_PATH}")

# On Windows, CMake uses a different compiler at configuration time
if (CMAKE_CL_64)
  set (CISST_CMAKE_COMPILER_IS_MSVC_64 ON)
else (CMAKE_CL_64)
  set (CISST_CMAKE_COMPILER_IS_MSVC_64 OFF)
endif (CMAKE_CL_64)

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


# Use a separate directory to find external dependencies, this allows
# to include some CMake "use" files without polluting the environment
add_subdirectory (cisst-dependencies)


# Wrapping with swig for Python
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
  cisst_set_package_settings (cisstPython Python INCLUDE_DIRECTORIES PYTHON_INCLUDE_PATH)
  set (PYTHON_ALL_LIBRARIES "debug;${PYTHON_DEBUG_LIBRARIES};optimized;${PYTHON_LIBRARIES}")
  cisst_set_package_settings (cisstPython Python LIBRARIES PYTHON_ALL_LIBRARIES)
  # We also need numpy if we have cisstVector
  if (CISST_BUILD_LIBS_cisstVector)
    find_package (Numpy REQUIRED)
    if (PYTHON_NUMPY_FOUND)
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
     ${cisstLibs_BINARY_DIR}/include
     ${cisstLibs_SOURCE_DIR})
if (CISST_BUILD_COMPONENTS)
  set (CISST_INCLUDE_DIR ${CISST_INCLUDE_DIR}
                         ${cisst_BINARY_DIR}/components
                         ${cisst_SOURCE_DIR}/components)
endif (CISST_BUILD_COMPONENTS)

include_directories (${CISST_INCLUDE_DIR})


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
     cisstNumerical
     cisstInteractive
     )

set (CISST_LIBS_UNSTABLE
     cisstMultiTask
     cisstStereoVision
     cisstRobot
     cisstParameterTypes
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

# create equivalent for cisstInteractive
set (CISST_HAS_IRE ${CISST_BUILD_LIBS_cisstInteractive})

# Finally, create lib targets
cisst_cmake_debug ("Libraries to be built: ${CISST_LIBRARIES}")

# Build everything
foreach (lib ${CISST_LIBS})
  if (CISST_BUILD_LIBS_${lib})
    add_subdirectory (${lib})
  endif (CISST_BUILD_LIBS_${lib})
endforeach (lib)


# Create misc. config files
set (CISST_BUILD_INCLUDE_DIRECTORIES ${CISST_INCLUDE_DIR})
set (CISST_BUILD_DOXYGEN_DIRECTORY ${cisst_SOURCE_DIR}/libs/include)
set (CISST_BUILD_LINK_DIRECTORIES ${cisst_LIBRARY_PATH})

# set (CISST_BUILD_CISSTNETLIB_LIBRARIES ${CISSTNETLIB_LIBRARIES} ${CISSTNETLIB_F95_LIBRARIES})
# if (NOT UNIX)
#     set (CISST_BUILD_CISSTNETLIB_LIBRARIES ${CISST_BUILD_CISSTNETLIB_LIBRARIES} ${CISSTNETLIB_GCC_LIBRARIES})
# endif (NOT UNIX)
# # Preserved for backward compatibility
# configure_file (${cisst_SOURCE_DIR}/libs/cmake/cisstBuild.cmake.in
#                 ${cisst_BINARY_DIR}/cisstBuild.cmake
#                 @ONLY)
# Very similar file but usable with CMake find_package NO_MODULE (if
# the user builds against the cisst build tree)

configure_file ("${cisstLibs_SOURCE_DIR}/cmake/cisst-config.cmake.in"
                "${cisstLibs_BINARY_DIR}/cisst-config.cmake"
                @ONLY)


# # Create misc. config files
# set (CISST_BUILD_INCLUDE_DIRECTORIES ${CISST_INCLUDE_DIR})
# set (CISST_BUILD_DOXYGEN_DIRECTORY ${cisst_SOURCE_DIR}/libs/include)
# set (CISST_BUILD_LINK_DIRECTORIES ${cisst_LIBRARY_PATH})
# set (CISST_BUILD_CISSTNETLIB_LIBRARIES ${CISSTNETLIB_LIBRARIES} ${CISSTNETLIB_F95_LIBRARIES})
# if (NOT UNIX)
#     set (CISST_BUILD_CISSTNETLIB_LIBRARIES ${CISST_BUILD_CISSTNETLIB_LIBRARIES} ${CISSTNETLIB_GCC_LIBRARIES})
# endif (NOT UNIX)
# # Preserved for backward compatibility
# configure_file (${cisst_SOURCE_DIR}/libs/cmake/cisstBuild.cmake.in
#                 ${cisst_BINARY_DIR}/cisstBuild.cmake
#                 @ONLY)
# # Very similar file but usable with CMake find_package NO_MODULE (if
# # the user builds against the cisst build tree)
# configure_file (${cisst_SOURCE_DIR}/libs/cmake/cisstBuild.cmake.in
#                 ${cisst_BINARY_DIR}/cisst-config.cmake
#                 @ONLY)

# # Generate cisst-config.cmake to work with find_package NO_MODULE (if
# # the user builds against an "installed" version of cisst) -- not
# # supported yet
# set (CISST_INSTALLED_INCLUDE_DIRECTORIES ${CMAKE_INSTALL_PREFIX}/include)
# set (CISST_INSTALLED_DOXYGEN_DIRECTORY ${CMAKE_INSTALL_PREFIX}/include)
# set (CISST_INSTALLED_LINK_DIRECTORIES ${CMAKE_INSTALL_PREFIX}/lib)
# configure_file (${cisst_SOURCE_DIR}/libs/cmake/cisstInstalled.cmake.in
#                 ${cisst_BINARY_DIR}/cisstInstalled.cmake
#                 @ONLY)
# install_files (/share/cisst/cmake
#                ".cmake"
#                ${cisst_BINARY_DIR}/cisstInstalled.cmake)


# Configuration file
set (CISST_SOURCE_DIR ${cisst_SOURCE_DIR})
configure_file (${cisstLibs_SOURCE_DIR}/cisstConfig.h.in
                ${cisstLibs_BINARY_DIR}/include/cisstConfig.h)
install_files (/include
               ".h"
               ${cisstLibs_BINARY_DIR}/include/cisstConfig.h)
