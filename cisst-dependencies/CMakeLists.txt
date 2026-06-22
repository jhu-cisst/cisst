#
# (C) Copyright 2005-2026 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---


# Support for PkgConfig (used for JSON and Graphviz)
find_package (PkgConfig QUIET)
set (CISST_USE_PKG_CONFIG FALSE)
if (PKG_CONFIG_FOUND AND (NOT WIN32 OR DEFINED VCPKG_TARGET_TRIPLET OR DEFINED VCPKG_TOOLCHAIN_FILE OR DEFINED CMAKE_TOOLCHAIN_FILE OR VCPKG_TOOLCHAIN))
  set (CISST_USE_PKG_CONFIG TRUE)
endif ()

# Support for Graphviz
if (CISST_USE_PKG_CONFIG)
  pkg_check_modules (GRAPHVIZ libgvc libcgraph libcdt)

  if (GRAPHVIZ_FOUND)
    # Mark that we have graphviz for the main cisst CMakeLists.txt
    set (GRAPHVIZ_INCLUDE_DIRS ${GRAPHVIZ_INCLUDE_DIRS} PARENT_SCOPE)
    set (GRAPHVIZ_LIBRARY_DIRS ${GRAPHVIZ_LIBRARY_DIRS} PARENT_SCOPE)
    set (GRAPHVIZ_LIBRARIES ${GRAPHVIZ_LIBRARIES} PARENT_SCOPE)
    if (NOT TARGET cisstGraphviz)
      add_library (cisstGraphviz INTERFACE)
      target_include_directories (cisstGraphviz INTERFACE ${GRAPHVIZ_INCLUDE_DIRS})
      if (GRAPHVIZ_LIBRARY_DIRS)
        target_link_directories (cisstGraphviz INTERFACE ${GRAPHVIZ_LIBRARY_DIRS})
      endif ()
      target_link_libraries (cisstGraphviz INTERFACE ${GRAPHVIZ_LIBRARIES})
    endif ()
    set (CISST_HAS_GRAPHVIZ ON PARENT_SCOPE)
    set (CISST_HAS_GRAPHVIZ ON)
  else ()
    set (CISST_HAS_GRAPHVIZ OFF PARENT_SCOPE)
    set (CISST_HAS_GRAPHVIZ OFF)
  endif ()
endif (CISST_USE_PKG_CONFIG)

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
endif (CISST_HAS_OPENGL)


# Support for Qt

set (CISST_QT_VERSION "" CACHE STRING "Qt major version to use: empty or AUTO prefers Qt6 then Qt5; otherwise use 6 or 5")
set_property (CACHE CISST_QT_VERSION PROPERTY STRINGS "" AUTO 6 5)

set (CISST_QT_COMPONENTS_COMMON Core Xml Widgets Gui OpenGL)

set (_cisst_qt_versions_to_try)
if (CISST_QT_VERSION STREQUAL "" OR CISST_QT_VERSION STREQUAL "AUTO")
  set (_cisst_qt_versions_to_try 6 5)
elseif (CISST_QT_VERSION STREQUAL "6" OR CISST_QT_VERSION STREQUAL "5")
  set (_cisst_qt_versions_to_try ${CISST_QT_VERSION})
else ()
  message (FATAL_ERROR "CISST_QT_VERSION must be empty, AUTO, 6 or 5")
endif ()

set (CISST_QT_VERSION_FOUND "")
foreach (_cisst_qt_version ${_cisst_qt_versions_to_try})
  if (NOT CISST_QT_VERSION_FOUND)
    set (_cisst_qt_components ${CISST_QT_COMPONENTS_COMMON})
    if (_cisst_qt_version STREQUAL "5")
      list (APPEND _cisst_qt_components XmlPatterns)
    else ()
      list (APPEND _cisst_qt_components OpenGLWidgets)
    endif ()
    find_package (Qt${_cisst_qt_version} QUIET COMPONENTS ${_cisst_qt_components})
    if (Qt${_cisst_qt_version}_FOUND)
      set (CISST_QT_VERSION_FOUND ${_cisst_qt_version})
    else ()
      message ("Looking for Qt${_cisst_qt_version}: can't find required components ${_cisst_qt_components}")
    endif ()
  endif ()
