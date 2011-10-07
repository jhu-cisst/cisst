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

# set the version number
set (CISST_VERSION_MAJOR "1")
set (CISST_VERSION_MINOR "0")
set (CISST_VERSION_PATCH "0")
set (CISST_VERSION "${CISST_VERSION_MAJOR}.${CISST_VERSION_MINOR}.${CISST_VERSION_PATCH}")

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


# Use a separate directory to find external dependencies, this allows
# to include some CMake "use" files without polluting the environment
add_subdirectory (cisst-dependencies)


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

# Build test driver if needed
if (CISST_BUILD_TESTS)
  find_package (CppUnit REQUIRED)
  # TO DO, check if CPPUNIT Found
  add_subdirectory (cisstTestsDriver)
endif (CISST_BUILD_TESTS)

# Find native threads
if (NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "QNX")
  find_package (Threads REQUIRED)
  if (CMAKE_THREAD_LIBS_INIT)
    cisst_set_package_settings (cisstCommon Threads LIBRARIES CMAKE_THREAD_LIBS_INIT)
  endif (CMAKE_THREAD_LIBS_INIT)
endif (NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "QNX")

# Which libraries to build
set (CISST_LIBRARIES_DEFAULT
     cisstCommon
     cisstVector
     cisstOSAbstraction
     cisstNumerical
     cisstMultiTask
     cisstParameterTypes
     cisstRobot
     cisstStereoVision
     )

set (CISST_LIBRARIES_OPTIONAL
     cisstInteractive
     cisstDevices
     cisst3DUserInterface
     )

# All libraries
set (CISST_LIBS
     ${CISST_LIBRARIES_DEFAULT}
     ${CISST_LIBRARIES_OPTIONAL})

# Which librairies have Qt Widgets
set (CISST_LIBS_WITH_QT
     cisstCommon
     cisstVector
     cisstMultiTask
     cisstStereoVision
     cisstDevices)

# Which librairies have FLTK Widgets
set (CISST_LIBS_WITH_FLTK
     cisstVector)


# Build by default
foreach (lib ${CISST_LIBRARIES_DEFAULT})
  option (CISST_BUILD_${lib} "Build ${lib}." ON)
endforeach (lib)

foreach (lib ${CISST_LIBRARIES_OPTIONAL})
  option (CISST_BUILD_${lib} "Build ${lib}." OFF)
endforeach (lib)

# If OS abstraction is compiled, require dynamic loading libraries
if (CISST_BUILD_cisstOSAbstraction OR CISST_BUILD_cisstInteractive)
  if (CMAKE_DL_LIBS)
    cisst_set_package_settings (cisstOSAbstraction DL LIBRARIES CMAKE_DL_LIBS)
    cisst_set_package_settings (cisstInteractive DL LIBRARIES CMAKE_DL_LIBS)
  endif (CMAKE_DL_LIBS)
endif (CISST_BUILD_cisstOSAbstraction OR CISST_BUILD_cisstInteractive)

# If cisstInteractive in turned on, also force SWIG for Python and shared libraries
if (CISST_BUILD_cisstInteractive)
  set (CISST_HAS_SWIG_PYTHON ON CACHE BOOL "Build the Python wrappers for cisst with Swig." FORCE)
endif (CISST_BUILD_cisstInteractive)
if (CISST_HAS_SWIG_PYTHON)
  set (CISST_BUILD_SHARED_LIBS ON CACHE BOOL "Build cisst with shared libraries." FORCE)
endif (CISST_HAS_SWIG_PYTHON)


# List of libraries to be built
foreach (lib ${CISST_LIBS})
  if (CISST_BUILD_${lib})
    set (CISST_LIBRARIES ${lib} ${CISST_LIBRARIES})
    # Add libraries with Qt
    if (CISST_HAS_QT)
      list (FIND CISST_LIBS_WITH_QT ${lib} ${lib}_HAS_QT)
      if (${${lib}_HAS_QT} GREATER -1)
        set (CISST_LIBRARIES ${lib}Qt ${CISST_LIBRARIES})
        set (CISST_BUILD_${lib}Qt ON)
      else (${${lib}_HAS_QT} GREATER -1)
        set (CISST_BUILD_${lib}Qt OFF)
      endif (${${lib}_HAS_QT} GREATER -1)
    endif (CISST_HAS_QT)
    # Add libraries with FLTK
    if (CISST_HAS_FLTK)
      list (FIND CISST_LIBS_WITH_FLTK ${lib} ${lib}_HAS_FLTK)
      if (${${lib}_HAS_FLTK} GREATER -1)
        set (CISST_LIBRARIES ${lib}FLTK ${CISST_LIBRARIES})
        set (CISST_BUILD_${lib}FLTK ON)
      else (${${lib}_HAS_FLTK} GREATER -1)
        set (CISST_BUILD_${lib}FLTK OFF)
      endif (${${lib}_HAS_FLTK} GREATER -1)
    endif (CISST_HAS_FLTK)
  endif (CISST_BUILD_${lib})
