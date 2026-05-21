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
    # INTERFACE target: downstream targets link against cisstGraphviz and
    # automatically inherit include dirs / link flags.
    add_library (cisstGraphviz INTERFACE)
    add_library (cisst::cisstGraphviz ALIAS cisstGraphviz)
    target_include_directories (cisstGraphviz INTERFACE ${GRAPHVIZ_INCLUDE_DIRS})
    target_link_directories    (cisstGraphviz INTERFACE ${GRAPHVIZ_LIBRARY_DIRS})
    target_link_libraries      (cisstGraphviz INTERFACE ${GRAPHVIZ_LIBRARIES})
    install (TARGETS cisstGraphviz EXPORT cisst-targets COMPONENT cisstCommon)
    # Mark that we have graphviz for the main cisst CMakeLists.txt
    set (CISST_HAS_GRAPHVIZ ON PARENT_SCOPE)
    set (CISST_HAS_GRAPHVIZ ON)
  else ()
    set (CISST_HAS_GRAPHVIZ OFF PARENT_SCOPE)
    set (CISST_HAS_GRAPHVIZ OFF)
  endif ()
endif (CISST_USE_PKG_CONFIG)

# Support for OpenGL
find_package (OpenGL)
include (CMakeDependentOption)
cmake_dependent_option (CISST_HAS_OPENGL "Is OpenGL available." ON "OPENGL_FOUND" OFF)
mark_as_advanced (CISST_HAS_OPENGL)

if (CISST_HAS_OPENGL)
  find_package (OpenGL REQUIRED)
  add_library (cisstOpenGL INTERFACE)
  add_library (cisst::cisstOpenGL ALIAS cisstOpenGL)
  if (OPENGL_INCLUDE_DIR)
    target_include_directories (cisstOpenGL INTERFACE ${OPENGL_INCLUDE_DIR})
  endif (OPENGL_INCLUDE_DIR)
  # Prefer imported targets (OpenGL::GL, OpenGL::GLU) when available (CMake >= 3.8)
  if (TARGET OpenGL::GL)
    target_link_libraries (cisstOpenGL INTERFACE OpenGL::GL)
    if (TARGET OpenGL::GLU)
      target_link_libraries (cisstOpenGL INTERFACE OpenGL::GLU)
    endif ()
  else ()
    target_link_libraries (cisstOpenGL INTERFACE ${OPENGL_LIBRARIES})
  endif ()
  install (TARGETS cisstOpenGL EXPORT cisst-targets COMPONENT cisstCommon)
endif (CISST_HAS_OPENGL)


# Support for Qt5
set (CISST_QT5_COMPONENTS Core Xml XmlPatterns Widgets Gui OpenGL)
find_package (Qt5 COMPONENTS ${CISST_QT5_COMPONENTS} QUIET)
cmake_dependent_option (CISST_HAS_QT5 "Is Qt5 available.  If not, Qt widgets and some examples won't be compiled" ON "Qt5_FOUND" OFF)
mark_as_advanced (CISST_HAS_QT5)

# set overall Qt flag
if (CISST_HAS_QT5)
  set (CISST_HAS_QT ON PARENT_SCOPE)
else ()
  set (CISST_HAS_QT OFF PARENT_SCOPE)
endif ()

if (CISST_HAS_QT5)
  find_package (Qt5 COMPONENTS ${CISST_QT5_COMPONENTS} REQUIRED)
  # INTERFACE target: links Qt5 components with PUBLIC/INTERFACE visibility so
  # any cisst library that links cisstQt automatically inherits Qt5 dependencies.
  add_library (cisstQt INTERFACE)
  add_library (cisst::cisstQt ALIAS cisstQt)
  foreach (_comp ${CISST_QT5_COMPONENTS})
    if (TARGET Qt5::${_comp})
      target_link_libraries (cisstQt INTERFACE Qt5::${_comp})
    endif ()
  endforeach ()
  install (TARGETS cisstQt EXPORT cisst-targets COMPONENT cisstCommon)
endif (CISST_HAS_QT5)

