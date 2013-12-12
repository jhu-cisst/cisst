#
# $Id$
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


# Backward compatibility
cmake_minimum_required (VERSION 2.6.2)

# cisst libraries (Computer Integrated Surgical Systems and Technology)
project (cisst)

# Set the version number
set (CISST_VERSION_MAJOR "1")
set (CISST_VERSION_MINOR "0")
set (CISST_VERSION "${CISST_VERSION_MAJOR}.${CISST_VERSION_MINOR}")

# To compile examples within this project
set (CURRENT_PROJECT_IS_CISST ON)

# The upper case variables have been used in the past and need to be preserved
set (CISST_BINARY_DIR "${cisst_BINARY_DIR}")
set (CISST_SOURCE_DIR "${cisst_SOURCE_DIR}")
set (CISST_SHARE_INSTALL_SUFFIX "share/cisst-${CISST_VERSION}")
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

# Try to find SVN version number
find_package (Subversion QUIET)
set (CISST_WC_REVISION "undefined")
set (CISST_REVISION_SVN_DIR "${cisst_saw_SOURCE_DIR}")
if (Subversion_FOUND AND EXISTS "${CISST_REVISION_SVN_DIR}/.svn/")
  Subversion_WC_INFO ("${CISST_REVISION_SVN_DIR}" CISST)
endif (Subversion_FOUND AND EXISTS "${CISST_REVISION_SVN_DIR}/.svn/")
message ("Configuring cisst ${CISST_VERSION} from \"${cisst_SOURCE_DIR}\", revision: ${CISST_WC_REVISION}")

# Generate cisstRevision.h
set (CISST_REVISION_HEADER_SRC "${cisst_SOURCE_DIR}/cisstRevision.h.in")
set (CISST_REVISION_HEADER "${cisst_BINARY_DIR}/include/cisstRevision.h")
# Script to be executed everytime
configure_file ("${cisst_SOURCE_DIR}/cmake/cisstGenerateRevision.cmake.in"
                "${cisst_BINARY_DIR}/cisstGenerateRevision.cmake")
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
install (FILES "${cisst_BINARY_DIR}/include/cisstRevision.h"
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
install (FILES "${cisst_BINARY_DIR}/include/cisstBuildType.h"
         DESTINATION include
         COMPONENT cisstCommon)


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
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CISST_CMAKE_SOURCE_DIR}")


# Include all useful CMake macros and functions for cisst
include (cisstMacros)
include (cisstInternalMacros)

# Output directories.
set (CISST_LIBRARY_INSTALL_SUFFIX "lib")
set (LIBRARY_OUTPUT_PATH "${cisst_BINARY_DIR}/${CISST_LIBRARY_INSTALL_SUFFIX}")
set (EXECUTABLE_OUTPUT_PATH ${cisst_BINARY_DIR}/bin)

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


# Determine if size_t is a native type
include (CheckSizeTNativeType)
check_size_t_native_type (CISST_SIZE_T_NATIVE)


# Determine if we can use cast to void * for CMN_LOG
include (CheckCXXSourceCompiles)
check_cxx_source_compiles(
"#include <iostream>
int main(int argc, char ** argv) {
    true ? (void *) 0 : std::cerr << \"true\";
    false ? (void *) 0 : std::cerr << \"false\";
    return 0;
}" CISST_OSTREAM_CAN_CAST_TO_VOID_PTR)


# Look for isfinite function
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
      ExternalProject_Add (cisstCppUnit
                           PREFIX ${cisstCppUnit_PREFIX}
                           URL http://downloads.sourceforge.net/cppunit/cppunit-1.12.1.tar.gz
                           URL_MD5 bd30e9cf5523cdfc019b94f5e1d7fd19
                           CMAKE_CACHE_ARGS -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
                                            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
                           PATCH_COMMAND ${CMAKE_COMMAND} -E copy_directory
                                         ${CMAKE_CURRENT_SOURCE_DIR}/cisstTestsDriver/cppunit-patch
                                         "."
                           INSTALL_COMMAND ""
                           )
      set (CPPUNIT_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/${cisstCppUnit_PREFIX}/src/cisstCppUnit/include")
      set (CPPUNIT_LIBRARIES "cppunit")
      set (CPPUNIT_DEBUG_LIBRARIES "cppunit")
      set (CPPUNIT_RELEASE_LIBRARIES "cppunit")
      link_directories ("${CMAKE_CURRENT_BINARY_DIR}/${cisstCppUnit_PREFIX}/src/cisstCppUnit-build")
      set (CPPUNIT_FOUND ON)
    endif (CISST_USE_EXTERNAL)
  endif (NOT CPPUNIT_FOUND)

  if (CPPUNIT_FOUND)
    # set variable to confirm that cisstTestsDriver is being build
    set (CISST_cisstTestsDriver ON)
    # save settings for cisstTestsDriver
    cisst_set_package_settings (cisstTestsDriver CppUnit INCLUDE_DIRECTORIES CPPUNIT_INCLUDE_DIR)
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
     cisstStereoVision
     )

set (CISST_LIBRARIES_OPTIONAL
     cisstInteractive
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

# Add cisstCommonXML if needed
if (CISST_cisstCommonXML)
  set (CISST_LIBRARIES cisstCommonXML ${CISST_LIBRARIES})
endif (CISST_cisstCommonXML)

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

configure_file ("${CISST_CMAKE_SOURCE_DIR}/cisst-config.cmake.in"
                "${cisst_BINARY_DIR}/cisst-config.cmake"
                @ONLY)

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
configure_file ("${CISST_CMAKE_SOURCE_DIR}/Usecisst.cmake"
                "${CISST_CMAKE_BINARY_DIR}/Usecisst.cmake"
                COPYONLY)
configure_file ("${CISST_CMAKE_SOURCE_DIR}/cisstMacros.cmake"
                "${CISST_CMAKE_BINARY_DIR}/cisstMacros.cmake"
                COPYONLY)
install (FILES "${CISST_CMAKE_BINARY_DIR}/Usecisst.cmake"
               "${CISST_CMAKE_BINARY_DIR}/cisstMacros.cmake"
         DESTINATION ${CISST_CMAKE_INSTALL_SUFFIX}
         COMPONENT cisstCommon)

# Configuration file
set (CISST_SOURCE_DIR ${cisst_SOURCE_DIR})
configure_file (${cisst_SOURCE_DIR}/cisstConfig.h.in
                ${cisst_BINARY_DIR}/include/cisstConfig.h)
install (FILES  ${cisst_BINARY_DIR}/include/cisstConfig.h
         DESTINATION include
         COMPONENT cisstCommon)

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
# For now, enable all default libraries (even if not compiled)
set (CPACK_COMPONENTS_ALL ${CISST_LIBRARIES_DEFAULT})
if (CISST_cisstCommonXML) # we can only test CISST_XML_LIB if cisstCommonXML is activated
  if (${CISST_XML_LIB} STREQUAL "LibXml2")
    set (CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL} cisstCommonXML)
  endif (${CISST_XML_LIB} STREQUAL "LibXml2")
endif (CISST_cisstCommonXML)
set (CPACK_PACKAGE_VENDOR "JHU")
set (CPACK_PACKAGE_VERSION "${CISST_VERSION}.${CISST_WC_REVISION}")
set (CPACK_PACKAGE_VERSION_MAJOR ${CISST_VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${CISST_VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${CISST_WC_REVISION})
set (CPACK_RESOURCE_FILE_LICENSE "${cisst_saw_SOURCE_DIR}/license.txt")
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
