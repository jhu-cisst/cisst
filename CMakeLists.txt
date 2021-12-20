## -*- Mode: CMAKE; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-

#
# (C) Copyright 2005-2021 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---


# Backward compatibility
cmake_minimum_required (VERSION 3.0)

# cisst (Computer Integrated Surgical Systems and Technology)
project (cisst)

# cisst compiler settings
include (cmake/cisstSettings.cmake)

# To organize targets in folders
option (CISST_USE_FOLDERS "Use folders to organize cisst in IDE (Visual Studio, ...)" OFF)
mark_as_advanced (CISST_USE_FOLDERS)
if (CISST_USE_FOLDERS)
  set_property (GLOBAL PROPERTY USE_FOLDERS ON)
else (CISST_USE_FOLDERS)
  set_property (GLOBAL PROPERTY USE_FOLDERS OFF)
endif (CISST_USE_FOLDERS)

# Create some variables so that find_package (cisst) can be used in subdirectories (examples)
set (cisst_DIR "${cisst_BINARY_DIR}")

# Option to make cisst CMake functions more verbose
option (CISST_HAS_CMAKE_DEBUG "Print extra messages when configuring with CMake" OFF)
mark_as_advanced (CISST_HAS_CMAKE_DEBUG)

# Options
option (CISST_BUILD_TESTS "Build cisst tests (when turned ON, all tests are to be compiled by default)." OFF)
option (CISST_BUILD_EXAMPLES "Build cisst examples (when turned ON, user will be asked to select which examples to build)." OFF)

# Applications should be compiled by default
option (CISST_BUILD_APPLICATIONS "Build cisst applications (when turned ON, all applications are to be compile by default)." ON)
mark_as_advanced (CISST_BUILD_APPLICATIONS)

# Activate tests before any directory is added
if (CISST_BUILD_TESTS)
  # Settings for CTest
  enable_testing ()
  mark_as_advanced (BUILD_TESTING) # CMake variable for CTest
  include (Dart)
  mark_as_advanced (DART_ROOT)
endif (CISST_BUILD_TESTS)

# Build static or dynamic libraries
option (CISST_BUILD_SHARED_LIBS "Build cisst with shared libraries." OFF)
if (CISST_BUILD_SHARED_LIBS)
  set (IS_SHARED SHARED)
  set (BUILD_SHARED_LIBS SHARED)
  if (WIN32)
    remove_definitions (-DCISST_DLL)
    add_definitions (-DCISST_DLL)
  endif (WIN32)
else (CISST_BUILD_SHARED_LIBS)
  set (IS_SHARED STATIC)
endif (CISST_BUILD_SHARED_LIBS)

# Set the version number
set (CISST_VERSION_MAJOR "1")
set (CISST_VERSION_MINOR "1")
set (CISST_VERSION_PATCH "0")
set (CISST_VERSION "${CISST_VERSION_MAJOR}.${CISST_VERSION_MINOR}.${CISST_VERSION_PATCH}")

# To compile examples within this project
set (CURRENT_PROJECT_IS_CISST ON)

# The upper case variables have been used in the past and need to be preserved
set (CISST_BINARY_DIR "${cisst_BINARY_DIR}")
set (CISST_SOURCE_DIR "${cisst_SOURCE_DIR}")
set (CISST_SHARE_INSTALL_SUFFIX "share/cisst-${CISST_VERSION_MAJOR}.${CISST_VERSION_MINOR}")
set (CISST_SHARE_DIR  "${CISST_BINARY_DIR}/${CISST_SHARE_INSTALL_SUFFIX}")
set (CISST_CMAKE_INSTALL_SUFFIX "${CISST_SHARE_INSTALL_SUFFIX}/cmake")
set (CISST_CMAKE_MODULES_INSTALL_SUFFIX "${CISST_SHARE_INSTALL_SUFFIX}/cmake/Modules")
set (CISST_CMAKE_SOURCE_DIR "${CISST_SOURCE_DIR}/cmake")
set (CISST_CMAKE_BINARY_DIR "${CISST_BINARY_DIR}/${CISST_CMAKE_INSTALL_SUFFIX}")
set (CISST_CMAKE_DIRS "${CISST_CMAKE_SOURCE_DIR}" "${CISST_CMAKE_BINARY_DIR}")

# Option to download and build as CMake External projects external dependencies when not found automatically
option (CISST_USE_EXTERNAL "Try to build dependencies using CMake external projects (download and build along cisst)" OFF)

# Add include directories
set (CISST_INCLUDE_DIR
     ${cisst_BINARY_DIR}/include
     ${cisst_SOURCE_DIR})
include_directories (${CISST_INCLUDE_DIR})

# Generate cisstRevision.h
find_package (Git QUIET)
set (CISST_REVISION_HEADER_SRC "${cisst_SOURCE_DIR}/cisstRevision.h.in")
set (CISST_REVISION_HEADER "${cisst_BINARY_DIR}/include/cisstRevision.h")
# Script to be executed everytime
configure_file ("${cisst_SOURCE_DIR}/cmake/cisstGenerateRevision.cmake.in"
                "${cisst_BINARY_DIR}/cisstGenerateRevision.cmake"
                @ONLY)