endforeach ()

if (CISST_QT_VERSION_FOUND)
  set (CISST_QT_VERSION ${CISST_QT_VERSION_FOUND} CACHE STRING "Qt major version to use: empty or AUTO prefers Qt6 then Qt5; otherwise use 6 or 5" FORCE)
  option (CISST_HAS_QT "Is Qt available.  If not, Qt widgets and some examples won't be compiled" ON)
else ()
  option (CISST_HAS_QT "Is Qt available.  If not, Qt widgets and some examples won't be compiled" OFF)
endif ()
mark_as_advanced (CISST_HAS_QT)

if (CISST_HAS_QT AND NOT (CISST_QT_VERSION STREQUAL "6" OR CISST_QT_VERSION STREQUAL "5"))
  message (FATAL_ERROR "CISST_HAS_QT is ON but no usable Qt version was found. Set CISST_QT_VERSION to 6 or 5, or turn CISST_HAS_QT OFF.")
endif ()

set (CISST_HAS_QT5 OFF)
set (CISST_HAS_QT6 OFF)
set (CISST_HAS_QT5 OFF PARENT_SCOPE)
set (CISST_HAS_QT6 OFF PARENT_SCOPE)

if (CISST_HAS_QT)
  set (CISST_QT_COMPONENTS ${CISST_QT_COMPONENTS_COMMON})
  if (CISST_QT_VERSION STREQUAL "5")
    list (APPEND CISST_QT_COMPONENTS XmlPatterns)
  else ()
    list (APPEND CISST_QT_COMPONENTS OpenGLWidgets)
  endif ()
  find_package (Qt${CISST_QT_VERSION} REQUIRED COMPONENTS ${CISST_QT_COMPONENTS})
  set (CISST_QT_LIBRARIES)
  foreach (component ${CISST_QT_COMPONENTS})
    list (APPEND CISST_QT_LIBRARIES Qt${CISST_QT_VERSION}::${component})
  endforeach ()
  set (_cisst_qt_qmake_variable Qt${CISST_QT_VERSION}_QMAKE_EXECUTABLE)
  if (${_cisst_qt_qmake_variable})
    set (QT_QMAKE_EXECUTABLE ${${_cisst_qt_qmake_variable}})
  elseif (TARGET Qt${CISST_QT_VERSION}::qmake)
    get_target_property (QT_QMAKE_EXECUTABLE Qt${CISST_QT_VERSION}::qmake IMPORTED_LOCATION)
  endif ()
  set (CISST_HAS_QT${CISST_QT_VERSION} ON)
  set (CISST_HAS_QT${CISST_QT_VERSION} ON PARENT_SCOPE)
  set (CISST_HAS_QT ON PARENT_SCOPE)
  set (CISST_QT_VERSION ${CISST_QT_VERSION} PARENT_SCOPE)
  set (CISST_QT_COMPONENTS ${CISST_QT_COMPONENTS} PARENT_SCOPE)
  set (CISST_QT_LIBRARIES ${CISST_QT_LIBRARIES} PARENT_SCOPE)
  set (CISST_QT_PACKAGES)
  foreach (component ${CISST_QT_COMPONENTS})
    set (package Qt${CISST_QT_VERSION}${component})
    list (APPEND CISST_QT_PACKAGES ${package})
  endforeach ()
else ()
  set (CISST_HAS_QT OFF PARENT_SCOPE)
endif ()