# for both version of Qt
if (CISST_HAS_QT5)
  # OpenGL
  set (CISST_HAS_OPENGL ON CACHE BOOL "Is OpenGL available." FORCE)
  mark_as_advanced (CISST_HAS_OPENGL)
  # XML Qt based
  set (CISST_FOUND_QT_XML ON CACHE BOOL "Is Qt XML available.  CISST_XML_LIB determines which library is actually used." FORCE)
  mark_as_advanced (CISST_FOUND_QT_XML)

  # Create the CMake config file, save path to qmake to make it easy to find
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cisstQtBuild.cmake.in
                  ${CISST_CMAKE_BINARY_DIR}/cisstQtBuild.cmake
                  @ONLY)
  install (FILES ${CISST_CMAKE_BINARY_DIR}/cisstQtBuild.cmake
           DESTINATION ${CISST_CMAKE_INSTALL_SUFFIX}
           COMPONENT cisstCommon)
  
  # Prefer an OS-provided QtNodes package and fall back to the external project.
  find_package (QtNodes QUIET CONFIG)
  if (QtNodes_FOUND)
    set (QTNODES_FOUND ON)
    if (TARGET QtNodes::QtNodes)
      set (QTNODES_TARGET QtNodes::QtNodes)
    elseif (TARGET QtNodes)
      set (QTNODES_TARGET QtNodes)
    else ()
      set (QTNODES_INCLUDE_DIR ${QtNodes_INCLUDE_DIRS})
      set (QTNODES_LIBRARIES ${QtNodes_LIBRARIES})
      if (NOT QTNODES_LIBRARIES)
        set (QTNODES_LIBRARIES QtNodes)
      endif ()
    endif ()
  else ()
    find_path (QTNODES_INCLUDE_DIR
               NAMES QtNodes/NodeDelegateModel)
    find_library (QTNODES_LIBRARY
                  NAMES QtNodes)
    if (QTNODES_INCLUDE_DIR AND QTNODES_LIBRARY)
      set (QTNODES_FOUND ON)
      set (QTNODES_LIBRARIES ${QTNODES_LIBRARY})
    endif ()
  endif ()

  if (QTNODES_FOUND)
    set (QTNODES_FOUND ON PARENT_SCOPE)
    set (QTNODES_INCLUDE_DIR ${QTNODES_INCLUDE_DIR} PARENT_SCOPE)
    set (QTNODES_TARGET ${QTNODES_TARGET} PARENT_SCOPE)
    set (QTNODES_LIBRARIES ${QTNODES_LIBRARIES} PARENT_SCOPE)
  endif ()

  # Optionally build QtNode as an external project if Graphviz is available
  if (QTNODES_FOUND OR (CISST_HAS_GRAPHVIZ AND CISST_USE_EXTERNAL))
     set (CISST_HAS_QTNODES_DEFAULT ON)
  else ()
     set (CISST_HAS_QTNODES_DEFAULT OFF)
     if (NOT CISST_HAS_GRAPHVIZ)
       message (WARNING "CISST_HAS_QTNODES set to OFF by default since libgraphviz is not found. You can install graphviz with: sudo apt install libgraphviz-dev and then set CISST_HAS_QTNODES back to ON.")
     endif ()
  endif ()
  option (CISST_HAS_QTNODES "Build QtNode (Qt + Node integration) as an external project." ${CISST_HAS_QTNODES_DEFAULT})

  if (CISST_HAS_QTNODES)
    if (NOT CISST_HAS_GRAPHVIZ)
      message (SEND_ERROR "CISST_HAS_QTNODES requires Graphviz. Install graphviz or set CISST_HAS_QTNODES to OFF.")
    endif ()

    if (NOT QTNODES_FOUND AND NOT CISST_USE_EXTERNAL)
      message (SEND_ERROR "CISST_HAS_QTNODES requires CISST_USE_EXTERNAL because QtNodes is built as a CMake external project")
    endif ()
  endif ()

  if (CISST_HAS_QTNODES AND NOT QTNODES_FOUND)
    include (ExternalProject)
  
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
        -DUSE_QT6:BOOL=OFF
        -DCMAKE_PREFIX_PATH:STRING=${CMAKE_PREFIX_PATH}
        -DQt5_DIR:PATH=${Qt5_DIR}
      INSTALL_DIR ${cisst_BINARY_DIR}/cisstQtNodes
      UPDATE_COMMAND ""
    )
    
    # Set variables pointing to the install dir so downstream code can use QtNode
    set (QTNODES_INCLUDE_DIR "${cisst_BINARY_DIR}/cisstQtNodes/include")
    set (QTNODES_LIBRARY_DIRS "${cisst_BINARY_DIR}/cisstQtNodes/lib")
    set (QTNODES_LIBRARIES QtNodes)
    
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

  # Save settings (so other cisst modules can pick them up)
  if (QTNODES_FOUND)
    set (CISST_SETTINGS ${CISST_SETTINGS} cisstQtNodes PARENT_SCOPE)
    add_library (cisstQtNodes INTERFACE)
    add_library (cisst::cisstQtNodes ALIAS cisstQtNodes)
    if (QTNODES_INCLUDE_DIR)
      target_include_directories (cisstQtNodes INTERFACE
        $<BUILD_INTERFACE:${QTNODES_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>)
    endif ()
    if (QTNODES_LIBRARY_DIRS)
      target_link_directories (cisstQtNodes INTERFACE
        $<BUILD_INTERFACE:${QTNODES_LIBRARY_DIRS}>
        $<INSTALL_INTERFACE:lib>)
    endif ()
    if (QTNODES_TARGET)
      target_link_libraries (cisstQtNodes INTERFACE ${QTNODES_TARGET})
    else ()
      target_link_libraries (cisstQtNodes INTERFACE ${QTNODES_LIBRARIES})
    endif ()
    if (TARGET cisstQtNodesExternal)
      add_dependencies (cisstQtNodes cisstQtNodesExternal)
    endif ()
    install (TARGETS cisstQtNodes EXPORT cisst-targets COMPONENT cisstCommon)
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
    # INTERFACE target for SWIG (carries no link deps itself; UseSWIG must
    # be included separately in cisst_add_swig_module via find_package(SWIG))
    add_library (cisstSWIG INTERFACE)
    add_library (cisst::cisstSWIG ALIAS cisstSWIG)
    install (TARGETS cisstSWIG EXPORT cisst-targets COMPONENT cisstCommon)
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

  # INTERFACE target for Python (carries include paths and link libraries)
  add_library (cisstPython INTERFACE)
  add_library (cisst::cisstPython ALIAS cisstPython)
  target_include_directories (cisstPython INTERFACE ${PYTHON_ALL_INCLUDE_DIRS})
  if (TARGET Python::Python)
    target_link_libraries (cisstPython INTERFACE Python::Python)
  else ()
    target_link_libraries (cisstPython INTERFACE ${PYTHON_ALL_LIBRARIES})
  endif ()
  install (TARGETS cisstPython EXPORT cisst-targets COMPONENT cisstCommon)

  # Create cisstPythonBuild, which contains SWIG_EXECUTABLE and Python_EXECUTABLE.
  # (Currently, this is only created if the correct version of SWIG was found -- should that check be removed?
  #  Also, note that "if (CISST_SWIG_FOUND)" does not work at this point in the CMakeLists.txt.)
  if (NOT ${SWIG_VERSION} VERSION_LESS ${CISST_SWIG_REQUIRED_VERSION})
    # Create the CMake config file
    configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cisstPythonBuild.cmake.in
                    ${CISST_CMAKE_BINARY_DIR}/cisstPythonBuild.cmake
                    @ONLY)
    install (FILES ${CISST_CMAKE_BINARY_DIR}/cisstPythonBuild.cmake
             DESTINATION ${CISST_CMAKE_INSTALL_SUFFIX}
             COMPONENT cisstCommon)
  endif (NOT ${SWIG_VERSION} VERSION_LESS ${CISST_SWIG_REQUIRED_VERSION})

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
    pkg_check_modules(JSONCPP IMPORTED_TARGET jsoncpp)

    if (JSONCPP_FOUND)
      set (JSON_INCLUDE_DIR "${JSONCPP_INCLUDE_DIRS}")
      set (JSON_LIBRARY_DIRS "${JSONCPP_LIBDIR}")
      set (JSON_LIBRARIES "${JSONCPP_LIBRARIES}")
      set (JSON_FOUND ON PARENT_SCOPE)
      set (JSON_USE_SYSTEM_VERSION ON PARENT_SCOPE)
      # Propagate the imported target name so the parent deferred block can use it
      set (JSON_TARGET "PkgConfig::JSONCPP" PARENT_SCOPE)
    endif (JSONCPP_FOUND)
  endif (CISST_USE_PKG_CONFIG)

  if (NOT JSONCPP_FOUND AND CISST_USE_EXTERNAL)

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
                                          -DOPTION_BUILD_EXAMPLES:BOOL=OFF
                                          -DJSONCPP_WITH_TESTS:BOOL=OFF
                                          -DJSONCPP_WITH_POST_BUILD_UNITTEST:BOOL=OFF
                                          -DJSONCPP_WITH_STRICT_ISO:BOOL=OFF
                                          -DCMAKE_INSTALL_PREFIX:FILEPATH=${cisst_BINARY_DIR}/cisstJSON
                         INSTALL_DIR ${cisst_BINARY_DIR}/cisstJSON
                         UPDATE_COMMAND ""
                         )

    install (DIRECTORY ${cisst_BINARY_DIR}/cisstJSON/
             DESTINATION .
             USE_SOURCE_PERMISSIONS
             COMPONENT cisstCommon)

    # Set all JSON variables based on install directory
    set (JSON_INCLUDE_DIR "${cisst_BINARY_DIR}/cisstJSON/include")
    set (JSON_LIBRARY_DIRS "${cisst_BINARY_DIR}/cisstJSON/lib")
    set (JSON_LIBRARIES jsoncpp)
    set (JSON_EXTERNAL_TARGET cisstJSONExternal)
    set (JSON_FOUND ON PARENT_SCOPE)
    set (JSON_USE_SYSTEM_VERSION OFF PARENT_SCOPE)
    set (JSON_EXTERNAL_TARGET ${JSON_EXTERNAL_TARGET} PARENT_SCOPE)

  endif (NOT JSONCPP_FOUND AND CISST_USE_EXTERNAL)

  if (NOT JSONCPP_FOUND AND NOT CISST_USE_EXTERNAL)
    message (SEND_ERROR "JsonCpp is needed for CISST_HAS_JSON, you can either install it or turn ON the option CISST_USE_EXTERNAL")
  endif ()

  # propagate to parent directory
  set (JSON_INCLUDE_DIR ${JSON_INCLUDE_DIR} PARENT_SCOPE)
  set (JSON_LIBRARY_DIRS ${JSON_LIBRARY_DIRS} PARENT_SCOPE)
  set (JSON_LIBRARIES ${JSON_LIBRARIES} PARENT_SCOPE)

endif (CISST_HAS_JSON)