add_custom_target (cisstRevision ALL
                   COMMAND ${CMAKE_COMMAND} -P "${cisst_BINARY_DIR}/cisstGenerateRevision.cmake"
                   COMMENT "Generating ${CISST_REVISION_HEADER} from ${CISST_REVISION_HEADER_SRC}"
                   SOURCES "${CISST_REVISION_HEADER_SRC}"
                           "${CISST_REVISION_HEADER}"
                           "${cisst_SOURCE_DIR}/cmake/cisstGenerateRevision.cmake.in"
                           "${cisst_BINARY_DIR}/cisstGenerateRevision.cmake")
set_property (TARGET cisstRevision PROPERTY FOLDER "cisstCommon")

# Install cisstRevision.h
set_source_files_properties ("${CISST_REVISION_HEADER}" GENERATED)
install (FILES "${CISST_REVISION_HEADER}"
         DESTINATION include
         COMPONENT cisstCommon)

# Generate cisstBuildType.h
set (CISST_BUILDTYPE_HEADER_SRC "${cisst_SOURCE_DIR}/cisstBuildType.h.in")
set (CISST_BUILDTYPE_HEADER "${cisst_BINARY_DIR}/include/cisstBuildType.h")
# Script to be executed everytime
configure_file ("${cisst_SOURCE_DIR}/cmake/cisstGenerateBuildType.cmake.in"
                "${cisst_BINARY_DIR}/cisstGenerateBuildType.cmake"
                @ONLY)
add_custom_target (cisstBuildType ALL
                   COMMAND ${CMAKE_COMMAND}
                           -DCISST_BUILD_TYPE:STRING="${CMAKE_BUILD_TYPE}"
                           -DCISST_CFG_INTDIR:STRING="${CMAKE_CFG_INTDIR}"
                           -P "${cisst_BINARY_DIR}/cisstGenerateBuildType.cmake"
                   COMMENT "Generating ${CISST_BUILDTYPE_HEADER} from ${CISST_BUILDTYPE_HEADER_SRC}"
                   SOURCES "${CISST_BUILDTYPE_HEADER_SRC}"
                           "${CISST_BUILDTYPE_HEADER}"
                           "${cisst_SOURCE_DIR}/cmake/cisstGenerateBuildType.cmake.in"
                           "${cisst_BINARY_DIR}/cisstGenerateBuildType.cmake")
set_property (TARGET cisstBuildType PROPERTY FOLDER "cisstCommon")

# Install cisstBuildType.h
set_source_files_properties ("${CISST_BUILDTYPE_HEADER}" GENERATED)
install (FILES "${CISST_BUILDTYPE_HEADER}"
         DESTINATION include
         COMPONENT cisstCommon)

# Option to make cisst CMake functions more verbose
option (CISST_HAS_CMAKE_DEBUG "Print extra messages when configuring with CMake" OFF)
mark_as_advanced (CISST_HAS_CMAKE_DEBUG)


# Function to expend a list without repetitions, forces the list to be internal
function (CISST_ADD_TO_INTERNAL list newElements)
  message (SEND_ERROR "cisst_add_to_internal is now deprecated, use cisst_set_package_settings instead")
endfunction (CISST_ADD_TO_INTERNAL)

# Where to find cisst defined CMake Macros/Modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CISST_CMAKE_SOURCE_DIR}")


# Include all useful CMake macros and functions for cisst
include (cisstMacros)
include (cisstInternalMacros)

# Set CISST_CATKIN_BUILT
cisst_is_catkin_build (CISST_CATKIN_BUILT)

# Destination for CMake package config files
if (CISST_CATKIN_BUILT)
  # Output directories
  set (CISST_PACKAGE_CONFIG_DIRECTORY "${CATKIN_DEVEL_PREFIX}/cmake")
  set (EXECUTABLE_OUTPUT_PATH "${CATKIN_DEVEL_PREFIX}/bin")
  set (LIBRARY_OUTPUT_PATH    "${CATKIN_DEVEL_PREFIX}/lib")
  # Options defaults
  set (CISST_cisstMesh         ON  CACHE BOOL "")
  set (CISST_HAS_JSON          ON  CACHE BOOL "")
  set (CISST_HAS_SWIG_PYTHON   ON  CACHE BOOL "" FORCE)
  set (CISST_BUILD_SHARED_LIBS ON  CACHE BOOL "" FORCE)
  set (CISST_USE_SI_UNITS      ON  CACHE BOOL "")
  set (IS_SHARED SHARED)
  set (BUILD_SHARED_LIBS SHARED)
else ()
  # Output directories
  set (CISST_PACKAGE_CONFIG_DIRECTORY "${cisst_BINARY_DIR}")
  set (EXECUTABLE_OUTPUT_PATH "${cisst_BINARY_DIR}/bin")
