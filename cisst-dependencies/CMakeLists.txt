#
# (C) Copyright 2005-2016 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---


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
    cisst_set_package_settings (cisstOpenGL OpenGL INCLUDE_DIRECTORIES OPENGL_INCLUDE_DIR)
  endif (OPENGL_INCLUDE_DIR)
  cisst_set_package_settings (cisstOpenGL OpenGL LIBRARIES OPENGL_LIBRARIES)
else (CISST_HAS_OPENGL)
  cisst_unset_all_package_settings (cisstOpenGL OpenGL)
endif (CISST_HAS_OPENGL)


# Support for QT

# first try to find Qt5
set (CISST_QT5_PACKAGES Qt5Core Qt5Xml Qt5XmlPatterns Qt5Widgets Qt5Gui Qt5OpenGL)

set (CISST_QT5_FOUND ON)
foreach (package ${CISST_QT5_PACKAGES})
  find_package (${package} QUIET)
  if (${package}_FOUND)
    cisst_cmake_debug ("Found ${package}")
  else ()
    message ("Looking for Qt5: can't find ${package}")
    set (CISST_QT5_FOUND OFF)
  endif ()
endforeach ()

# if Qt 5 has been found, offer it by default
if (CISST_QT5_FOUND)
  option (CISST_HAS_QT5 "Is Qt 5 available.  If not, Qt widgets and some examples won't be compiled" ON)
  mark_as_advanced (CISST_HAS_QT5)
else ()
  option (CISST_HAS_QT5 "Is Qt 5 available.  If not, Qt widgets and some examples won't be compiled" OFF)
endif ()

# try to find Qt 4 as well but offer it only if Qt5 has not been found
set (CISST_QT4_COMPONENTS QtCore QtGui QtOpenGL QtXML QtXMLPatterns)
find_package (Qt4 COMPONENTS ${CISST_QT4_COMPONENTS})
if (QT_FOUND AND NOT CISST_HAS_QT5)
  option (CISST_HAS_QT4 "Is Qt 4 available.  If not, Qt widgets and some examples won't be compiled" ON)
  mark_as_advanced (CISST_HAS_QT4)
else (QT_FOUND AND NOT CISST_HAS_QT5)
  option (CISST_HAS_QT4 "Is Qt 4 available.  If not, Qt widgets and some examples won't be compiled" OFF)
endif (QT_FOUND AND NOT CISST_HAS_QT5)

# check that we don't have both Qt 4 and 5 set
if (CISST_HAS_QT4 AND CISST_HAS_QT5)
  message (FATAL_ERROR "You can't have both CISST_HAS_QT4 and CISST_HAS_QT5 set, pick one only")
endif (CISST_HAS_QT4 AND CISST_HAS_QT5)

# set overall Qt flag
if (CISST_HAS_QT4 OR CISST_HAS_QT5)
  set (CISST_HAS_QT ON PARENT_SCOPE)
else ()
  set (CISST_HAS_QT OFF PARENT_SCOPE)
endif ()

# Qt5: if detected or if user requests it
if (CISST_HAS_QT5)
  # Qt5 is now required
  foreach (package ${CISST_QT5_PACKAGES})
    find_package (${package} REQUIRED)
    cisst_set_package_settings (cisstQt ${package} PACKAGES package)
  endforeach ()
else (CISST_HAS_QT5)
  cisst_unset_all_package_settings (cisstQt Qt5)
endif (CISST_HAS_QT5)


# if detected or if user requests it
if (CISST_HAS_QT4)
  # Qt is now required so we can include the QT_USE_FILE
  find_package (Qt4 REQUIRED ${CISST_QT4_COMPONENTS})
  include (${QT_USE_FILE})
  set (CISST_QT4_PACKAGE "Qt4")
  cisst_set_package_settings (cisstQt Qt4 PACKAGES           CISST_QT4_PACKAGE)
  cisst_set_package_settings (cisstQt Qt4 PACKAGE_COMPONENTS CISST_QT4_COMPONENTS)
  cisst_set_package_settings (cisstQt Qt4 CMAKE_FILES        QT_USE_FILE)
  cisst_set_package_settings (cisstQt Qt4 LIBRARIES          QT_LIBRARIES)