endforeach (lib)

# Add cisstCommonXML if needed
if (CISST_BUILD_cisstCommonXML)
  set (CISST_LIBRARIES cisstCommonXML ${CISST_LIBRARIES})
endif (CISST_BUILD_cisstCommonXML)

# create equivalent for cisstInteractive
set (CISST_HAS_IRE ${CISST_BUILD_cisstInteractive})

# Create list of general settings, i.e. cisstPython, cisstQt, cisstFLTK, cisstOpenGL, ...
set (CISST_SETTINGS "")
if (CISST_HAS_QT)
  set (CISST_SETTINGS ${CISST_SETTINGS} cisstQt)
endif (CISST_HAS_QT)

if (CISST_HAS_FLTK)
  set (CISST_SETTINGS ${CISST_SETTINGS} cisstFLTK)
endif (CISST_HAS_FLTK)

if (CISST_HAS_SWIG_PYTHON)
  set (CISST_SETTINGS ${CISST_SETTINGS} cisstPython)
endif (CISST_HAS_SWIG_PYTHON)

if (CISST_HAS_OPENGL)
  set (CISST_SETTINGS ${CISST_SETTINGS} cisstOpenGL)
endif (CISST_HAS_OPENGL)


# Finally, create lib targets
cisst_cmake_debug ("Libraries to be built: ${CISST_LIBRARIES}")
cisst_cmake_debug ("General settings: ${CISST_SETTINGS}")

# Build all libraries
foreach (lib ${CISST_LIBS})
  if (CISST_BUILD_${lib})
    add_subdirectory (${lib})
  endif (CISST_BUILD_${lib})
endforeach (lib)

# Create misc. config files
set (CISST_BUILD_INCLUDE_DIRECTORIES ${CISST_INCLUDE_DIR})
set (CISST_BUILD_DOXYGEN_DIRECTORY ${cisstLibs_SOURCE_DIR})
set (CISST_BUILD_LINK_DIRECTORIES ${cisst_LIBRARY_PATH})
configure_file ("${cisstLibs_SOURCE_DIR}/cmake/cisst-config.cmake.in"
                "${cisstLibs_BINARY_DIR}/cisst-config.cmake"
                @ONLY)

# Configuration file
set (CISST_SOURCE_DIR ${cisst_SOURCE_DIR})
configure_file (${cisstLibs_SOURCE_DIR}/cisstConfig.h.in
                ${cisstLibs_BINARY_DIR}/include/cisstConfig.h)
install (FILES  ${cisstLibs_BINARY_DIR}/include/cisstConfig.h
         DESTINATION include
         COMPONENT cisstCommon)

# Use CPack package generator
set(CPACK_PACKAGE_NAME "cisst")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "cisst libraries")
# For now, enable all default libraries (even if not compiled)
set(CPACK_COMPONENTS_ALL ${CISST_LIBRARIES_DEFAULT})
if (CISST_BUILD_cisstCommonXML AND ${CISST_XML_LIB} STREQUAL "LibXml2")
  set (CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL} cisstCommonXML)
endif (CISST_BUILD_cisstCommonXML AND ${CISST_XML_LIB} STREQUAL "LibXml2")
set(CPACK_PACKAGE_VENDOR "JHU")
set(CPACK_PACKAGE_VERSION ${CISST_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${CISST_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${CISST_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${CISST_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${CISST_SOURCE_DIR}/license.txt")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "cisst")
set(CPACK_COMPONENT_CISSTCOMMON_REQUIRED ON)
# Dependencies should be done automatically (don't really need to specify
# cisstCommon dependency because it is a required component)
set(CPACK_COMPONENT_CISSTCOMMONXML_DEPENDS       cisstCommon)
set(CPACK_COMPONENT_CISSTVECTOR_DEPENDS          cisstCommon)
set(CPACK_COMPONENT_CISSTNUMERICAL_DEPENDS       cisstCommon cisstVector)
set(CPACK_COMPONENT_CISSTOSABSTRACTION_DEPENDS   cisstCommon)
set(CPACK_COMPONENT_CISSTMULTITASK_DEPENDS       cisstCommon cisstVector cisstOSAbstraction)
set(CPACK_COMPONENT_CISSTSTEREOVISION_DEPENDS    cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)
set(CPACK_COMPONENT_CISSTPARAMETERTYPES_DEPENDS  cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)
set(CPACK_COMPONENT_CISSTROBOT_DEPENDS           cisstCommon cisstVector cisstNumerical)
if (WIN32)
  set(CPACK_NSIS_MODIFY_PATH ON)
endif (WIN32)
INCLUDE(CPack)