endif ()

# Output directories
set (CISST_LIBRARY_INSTALL_SUFFIX "lib")
set (LIBRARY_OUTPUT_PATH "${cisst_BINARY_DIR}/${CISST_LIBRARY_INSTALL_SUFFIX}")

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
    set (CISST_XENOMAI_INCLUDE_DIRECTORIES ${XENOMAI_INCLUDE_DIR} )#${XENOMAI_INCLUDE_POSIX_DIR})
    cisst_set_package_settings (cisstCommon Xenomai INCLUDE_DIRECTORIES CISST_XENOMAI_INCLUDE_DIRECTORIES)
    # add libnative libxenomai librtdm libpthread_rt
    set (CISST_XENOMAI_LIBRARIES ${XENOMAI_LIBRARY_NATIVE}
#                                 ${XENOMAI_LIBRARY_PTHREAD_RT}
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

# Check on Apple only
if (APPLE)
  # User option for IOS
  option (CISST_HAS_IOS "Compile for IOS." OFF)
endif (APPLE)


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


# Determine is cisst is SI or not
option (CISST_USE_SI_UNITS "Use SI units, if not, distances are in millimeters (default if OFF)" OFF)
mark_as_advanced (CISST_USE_SI_UNITS)


# Determine if size_t is a native type
include (CheckSizeTNativeType)
check_size_t_native_type (CISST_SIZE_T_NATIVE)

# Determine if "long" and "long long" are different types (assume same is true
# for "unsigned long" and "unsigned long long")
include (CheckIfDifferentTypes)
check_if_different_types (CISST_LONG_LONG_NATIVE "long long" "long")

# Determine if we can use std::isnan and std::isfinite
include (CheckCXXSourceCompiles)
check_cxx_source_compiles (
  "#include <cmath>
   int main(int argc, char ** argv) {
       return std::isnan(1.0 / 0.0);
   }" CISST_HAS_STD_ISNAN)
check_cxx_source_compiles (
  "#include <cmath>
   int main(int argc, char ** argv) {
       float f = 1.0;
       double d = 1.0;
       return (std::isfinite(f) && std::isfinite(d));
   }" CISST_HAS_STD_ISFINITE)

# Look for isfinite function, this will only be used if not CISST_HAS_STD_ISFINITE
include (CheckSymbolExists)
check_symbol_exists (isfinite "math.h" CISST_HAS_ISFINITE)


# Path of all CISST shared libraries.
link_directories (${LIBRARY_OUTPUT_PATH})

# Determine if we want to use rpath, no by default
if (UNIX)
  option (CMAKE_SKIP_RPATH "Do NOT use rpath to hard code the path to dynamic libraries." ON)
  mark_as_advanced (FORCE CMAKE_SKIP_RPATH)
endif (UNIX)


# Add include directories
set (CISST_INCLUDE_DIR
     ${cisst_BINARY_DIR}/include
     ${cisst_SOURCE_DIR})
include_directories (${CISST_INCLUDE_DIR})

# Build test driver if needed
if (CISST_BUILD_TESTS)
  find_package (CppUnit REQUIRED)
  if (NOT CPPUNIT_FOUND)
    # Try to build as external project
    if (CISST_USE_EXTERNAL)
      include (ExternalProject)
      set (cisstCppUnit_PREFIX cisstCppUnitExternal)
      set (CISST_CPPUNIT_GIT_REPOSITORY https://github.com/jhu-cisst-external/cppunit.git)
      set (CISST_CPPUNIT_GIT_TAG cppunit-1.13.3)

      ExternalProject_Add (cisstCppUnit
                           PREFIX ${cisstCppUnit_PREFIX}
                           GIT_REPOSITORY    ${CISST_CPPUNIT_GIT_REPOSITORY}
                           GIT_TAG           ${CISST_CPPUNUT_GIT_TAG}
                           CMAKE_CACHE_ARGS -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
                                            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
                           INSTALL_COMMAND ""
                           UPDATE_COMMAND ""
                           )
      set (CPPUNIT_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/${cisstCppUnit_PREFIX}/src/cisstCppUnit/include")
      set (CPPUNIT_LIBRARIES "cppunit")
      set (CPPUNIT_DEBUG_LIBRARIES "cppunit")
      set (CPPUNIT_RELEASE_LIBRARIES "cppunit")
      set (CPPUNIT_LINK_DIR "${CMAKE_CURRENT_BINARY_DIR}/${cisstCppUnit_PREFIX}/src/cisstCppUnit-build")
      link_directories (${CPPUNIT_LINK_DIR})
      set (CPPUNIT_FOUND ON)
    endif (CISST_USE_EXTERNAL)
  endif (NOT CPPUNIT_FOUND)

  if (CPPUNIT_FOUND)
    # set variable to confirm that cisstTestsDriver is being build
    set (CISST_cisstTestsDriver ON)
    # save settings for cisstTestsDriver
    cisst_set_package_settings (cisstTestsDriver CppUnit INCLUDE_DIRECTORIES CPPUNIT_INCLUDE_DIR)
    cisst_set_package_settings (cisstTestsDriver CppUnit LIBRARIES CPPUNIT_LIBRARIES)
    # when using external package, we defined CPPUNIT_LINK_DIR.   If CppUnit is installed on the host
    # (e.g. debian package), CPPUNIT_INCLUDE_DIR is defined but not CPPUNIT_LINK_DIR
    if (DEFINED CPPUNIT_LINK_DIR)
      cisst_set_package_settings (cisstTestsDriver CppUnit LINK_DIRECTORIES CPPUNIT_LINK_DIR)
    endif ()
  else (CPPUNIT_FOUND)
    message (SEND_ERROR "CppUnit is needed for CISST_BUILD_TESTS, you can either install it or turn ON the option CISST_USE_EXTERNAL")
  endif (CPPUNIT_FOUND)
else (CISST_BUILD_TESTS)
  cisst_unset_all_package_settings (cisstTestsDriver CppUnit)
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
     )

set (CISST_LIBRARIES_OPTIONAL
     cisstMesh
     cisstInteractive
     cisstStereoVision
     cisst3DUserInterface
     )

# All libraries
set (CISST_LIBS
     ${CISST_LIBRARIES_DEFAULT}
     ${CISST_LIBRARIES_OPTIONAL})

# Add test driver if needed
if (CISST_BUILD_TESTS)
  set (CISST_LIBS ${CISST_LIBS} cisstTestsDriver)
endif (CISST_BUILD_TESTS)

# Which librairies have Qt Widgets
set (CISST_LIBS_WITH_QT
     cisstCommon
     cisstVector
     cisstMultiTask
     cisstParameterTypes
     cisstStereoVision)

# Which librairies have FLTK Widgets
set (CISST_LIBS_WITH_FLTK
     cisstVector)


# Build by default
foreach (lib ${CISST_LIBRARIES_DEFAULT})
  option (CISST_${lib} "Build ${lib}." ON)
endforeach (lib)

foreach (lib ${CISST_LIBRARIES_OPTIONAL})
  option (CISST_${lib} "Build ${lib}." OFF)
endforeach (lib)

# If OS abstraction is compiled, require dynamic loading libraries
if (CISST_cisstOSAbstraction OR CISST_cisstInteractive)
  if (CMAKE_DL_LIBS)
    cisst_set_package_settings (cisstOSAbstraction DL LIBRARIES CMAKE_DL_LIBS)
    cisst_set_package_settings (cisstInteractive DL LIBRARIES CMAKE_DL_LIBS)
  endif (CMAKE_DL_LIBS)
endif (CISST_cisstOSAbstraction OR CISST_cisstInteractive)

# If cisstInteractive in turned on, also force SWIG for Python and shared libraries
if (CISST_cisstInteractive)
  set (CISST_HAS_SWIG_PYTHON ON CACHE BOOL "Build the Python wrappers for cisst with Swig." FORCE)
endif (CISST_cisstInteractive)
if (CISST_HAS_SWIG_PYTHON)
  set (CISST_BUILD_SHARED_LIBS ON CACHE BOOL "Build cisst with shared libraries." FORCE)
endif (CISST_HAS_SWIG_PYTHON)


# List of libraries to be built
foreach (lib ${CISST_LIBS})
  if (CISST_${lib})
    set (CISST_LIBRARIES ${lib} ${CISST_LIBRARIES})
    # Add libraries with Qt
    if (CISST_HAS_QT)
      list (FIND CISST_LIBS_WITH_QT ${lib} ${lib}_HAS_QT)
      if (${${lib}_HAS_QT} GREATER -1)
        set (CISST_LIBRARIES ${lib}Qt ${CISST_LIBRARIES})
        set (CISST_${lib}Qt ON)
      else (${${lib}_HAS_QT} GREATER -1)
        set (CISST_${lib}Qt OFF)
      endif (${${lib}_HAS_QT} GREATER -1)
    endif (CISST_HAS_QT)
    # Add libraries with FLTK
    if (CISST_HAS_FLTK)
      list (FIND CISST_LIBS_WITH_FLTK ${lib} ${lib}_HAS_FLTK)
      if (${${lib}_HAS_FLTK} GREATER -1)
        set (CISST_LIBRARIES ${lib}FLTK ${CISST_LIBRARIES})
        set (CISST_${lib}FLTK ON)
      else (${${lib}_HAS_FLTK} GREATER -1)
        set (CISST_${lib}FLTK OFF)
      endif (${${lib}_HAS_FLTK} GREATER -1)
    endif (CISST_HAS_FLTK)
  endif (CISST_${lib})
endforeach (lib)

# create equivalent for cisstInteractive
set (CISST_HAS_IRE ${CISST_cisstInteractive})

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
  if (CISST_${lib})
    add_subdirectory (${lib})
  endif (CISST_${lib})
endforeach (lib)


# Create misc. config files
set (CISST_INCLUDE_DIR_FOR_CONFIG ${CISST_INCLUDE_DIR})
set (CISST_BINARY_DIR_FOR_CONFIG ${CISST_BINARY_DIR})
set (CISST_CMAKE_DIRS_FOR_CONFIG ${CISST_CMAKE_BINARY_DIR})

set (CISST_BUILD_DOXYGEN_DIRECTORY ${cisst_SOURCE_DIR})
set (CISST_BUILD_LINK_DIRECTORIES ${cisst_LIBRARY_PATH})

# Config file
configure_file ("${CISST_CMAKE_SOURCE_DIR}/cisst-config.cmake.in"
                "${CISST_PACKAGE_CONFIG_DIRECTORY}/cisst-config.cmake"
                @ONLY)

# Version file
cisst_add_config_version ("${CISST_PACKAGE_CONFIG_DIRECTORY}/cisst-config-version.cmake"
                          VERSION ${CISST_VERSION}
                          DESTINATION ${CISST_CMAKE_INSTALL_SUFFIX}
                          COMPONENT cisstCommon)

# Modify some settings just for the installed version of cisst-config.cmake
set (CISST_BINARY_DIR_FOR_CONFIG "")
set (CISST_INCLUDE_DIR_FOR_CONFIG "")
set (CISST_CMAKE_DIRS_FOR_CONFIG "")
configure_file ("${CISST_CMAKE_SOURCE_DIR}/cisst-config.cmake.in"
                "${cisst_BINARY_DIR}/cisst-config.cmake-to-be-installed"
                @ONLY)
install (FILES "${cisst_BINARY_DIR}/cisst-config.cmake-to-be-installed"
         DESTINATION ${CISST_CMAKE_INSTALL_SUFFIX}
         RENAME "cisst-config.cmake"
         COMPONENT cisstCommon)

# CMake use cisst file
set (CISST_CMAKE_FILES_TO_COPY
     Usecisst.cmake cisstSettings.cmake cisstMacros.cmake cisstConfigVersion.cmake.in
     sawRevision.h.in sawConfig.cmake.in sawConfigVersion.cmake.in)
foreach (cmake_file_to_copy ${CISST_CMAKE_FILES_TO_COPY})
  # copy to build for users not installing
  configure_file ("${CISST_CMAKE_SOURCE_DIR}/${cmake_file_to_copy}"
                  "${CISST_CMAKE_BINARY_DIR}/${cmake_file_to_copy}"
                  COPYONLY)
  # and install
  install (FILES "${CISST_CMAKE_BINARY_DIR}/${cmake_file_to_copy}"
           DESTINATION ${CISST_CMAKE_INSTALL_SUFFIX}
           COMPONENT cisstCommon)
endforeach ()

# Configuration file
set (CISST_SOURCE_DIR ${cisst_SOURCE_DIR})
configure_file (${cisst_SOURCE_DIR}/cisstConfig.h.in
                ${cisst_BINARY_DIR}/include/cisstConfig.h)
install (FILES  ${cisst_BINARY_DIR}/include/cisstConfig.h
         DESTINATION include
         COMPONENT cisstCommon)

# Clean files
set_directory_properties (PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
  "${CISST_REVISION_HEADER};${CISST_BUILDTYPE_HEADER};${cisst_BINARY_DIR}/include/cisstConfig.h")

# Install auxiliary Find*.cmake files
install (DIRECTORY ${CISST_CMAKE_SOURCE_DIR}/
         DESTINATION ${CISST_CMAKE_MODULES_INSTALL_SUFFIX}
         COMPONENT cisstCommon
         FILES_MATCHING PATTERN "Find*.cmake"
         PATTERN ".svn" EXCLUDE)

file (COPY ${CISST_CMAKE_SOURCE_DIR}/
      DESTINATION ${CISST_CMAKE_MODULES_INSTALL_SUFFIX}
      FILES_MATCHING PATTERN "Find*.cmake"
      PATTERN ".svn" EXCLUDE)

# Use CPack package generator
if (NOT UNIX)
  set (CPACK_PACKAGE_NAME "cisst")
  set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "cisst libraries")
endif (NOT UNIX)

if (UNIX)
  set (CPACK_GENERATOR "STGZ;TGZ;DEB")
  set (CPACK_DEBIAN_PACKAGE_MAINTAINER "anton.deguet@jhu.edu")
  set (CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
endif ()

# For now, enable all default libraries (even if not compiled)
set (CPACK_COMPONENTS_ALL ${CISST_LIBRARIES_DEFAULT})
if (CISST_cisstCommonXML) # we can only test CISST_XML_LIB if cisstCommonXML is activated
  if (${CISST_XML_LIB} STREQUAL "LibXml2")
    set (CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL} cisstCommonXML)
  endif (${CISST_XML_LIB} STREQUAL "LibXml2")
endif (CISST_cisstCommonXML)
set (CPACK_PACKAGE_VENDOR "JHU")
set (CPACK_PACKAGE_VERSION "${CISST_VERSION}")
set (CPACK_PACKAGE_VERSION_MAJOR ${CISST_VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${CISST_VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${CISST_VERSION_PATCH})
set (CPACK_RESOURCE_FILE_LICENSE "${cisst_SOURCE_DIR}/license.txt")
set (CPACK_PACKAGE_INSTALL_DIRECTORY "cisst")
set (CPACK_COMPONENT_CISSTCOMMON_REQUIRED ON)
# Dependencies should be done automatically (don't really need to specify
# cisstCommon dependency because it is a required component)
set (CPACK_COMPONENT_CISSTCOMMONXML_DEPENDS       cisstCommon)
set (CPACK_COMPONENT_CISSTVECTOR_DEPENDS          cisstCommon)
set (CPACK_COMPONENT_CISSTNUMERICAL_DEPENDS       cisstCommon cisstVector)
set (CPACK_COMPONENT_CISSTOSABSTRACTION_DEPENDS   cisstCommon)
set (CPACK_COMPONENT_CISSTMULTITASK_DEPENDS       cisstCommon cisstVector cisstOSAbstraction)
set (CPACK_COMPONENT_CISSTSTEREOVISION_DEPENDS    cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)
set (CPACK_COMPONENT_CISSTPARAMETERTYPES_DEPENDS  cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)
set (CPACK_COMPONENT_CISSTROBOT_DEPENDS           cisstCommon cisstVector cisstNumerical)
if (WIN32)
  set(CPACK_NSIS_MODIFY_PATH ON)
endif (WIN32)

include (CPack)

# Environment variables for Windows
if (WIN32)
  # Script to set cisst environment variables
  file (TO_NATIVE_PATH ${cisst_BINARY_DIR} cisst_BINARY_DIR_NATIVE)
  configure_file (${cisst_SOURCE_DIR}/utils/cisstvars.bat.in
                  ${cisst_BINARY_DIR}/cisstvars.bat)
  # Script to set Microsoft compiler environment variables
  get_filename_component (BUILD_TOOL_DIRECTORY ${CMAKE_MAKE_PROGRAM} PATH)
  find_program (VSVARS32_BAT
                NAMES vsvars32.bat
                PATHS ${BUILD_TOOL_DIRECTORY}/../../Common7/Tools)
  mark_as_advanced (VSVARS32_BAT)
  # vcvarsall.bat can be used instead of vsvars32.bat
  #   vcvarsall  [x86]    -- defaults to x86 (calls vsvars32.bat)
  #   vcvarsall x86_amd64 -- cross-compile for x64
  #   vcvarsall x86_ia64  -- cross-compile for Itanium
  #   vcvarsall amd64     -- set up for x64
  #   vcvarsall ia64      -- set up for Itanium
  set (VCVARSALL_CONFIG "x86")
  if (CMAKE_CL_64)
     # For now, assume x64. Need to change to also support Itanium
     set (VCVARSALL_CONFIG "amd64")
  endif (CMAKE_CL_64)
  find_program (VCVARSALL_BAT
                NAMES vcvarsall.bat
                PATHS ${BUILD_TOOL_DIRECTORY}/../../VC)
  mark_as_advanced (VCVARSALL_BAT)
  configure_file (${cisst_SOURCE_DIR}/utils/cisstvsvars.bat.in
                  ${cisst_BINARY_DIR}/cisstvsvars.bat)

  # If Visual Studio
  if (CMAKE_GENERATOR MATCHES "Visual Studio*" )
    foreach (config ${CMAKE_CONFIGURATION_TYPES})
      configure_file (${cisst_SOURCE_DIR}/utils/visual-studio.bat.in
                      ${cisst_BINARY_DIR}/visual-studio-${config}.bat)
    endforeach (config)
  endif (CMAKE_GENERATOR MATCHES "Visual Studio*" )

  # Offer to register the cisst paths
  option (CISST_PATH_REGISTER "Register the paths to cisst libraries" OFF)
  if (CISST_PATH_REGISTER)

    # Set and test configuration (i.e. Debug, Release, ...)
    set (CISST_PATH_CONFIGURATION CACHE STRING "Configuration used for the paths")
    set (CISST_PATH_CONFIGURATION_VALID FALSE)

    foreach (config ${CMAKE_CONFIGURATION_TYPES})
      if ("${config}" STREQUAL "CISST_PATH_CONFIGURATION")
        set (CISST_PATH_CONFIGURATION_VALID TRUE)
      endif ("${config}" STREQUAL "CISST_PATH_CONFIGURATION")
    endforeach (config)

    # Configuration is valid, build the path and compare it to the current one
    if (CISST_PATH_CONFIGURATION_VALID)

      # Initial settings for paths
      set (CISST_PATH_CMAKE_FORMAT
           "${cisst_BINARY_DIR}/lib/${CISST_PATH_CONFIGURATION}"
           "${cisst_BINARY_DIR}/bin/${CISST_PATH_CONFIGURATION}")
      set (CISST_PYTHONPATH_CMAKE_FORMAT
           "${cisst_BINARY_DIR}/lib" "${cisst_BINARY_DIR}/lib/${CISST_PATH_CONFIGURATION}"
           "${cisst_BINARY_DIR}/bin" "${cisst_BINARY_DIR}/bin/${CISST_PATH_CONFIGURATION}")

      set (CISST_ROOT_CMAKE_FORMAT
           "${cisst_BINARY_DIR}")

      # Convert to native format to be able to compare
      file (TO_NATIVE_PATH "${CISST_PATH_CMAKE_FORMAT}" CISST_PATH)
      file (TO_NATIVE_PATH "${CISST_PYTHONPATH_CMAKE_FORMAT}" CISST_PYTHONPATH)
      file (TO_NATIVE_PATH "${CISST_ROOT_CMAKE_FORMAT}" CISST_ROOT)

      # Retrieve current path, little trick to read register key/value
      set (CISST_PATH_REG_KEY "HKEY_CURRENT_USER\\Environment;CISST_PATH")
      get_filename_component (CURRENT_CISST_PATH_CMAKE_FORMAT
                              "[HKEY_CURRENT_USER\\Environment;CISST_PATH]\\dummy.dum"
                              PATH)
      file (TO_NATIVE_PATH "${CURRENT_CISST_PATH_CMAKE_FORMAT}" CURRENT_CISST_PATH)

      set (CISST_PYTHONPATH_REG_KEY "HKEY_CURRENT_USER\\Environment;CISST_PYTHONPATH")
      get_filename_component (CURRENT_CISST_PYTHONPATH_CMAKE_FORMAT
                              "[HKEY_CURRENT_USER\\Environment;CISST_PYTHONPATH]\\dummy.dum"
                              PATH)
      file (TO_NATIVE_PATH "${CURRENT_CISST_PYTHONPATH_CMAKE_FORMAT}" CURRENT_CISST_PYTHONPATH)

      set (CISST_ROOT_REG_KEY "HKEY_CURRENT_USER\\Environment;CISST_ROOT")
      get_filename_component (CURRENT_CISST_ROOT_CMAKE_FORMAT
                              "[HKEY_CURRENT_USER\\Environment;CISST_ROOT]\\dummy.dum"
                              PATH)
      file (TO_NATIVE_PATH "${CURRENT_CISST_ROOT_CMAKE_FORMAT}" CURRENT_CISST_ROOT)

      # If there is no existing path, add it
      if ("${CURRENT_CISST_PATH}" STREQUAL "\\registry")
        # not defined, add it to register using cmake -E
        exec_program (${CMAKE_COMMAND}
                      ARGS "-E write_regv \"${CISST_PATH_REG_KEY}\" \"${CISST_PATH}\"")
      else ("${CURRENT_CISST_PATH}" STREQUAL "\\registry")
        if ("${CISST_PATH}" STREQUAL "${CURRENT_CISST_PATH}")
          # message ("CISST_PATH is the same")
        else ("${CISST_PATH}" STREQUAL "${CURRENT_CISST_PATH}")
          # The variable already exists and is different
          option (CISST_PATH_OVERWRITE "Overwrite existing CISST_PATH" OFF)
          if (CISST_PATH_OVERWRITE)
            # add it to register using cmake -E
            exec_program (${CMAKE_COMMAND}
                          ARGS "-E write_regv \"${CISST_PATH_REG_KEY}\" \"${CISST_PATH}\"")
            # Set overwrite back to OFF by default.
            set (CISST_PATH_OVERWRITE OFF CACHE BOOL "Overwrite existing CISST_PATH" FORCE)
          else (CISST_PATH_OVERWRITE)
            # Display a message of information
            message (SEND_ERROR "A different CISST_PATH exists:\n-- current: ${CURRENT_CISST_PATH}\n-- new: ${CISST_PATH}\n-- turn CISST_PATH_OVERWRITE \"ON\" or CISST_PATH_REGISTER \"OFF\"")
          endif (CISST_PATH_OVERWRITE)
        endif ("${CISST_PATH}" STREQUAL "${CURRENT_CISST_PATH}")

      endif ("${CURRENT_CISST_PATH}" STREQUAL "\\registry")

      # Do the same for PYTHONPATH if wrappers are compiled
      if (CISST_HAS_SWIG_PYTHON)
        # If there is no existing path, add it
        if ("${CURRENT_CISST_PYTHONPATH}" STREQUAL "\\registry")
          # not defined, add it to register using cmake -E
          exec_program (${CMAKE_COMMAND}
                        ARGS "-E write_regv \"${CISST_PYTHONPATH_REG_KEY}\" \"${CISST_PYTHONPATH}\"")
        else ("${CURRENT_CISST_PYTHONPATH}" STREQUAL "\\registry")
          if ("${CISST_PYTHONPATH}" STREQUAL "${CURRENT_CISST_PYTHONPATH}")
            # message ("CISST_PATH is the same")
          else ("${CISST_PYTHONPATH}" STREQUAL "${CURRENT_CISST_PYTHONPATH}")
            # The variable already exists and is different
            option (CISST_PYTHONPATH_OVERWRITE "Overwrite existing CISST_PYTHONPATH" OFF)
            if (CISST_PYTHONPATH_OVERWRITE)
              # Add it to register using cmake -E
              exec_program (${CMAKE_COMMAND}
                            ARGS "-E write_regv \"${CISST_PYTHONPATH_REG_KEY}\" \"${CISST_PYTHONPATH}\"")
              # Set overwrite back to OFF by default.
              set (CISST_PYTHONPATH_OVERWRITE OFF CACHE BOOL "Overwrite existing CISST_PYTHONPATH" FORCE)
            else (CISST_PYTHONPATH_OVERWRITE)
              # Display a message of information
              message (SEND_ERROR "A different CISST_PYTHONPATH exists:\n-- current: ${CURRENT_CISST_PYTHONPATH}\n-- new: ${CISST_PYTHONPATH}\n-- turn CISST_PYTHONPATH_OVERWRITE \"ON\" or CISST_PATH_REGISTER \"OFF\"")
            endif (CISST_PYTHONPATH_OVERWRITE)
          endif ("${CISST_PYTHONPATH}" STREQUAL "${CURRENT_CISST_PYTHONPATH}")

        endif ("${CURRENT_CISST_PYTHONPATH}" STREQUAL "\\registry")
      endif (CISST_HAS_SWIG_PYTHON)


      # If there is no CISST_ROOT
      if ("${CURRENT_CISST_ROOT}" STREQUAL "\\registry")
        # not defined, add it to register using cmake -E
        exec_program (${CMAKE_COMMAND}
                      ARGS "-E write_regv \"${CISST_ROOT_REG_KEY}\" \"${CISST_ROOT}\"")
      else ("${CURRENT_CISST_ROOT}" STREQUAL "\\registry")
        if ("${CISST_ROOT}" STREQUAL "${CURRENT_CISST_ROOT}")
          # message ("CISST_ROOT is the same")
        else ("${CISST_ROOT}" STREQUAL "${CURRENT_CISST_ROOT}")
          # The variable already exists and is different
          option (CISST_ROOT_OVERWRITE "Overwrite existing CISST_ROOT" OFF)
          if (CISST_ROOT_OVERWRITE)
            # add it to register using cmake -E
            exec_program (${CMAKE_COMMAND}
                          ARGS "-E write_regv \"${CISST_ROOT_REG_KEY}\" \"${CISST_ROOT}\"")
            # Set overwrite back to OFF by default.
            set (CISST_ROOT_OVERWRITE OFF CACHE BOOL "Overwrite existing CISST_ROOT" FORCE)
          else (CISST_ROOT_OVERWRITE)
            # Display a message of information
            message (SEND_ERROR "A different CISST_ROOT exists:\n-- current: ${CURRENT_CISST_ROOT}\n-- new: ${CISST_ROOT}\n-- turn CISST_ROOT_OVERWRITE \"ON\" or CISST_PATH_REGISTER \"OFF\"")
          endif (CISST_ROOT_OVERWRITE)
        endif ("${CISST_ROOT}" STREQUAL "${CURRENT_CISST_ROOT}")

      endif ("${CURRENT_CISST_ROOT}" STREQUAL "\\registry")

    else (CISST_PATH_CONFIGURATION_VALID)
      message (SEND_ERROR "CISST_PATH_CONFIGURATION must be one of ${CMAKE_CONFIGURATION_TYPES}")
    endif (CISST_PATH_CONFIGURATION_VALID)

  endif (CISST_PATH_REGISTER)

endif (WIN32)

# Environment variables for Unix
if (UNIX)
  if (CISST_CATKIN_BUILT)
    set (CISST_SCRIPTS_DIRECTORY "${CATKIN_DEVEL_PREFIX}")
  else ()
    set (CISST_SCRIPTS_DIRECTORY "${cisst_BINARY_DIR}")
  endif ()

  # Script to set cisst environment variables
  configure_file ("${cisst_SOURCE_DIR}/utils/cisstvars.csh.in"
                  "${CISST_SCRIPTS_DIRECTORY}/cisstvars.csh"
                  @ONLY)
  # Script to set cisst environment variables
  configure_file ("${cisst_SOURCE_DIR}/utils/cisstvars.sh.in"
                  "${CISST_SCRIPTS_DIRECTORY}/cisstvars.sh"
                  @ONLY)
endif (UNIX)

# License file
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/license.txt
                ${CMAKE_CURRENT_BINARY_DIR}/license.txt COPYONLY)

# Copy shared data if git submodule is checked-out properly
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/share/CMakeLists.txt")
  add_subdirectory (share)
endif ()