if (CISST_HAS_QT)
  # OpenGL
  set (CISST_HAS_OPENGL ON CACHE BOOL "Is OpenGL available." FORCE)
  mark_as_advanced (CISST_HAS_OPENGL)
  # XML Qt based
  if (CISST_HAS_QT5)
    set (CISST_FOUND_QT_XML ON CACHE BOOL "Is Qt XML available.  CISST_XML_LIB determines which library is actually used." FORCE)
  else ()
    set (CISST_FOUND_QT_XML OFF CACHE BOOL "Is Qt XML available.  CISST_XML_LIB determines which library is actually used." FORCE)
  endif ()
  mark_as_advanced (CISST_FOUND_QT_XML)

  # Optionally build QtNode as an external project if Graphviz is available
  if (CISST_HAS_GRAPHVIZ)
     set (CISST_HAS_QTNODES_DEFAULT ON)
  else ()
     set (CISST_HAS_QTNODES_DEFAULT OFF)
     message (WARNING "CISST_HAS_QTNODES set to OFF by default since libgraphviz is not found. You can install graphviz with: sudo apt install libgraphviz-dev and then set CISST_HAS_QTNODES back to ON.")
  endif ()
  option (CISST_HAS_QTNODES "Build QtNode (Qt + Node integration) as an external project." ${CISST_HAS_QTNODES_DEFAULT})

  if (CISST_HAS_QTNODES)
    include (ExternalProject)
    if (CISST_HAS_QT6)
      set (CISST_QTNODES_USE_QT6 ON)
    else ()
      set (CISST_QTNODES_USE_QT6 OFF)
    endif ()
  
    set (cisstQtNodes_PREFIX cisstQtNodeExternal)
    # Default repository and tag can be overridden by user cache variables
    set (CISST_QTNODES_GIT_REPOSITORY "https://github.com/jhu-cisst-external/nodeeditor" CACHE STRING "Git repository URL for QtNode external project")
    set (CISST_QTNODES_GIT_TAG "master" CACHE STRING "Git tag or branch to checkout for QtNode external project")
    
    ExternalProject_Add (
      cisstQtNodesExternal
      PREFIX ${cisstQtNodes_PREFIX}
      GIT_REPOSITORY    ${CISST_QTNODES_GIT_REPOSITORY}
      GIT_TAG           ${CISST_QTNODES_GIT_TAG}
      CMAKE_CACHE_ARGS
        -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
        -DCMAKE_OSX_SYSROOT:STRING=${CMAKE_OSX_SYSROOT}
        -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=${CMAKE_OSX_DEPLOYMENT_TARGET}
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
        -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_CXX_STANDARD:STRING=17
        -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=ON
        -DCMAKE_CXX_EXTENSIONS:BOOL=OFF
        -DCMAKE_INSTALL_PREFIX:FILEPATH=${cisst_BINARY_DIR}/cisstQtNodes
        -DBUILD_TESTING:BOOL=OFF
        -DBUILD_DOCS:BOOL=OFF
        -DBUILD_EXAMPLES:BOOL=OFF
        -DUSE_QT6:BOOL=${CISST_QTNODES_USE_QT6}
        -DCMAKE_PREFIX_PATH:STRING=${CMAKE_PREFIX_PATH}
        -DQt${CISST_QT_VERSION}_DIR:PATH=${Qt${CISST_QT_VERSION}_DIR}
      INSTALL_DIR ${cisst_BINARY_DIR}/cisstQtNodes
      UPDATE_COMMAND ""
    )
    
    # Set variables pointing to the install dir so downstream code can use QtNode
    set (QTNODES_INCLUDE_DIR "${cisst_BINARY_DIR}/cisstQtNodes/include")
    set (QTNODES_LIBRARY_DIRS "${cisst_BINARY_DIR}/cisstQtNodes/lib")
    set (QTNODES_LIBRARIES QtNodes)
    if (NOT TARGET cisstQtNodes)
      add_library (cisstQtNodes INTERFACE)
      add_dependencies (cisstQtNodes cisstQtNodesExternal)
      target_include_directories (cisstQtNodes INTERFACE
                                  $<BUILD_INTERFACE:${QTNODES_INCLUDE_DIR}>
                                  $<INSTALL_INTERFACE:include>)
      target_link_directories (cisstQtNodes INTERFACE
                               $<BUILD_INTERFACE:${QTNODES_LIBRARY_DIRS}>
                               $<INSTALL_INTERFACE:lib>)
      target_link_libraries (cisstQtNodes INTERFACE ${QTNODES_LIBRARIES})
    endif ()

    set (QTNODES_INCLUDE_DIR ${QTNODES_INCLUDE_DIR} PARENT_SCOPE)
    set (QTNODES_LIBRARY_DIRS ${QTNODES_LIBRARY_DIRS} PARENT_SCOPE)
    set (QTNODES_LIBRARIES ${QTNODES_LIBRARIES} PARENT_SCOPE)
    
    set (QTNODES_FOUND ON PARENT_SCOPE)
    set (QTNODES_FOUND ON)

    install (DIRECTORY ${cisst_BINARY_DIR}/cisstQtNodes/
             DESTINATION .
             USE_SOURCE_PERMISSIONS
             COMPONENT cisstCommon)
  endif ()