else (CISST_HAS_QT4)
  cisst_unset_all_package_settings (cisstQt Qt4)
endif (CISST_HAS_QT4)

# for both version of Qt
if (CISST_HAS_QT4 OR CISST_HAS_QT5)
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
else ()
  # XML Qt based
  set (CISST_FOUND_QT_XML OFF CACHE BOOL "Is Qt XML available.  CISST_XML_LIB determines which library is actually used." FORCE)
  mark_as_advanced (CISST_FOUND_QT_XML)
endif ()



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
  if (NOT FLTK_FOUND)
    # FLTK released versions have a broken CMake for Mac
    if (NOT APPLE)
      # Try to build as external project
      if (CISST_USE_EXTERNAL)
        include (ExternalProject)
        set (cisstFltk_PREFIX cisstFltkExternal)
        set (CISST_FLTK_URL http://fltk.org/pub/fltk/1.3.2/fltk-1.3.2-source.tar.gz)
        set (CISST_FLTK_URL_MD5 9f7e707d4fb7a5a76f0f9b73ff70623d)
        ExternalProject_Add (cisstFltkExternal
                             PREFIX ${cisstFltk_PREFIX}
                             URL ${CISST_FLTK_URL}
                             URL_MD5 ${CISST_FLTK_URL_MD5}
                             CMAKE_CACHE_ARGS -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
                                              -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
                                              -DOPTION_BUILD_EXAMPLES:BOOL=OFF
                                              -DCMAKE_INSTALL_PREFIX:FILEPATH=${cisst_BINARY_DIR}/cisstFltk
                             INSTALL_DIR ${cisst_BINARY_DIR}/cisstFltk
                             )

        # Set all FLTK variables based on install directory
        set (FLTK_INCLUDE_DIR "${cisst_BINARY_DIR}/cisstFltk/include")
        set (FLTK_INCLUDE_DIR ${FLTK_INCLUDE_DIR} PARENT_SCOPE)

	set (FLTK_LIBRARY_DIRS "${cisst_BINARY_DIR}/cisstFLTK/lib")
        set (FLTK_LIBRARY_DIRS ${FLTK_LIBRARY_DIRS} PARENT_SCOPE)

        if (WIN32)
          set (FLTK_LIBRARIES debug fltkd optimized fltk
                              debug fltkformsd optimized fltkforms
                              debug fltkgld optimized fltkgl
                              debug fltkimagesd optimized fltkimages)
        else (WIN32)
          set (FLTK_LIBRARIES fltk fltk_forms fltk_gl fltk_images)
          if (UNIX)
            include (FindX11)
            find_library (FLTK_MATH_LIBRARY m)
            set (FLTK_LIBRARIES ${FLTK_LIBRARIES} ${X11_LIBRARIES} ${FLTK_MATH_LIBRARY})
          endif()

	endif (WIN32)
        set (FLTK_LIBRARIES ${FLTK_LIBRARIES} PARENT_SCOPE)

	set (FLTK_FLUID_EXECUTABLE "${cisst_BINARY_DIR}/cisstFltk/bin/fluid${CMAKE_EXECUTABLE_SUFFIX}")
	set (FLTK_FLUID_EXECUTABLE ${FLTK_FLUID_EXECUTABLE} PARENT_SCOPE)

        set (FLTK_FOUND ON)
        set (FLTK_FOUND ON PARENT_SCOPE)

      else (CISST_USE_EXTERNAL)
        # don't want to use external project, now set package as required
        find_package (FLTK REQUIRED)
      endif (CISST_USE_EXTERNAL)
    else (NOT APPLE)
      # now set package as required
      find_package (FLTK REQUIRED)
    endif (NOT APPLE)
  endif (NOT FLTK_FOUND)

  # save settings for external projects
  cisst_set_package_settings (cisstFLTK FLTK INCLUDE_DIRECTORIES FLTK_INCLUDE_DIR)
  cisst_set_package_settings (cisstFLTK FLTK LIBRARIES           FLTK_LIBRARIES)
  if (FLTK_LIBRARY_DIRS)
    # If FLTK was built using CMake (i.e., there is an FLTKConfig.cmake), the FLTK_LIBRARIES (above) does not include the path,
    # so we need to add FLTK_LIBRARY_DIRS (which is only defined if FLTK was built using CMake) to the link directories.
    cisst_set_package_settings (cisstFLTK FLTK LINK_DIRECTORIES  FLTK_LIBRARY_DIRS)
  endif (FLTK_LIBRARY_DIRS)

  # Create the CMake config file
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cisstFLTKBuild.cmake.in
                  ${CISST_CMAKE_BINARY_DIR}/cisstFLTKBuild.cmake
                  @ONLY)
  install (FILES ${CISST_CMAKE_BINARY_DIR}/cisstFLTKBuild.cmake
           DESTINATION ${CISST_CMAKE_INSTALL_SUFFIX}
           COMPONENT cisstCommon)

else (CISST_HAS_FLTK)
  cisst_unset_all_package_settings (cisstCommon FLTK)
endif (CISST_HAS_FLTK)


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
    cisst_set_package_settings (cisstPython SWIG PACKAGES    CISST_SWIG_PACKAGE)
    cisst_set_package_settings (cisstPython SWIG CMAKE_FILES SWIG_USE_FILE)
    # propagate the fact that SWIG was found
    set (CISST_SWIG_FOUND 1 PARENT_SCOPE)
  endif (${SWIG_VERSION} VERSION_LESS ${CISST_SWIG_REQUIRED_VERSION})

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
  # We also need numpy for cisstVector, cisstMultiTask and cisstNumerical
  find_package (Numpy REQUIRED)
  if (NUMPY_FOUND)
    cisst_set_package_settings (cisstPython NumPy INCLUDE_DIRECTORIES PYTHON_NUMPY_INCLUDE_DIR)
    mark_as_advanced (PYTHON_NUMPY_INCLUDE_DIR)
  else (NUMPY_FOUND)
    cisst_unset_all_package_settings (cisstPython NumPy)
  endif (NUMPY_FOUND)

  # Create cisstPythonBuild, which contains SWIG_EXECUTABLE and PYTHON_EXECUTABLE.
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

else (CISST_HAS_SWIG_PYTHON)
  # propagate the fact that SWIG was not found
  set (CISST_SWIG_FOUND 0 PARENT_SCOPE)
  cisst_unset_all_package_settings (cisstPython Python)
  cisst_unset_all_package_settings (cisstPython NumPy)
endif (CISST_HAS_SWIG_PYTHON)


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

  if (WIN32 OR APPLE)
    # On Windows, no need to use PkgConfig
    set(JSONCPP_FOUND OFF)
  else (WIN32 OR APPLE)
    # try to find an installed version
    find_package (PkgConfig REQUIRED)
    pkg_check_modules(JSONCPP jsoncpp)

    if (JSONCPP_FOUND)
      set (JSON_INCLUDE_DIR "${JSONCPP_INCLUDE_DIRS}")
      set (JSON_LIBRARY_DIRS "${JSONCPP_LIBDIR}")
      set (JSON_LIBRARIES "${JSONCPP_LIBRARIES}")
      set (JSON_FOUND ON PARENT_SCOPE)
      set (JSON_USE_SYSTEM_VERSION ON PARENT_SCOPE)
    endif (JSONCPP_FOUND)
  endif (WIN32 OR APPLE)

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
                                          -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
                                          -DCMAKE_CXX_FLAGS:STRING=${JSON_CMAKE_CXX_FLAGS}
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

  cisst_set_package_settings (cisstCommon JSON INCLUDE_DIRECTORIES JSON_INCLUDE_DIR)
  cisst_set_package_settings (cisstCommon JSON LIBRARIES           JSON_LIBRARIES)
  cisst_set_package_settings (cisstCommon JSON LINK_DIRECTORIES    JSON_LIBRARY_DIRS)

else (CISST_HAS_JSON)
  cisst_unset_all_package_settings (cisstCommon JSON)
endif (CISST_HAS_JSON)