else ()
  # XML Qt based
  set (CISST_FOUND_QT_XML OFF CACHE BOOL "Is Qt XML available.  CISST_XML_LIB determines which library is actually used." FORCE)
  mark_as_advanced (CISST_FOUND_QT_XML)
endif ()


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
    # propagate the fact that SWIG was found
    set (CISST_SWIG_FOUND 0 PARENT_SCOPE)
  else (${SWIG_VERSION} VERSION_LESS ${CISST_SWIG_REQUIRED_VERSION})
    include (${SWIG_USE_FILE})
    # save SWIG settings
    set (CISST_SWIG_PACKAGE "SWIG")
    set (SWIG_EXECUTABLE ${SWIG_EXECUTABLE} PARENT_SCOPE)
    set (SWIG_USE_FILE ${SWIG_USE_FILE} PARENT_SCOPE)
    # propagate the fact that SWIG was found
    set (CISST_SWIG_FOUND 1 PARENT_SCOPE)
  endif (${SWIG_VERSION} VERSION_LESS ${CISST_SWIG_REQUIRED_VERSION})
endif (CISST_HAS_SWIG_PYTHON)

if (CISST_HAS_SWIG_PYTHON OR CISST_cisstInteractive)
  # Force to use exceptions
  set (CISST_CMN_THROW_DOES_ABORT OFF CACHE BOOL "cmnThrow uses abort() instead of throw" FORCE)
  set (CISST_CMN_ASSERT_THROWS_EXCEPTION ON CACHE BOOL "CMN_ASSERT throws an exception instead of abort" FORCE)

  # Find Python and NumPy
  set (CISST_PYTHON_VERSION_REQUIRED "" CACHE STRING "Required Python version (if not empty)")
  if (CISST_PYTHON_VERSION_REQUIRED)
    message (STATUS "Looking for Python ${CISST_PYTHON_VERSION_REQUIRED}")
    find_package (Python ${CISST_PYTHON_VERSION_REQUIRED} EXACT REQUIRED COMPONENTS Interpreter Development NumPy)
  else ()
    find_package (Python REQUIRED COMPONENTS Interpreter Development NumPy)
  endif ()

  set (Python_EXECUTABLE ${Python_EXECUTABLE} PARENT_SCOPE)
  set (PYTHON_ALL_INCLUDE_DIRS ${Python_INCLUDE_DIRS} ${Python_NumPy_INCLUDE_DIRS})
  set (PYTHON_ALL_LIBRARIES ${Python_LIBRARIES})
  set (PYTHON_ALL_INCLUDE_DIRS ${PYTHON_ALL_INCLUDE_DIRS} PARENT_SCOPE)
  set (PYTHON_ALL_LIBRARIES ${PYTHON_ALL_LIBRARIES} PARENT_SCOPE)

else (CISST_HAS_SWIG_PYTHON OR CISST_cisstInteractive)
  # propagate the fact that SWIG was not found
  set (CISST_SWIG_FOUND 0 PARENT_SCOPE)
endif (CISST_HAS_SWIG_PYTHON OR CISST_cisstInteractive)


# Support for cisstCommonXML, just to see what is available
find_package (LibXml2)
if (LIBXML2_FOUND)
  set (CISST_FOUND_LIBXML2_XML ON CACHE BOOL "Is LibXml2 XML available.  CISST_XML_LIB determines which library is actually used." FORCE)
  mark_as_advanced (CISST_FOUND_LIBXML2_XML)
else (LIBXML2_FOUND)
  set (CISST_FOUND_LIBXML2_XML OFF CACHE BOOL "Is LibXml2 XML available.  CISST_XML_LIB determines which library is actually used." FORCE)
  mark_as_advanced (CISST_FOUND_LIBXML2_XML)
endif (LIBXML2_FOUND)


# Support for JSON
option (CISST_HAS_JSON "Do you want to compile JsonCpp with cisst?  If not, some examples and utilities won't be compiled" ON)

if (CISST_HAS_JSON)

  if (CISST_USE_PKG_CONFIG)
    pkg_check_modules(JSONCPP jsoncpp)

    if (JSONCPP_FOUND)
      set (JSON_INCLUDE_DIR "${JSONCPP_INCLUDE_DIRS}")
      set (JSON_LIBRARY_DIRS "${JSONCPP_LIBDIR}")
      set (JSON_LIBRARIES "${JSONCPP_LIBRARIES}")
      set (JSON_FOUND ON PARENT_SCOPE)
      set (JSON_USE_SYSTEM_VERSION ON PARENT_SCOPE)
    endif (JSONCPP_FOUND)
  endif (CISST_USE_PKG_CONFIG)

  if (NOT JSONCPP_FOUND)

    # otherwise, build as external project
    include (ExternalProject)

    set (cisstJSON_PREFIX cisstJSONExternal)
    set (CISST_JSON_GIT_REPOSITORY https://github.com/jhu-cisst-external/jsoncpp.git)
    set (CISST_JSON_GIT_TAG 1.7.4.2)

    # add fPIC if possible
    include (CheckCXXCompilerFlag)
    check_cxx_compiler_flag ("-fPIC" CXX_SUPPORTS_FPIC)
    if (CXX_SUPPORTS_FPIC)
      set (JSON_CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    endif (CXX_SUPPORTS_FPIC)

    ExternalProject_Add (cisstJSONExternal
                         PREFIX ${cisstJSON_PREFIX}
                         GIT_REPOSITORY    ${CISST_JSON_GIT_REPOSITORY}
                         GIT_TAG           ${CISST_JSON_GIT_TAG}
                         CMAKE_CACHE_ARGS -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
                                          -DCMAKE_OSX_SYSROOT:STRING=${CMAKE_OSX_SYSROOT}
                                          -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=${CMAKE_OSX_DEPLOYMENT_TARGET}
                                          -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
                                          -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE}
                                          -DCMAKE_CXX_FLAGS:STRING=${JSON_CMAKE_CXX_FLAGS}
                                          -DCMAKE_POLICY_VERSION_MINIMUM:STRING=3.5
                                          -DOPTION_BUILD_EXAMPLES:BOOL=OFF
                                          -DJSONCPP_WITH_TESTS:BOOL=OFF
                                          -DJSONCPP_WITH_POST_BUILD_UNITTEST:BOOL=OFF
                                          -DJSONCPP_WITH_STRICT_ISO:BOOL=OFF
                                          -DCMAKE_INSTALL_PREFIX:FILEPATH=${cisst_BINARY_DIR}/cisstJSON
                         INSTALL_DIR ${cisst_BINARY_DIR}/cisstJSON
                         UPDATE_COMMAND ""
                         )

    # Set all JSON variables based on install directory
    set (JSON_INCLUDE_DIR "${cisst_BINARY_DIR}/cisstJSON/include")
    set (JSON_LIBRARY_DIRS "${cisst_BINARY_DIR}/cisstJSON/lib")
    set (JSON_LIBRARIES jsoncpp)
    set (JSON_FOUND ON PARENT_SCOPE)
    set (JSON_USE_SYSTEM_VERSION OFF PARENT_SCOPE)

  endif (NOT JSONCPP_FOUND)

  # propagate to parent directory
  set (JSON_INCLUDE_DIR ${JSON_INCLUDE_DIR} PARENT_SCOPE)
  set (JSON_LIBRARY_DIRS ${JSON_LIBRARY_DIRS} PARENT_SCOPE)
  set (JSON_LIBRARIES ${JSON_LIBRARIES} PARENT_SCOPE)
  if (NOT TARGET cisstJsonCpp)
    add_library (cisstJsonCpp INTERFACE IMPORTED)
    set_target_properties (cisstJsonCpp PROPERTIES
                           INTERFACE_INCLUDE_DIRECTORIES "${JSON_INCLUDE_DIR}"
                           INTERFACE_LINK_LIBRARIES "${JSON_LIBRARIES}")
    if (JSON_LIBRARY_DIRS)
      set_target_properties (cisstJsonCpp PROPERTIES
                             INTERFACE_LINK_DIRECTORIES "${JSON_LIBRARY_DIRS}")
    endif ()
  endif ()

endif (CISST_HAS_JSON)
