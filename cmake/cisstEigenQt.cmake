#
# (C) Copyright 2026 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# Keep managing an external build on reconfigure. An explicit EigenQt_DIR can
# still select a separately installed package instead.
set (_cisst_eigenqt_package_dir "${cisst_BINARY_DIR}/lib/cmake/EigenQt")
if (NOT CISST_EIGENQT_EXTERNAL OR NOT CISST_USE_EXTERNAL OR
    (EigenQt_DIR AND NOT EigenQt_DIR STREQUAL _cisst_eigenqt_package_dir))
  if (CISST_USE_EXTERNAL)
    find_package (EigenQt 0.1 CONFIG QUIET COMPONENTS Geometry)
  else ()
    find_package (EigenQt 0.1 CONFIG REQUIRED COMPONENTS Geometry)
  endif ()
  if (EigenQt_FOUND)
    if (NOT EigenQt_QT_VERSION STREQUAL CISST_QT_VERSION)
      message (FATAL_ERROR "EigenQt and cisst must use the same Qt major version")
    endif ()
    set (CISST_EIGENQT_EXTERNAL OFF CACHE INTERNAL "Build EigenQt with ExternalProject" FORCE)
    return ()
  endif ()
endif ()

include (ExternalProject)
find_package (OpenGL REQUIRED)
set (CISST_EIGENQT_EXTERNAL ON CACHE INTERNAL "Build EigenQt with ExternalProject" FORCE)
set (CISST_EIGENQT_GIT_REPOSITORY "https://github.com/jhu-cisst/eigen-qt.git"
     CACHE STRING "Git repository for the EigenQt external project")
set (CISST_EIGENQT_GIT_TAG "cd7e355c6f0c0dd621e6ac7cc987df101297fdb8"
     CACHE STRING "Git revision for the EigenQt external project")
mark_as_advanced (CISST_EIGENQT_GIT_REPOSITORY CISST_EIGENQT_GIT_TAG)

set (_cisst_eigenqt_args
     -DCMAKE_INSTALL_PREFIX:PATH=${cisst_BINARY_DIR}
     -DCMAKE_INSTALL_LIBDIR:STRING=lib
     -DCMAKE_INSTALL_BINDIR:STRING=bin
     -DCMAKE_INSTALL_INCLUDEDIR:STRING=include
     -DBUILD_SHARED_LIBS:BOOL=${CISST_BUILD_SHARED_LIBS}
     -DEIGENQT_QT_VERSION:STRING=${CISST_QT_VERSION}
     -DEIGENQT_BUILD_TESTS:BOOL=OFF
     -DEIGENQT_BUILD_EXAMPLES:BOOL=OFF
     -DEigen3_DIR:PATH=${Eigen3_DIR}
     -DQt${CISST_QT_VERSION}_DIR:PATH=${Qt${CISST_QT_VERSION}_DIR})
foreach (_variable CMAKE_BUILD_TYPE CMAKE_CONFIGURATION_TYPES CMAKE_PREFIX_PATH
         CMAKE_TOOLCHAIN_FILE CMAKE_CXX_COMPILER CMAKE_CXX_COMPILER_LAUNCHER
         CMAKE_CXX_COMPILER_TARGET CMAKE_CXX_COMPILER_EXTERNAL_TOOLCHAIN
         CMAKE_SYSROOT CMAKE_FIND_ROOT_PATH CMAKE_MSVC_RUNTIME_LIBRARY
         CMAKE_OSX_ARCHITECTURES CMAKE_OSX_SYSROOT CMAKE_OSX_DEPLOYMENT_TARGET
         CMAKE_CXX_FLAGS CMAKE_EXE_LINKER_FLAGS CMAKE_SHARED_LINKER_FLAGS)
  if (DEFINED ${_variable})
    # Preserve expressions such as the per-configuration MSVC runtime choice
    # for evaluation by EigenQt, not by ExternalProject's cache-file generator.
    string (REPLACE "$<" "$<1:$><" _value "${${_variable}}")
    list (APPEND _cisst_eigenqt_args "-D${_variable}:STRING=${_value}")
  endif ()
endforeach ()

# Distinct filenames keep Debug and Release installations usable together.
set (_cisst_eigenqt_suffix "")
if (CMAKE_CONFIGURATION_TYPES)
  set (_cisst_eigenqt_suffix "-$<CONFIG>")
  foreach (_config ${CMAKE_CONFIGURATION_TYPES})
    string (TOUPPER "${_config}" _upper_config)
    list (APPEND _cisst_eigenqt_args "-DCMAKE_${_upper_config}_POSTFIX:STRING=-${_config}")
  endforeach ()
endif ()

set (_cisst_eigenqt_library_type STATIC)
set (_cisst_eigenqt_prefix "${CMAKE_STATIC_LIBRARY_PREFIX}")
set (_cisst_eigenqt_extension "${CMAKE_STATIC_LIBRARY_SUFFIX}")
set (_cisst_eigenqt_library_dir "${cisst_BINARY_DIR}/lib")
if (CISST_BUILD_SHARED_LIBS)
  set (_cisst_eigenqt_library_type SHARED)
  set (_cisst_eigenqt_prefix "${CMAKE_SHARED_LIBRARY_PREFIX}")
  set (_cisst_eigenqt_extension "${CMAKE_SHARED_LIBRARY_SUFFIX}")
  if (WIN32)
    set (_cisst_eigenqt_library_dir "${cisst_BINARY_DIR}/bin")
  endif ()
endif ()

file (MAKE_DIRECTORY "${cisst_BINARY_DIR}/include")
set (_cisst_eigenqt_byproducts)
set (_cisst_eigenqt_dlls)
foreach (_component Widgets Plot Geometry)
  add_library (EigenQt::${_component} ${_cisst_eigenqt_library_type} IMPORTED GLOBAL)
  set_target_properties (EigenQt::${_component} PROPERTIES
                         INTERFACE_INCLUDE_DIRECTORIES "${cisst_BINARY_DIR}/include"
                         INTERFACE_COMPILE_FEATURES cxx_std_17)
  if (WIN32)
    set_property (TARGET EigenQt::${_component} APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS NOMINMAX)
  endif ()
  if (NOT CISST_BUILD_SHARED_LIBS)
    string (TOUPPER "${_component}" _upper_component)
    set_property (TARGET EigenQt::${_component} APPEND PROPERTY
                  INTERFACE_COMPILE_DEFINITIONS EIGENQT_${_upper_component}_STATIC_DEFINE)
  endif ()

  if (CMAKE_CONFIGURATION_TYPES)
    set (_configs ${CMAKE_CONFIGURATION_TYPES})
  else ()
    set (_configs "${CMAKE_BUILD_TYPE}")
  endif ()
  foreach (_config ${_configs})
    string (TOUPPER "${_config}" _upper_config)
    set (_suffix "")
    if (CMAKE_CONFIGURATION_TYPES)
      set (_suffix "-${_config}")
    endif ()
    set_property (TARGET EigenQt::${_component} APPEND PROPERTY IMPORTED_CONFIGURATIONS "${_config}")
    set_target_properties (EigenQt::${_component} PROPERTIES
      IMPORTED_LOCATION_${_upper_config}
        "${_cisst_eigenqt_library_dir}/${_cisst_eigenqt_prefix}eigenqt${_component}${_suffix}${_cisst_eigenqt_extension}")
    if (WIN32 AND CISST_BUILD_SHARED_LIBS)
      set_target_properties (EigenQt::${_component} PROPERTIES
        IMPORTED_IMPLIB_${_upper_config}
          "${cisst_BINARY_DIR}/lib/${CMAKE_IMPORT_LIBRARY_PREFIX}eigenqt${_component}${_suffix}${CMAKE_IMPORT_LIBRARY_SUFFIX}")
    endif ()
  endforeach ()

  list (APPEND _cisst_eigenqt_byproducts
    "${_cisst_eigenqt_library_dir}/${_cisst_eigenqt_prefix}eigenqt${_component}${_cisst_eigenqt_suffix}${_cisst_eigenqt_extension}")
  if (WIN32 AND CISST_BUILD_SHARED_LIBS)
    list (APPEND _cisst_eigenqt_dlls
      "${_cisst_eigenqt_library_dir}/${_cisst_eigenqt_prefix}eigenqt${_component}${_cisst_eigenqt_suffix}${_cisst_eigenqt_extension}")
    list (APPEND _cisst_eigenqt_byproducts
      "${cisst_BINARY_DIR}/lib/${CMAKE_IMPORT_LIBRARY_PREFIX}eigenqt${_component}${_cisst_eigenqt_suffix}${CMAKE_IMPORT_LIBRARY_SUFFIX}")
  endif ()
endforeach ()

target_link_libraries (EigenQt::Widgets INTERFACE Qt${CISST_QT_VERSION}::Widgets Eigen3::Eigen)
if (CISST_QT_VERSION STREQUAL "6")
  set (_cisst_eigenqt_gl_target Qt6::OpenGLWidgets)
else ()
  set (_cisst_eigenqt_gl_target Qt5::OpenGL)
endif ()
target_link_libraries (EigenQt::Plot INTERFACE EigenQt::Widgets ${_cisst_eigenqt_gl_target} OpenGL::GL)
target_link_libraries (EigenQt::Geometry INTERFACE EigenQt::Plot)

# Older Visual Studio generators do not need byproducts for imported libraries.
if (CMAKE_CONFIGURATION_TYPES AND CMAKE_VERSION VERSION_LESS 3.20)
  set (_cisst_eigenqt_byproducts)
endif ()
ExternalProject_Add (cisstEigenQtExternal
  PREFIX "${cisst_BINARY_DIR}/cisstEigenQtExternal"
  GIT_REPOSITORY "${CISST_EIGENQT_GIT_REPOSITORY}"
  GIT_TAG "${CISST_EIGENQT_GIT_TAG}"
  CMAKE_CACHE_ARGS ${_cisst_eigenqt_args}
  BUILD_BYPRODUCTS ${_cisst_eigenqt_byproducts})
foreach (_component Widgets Plot Geometry)
  add_dependencies (EigenQt::${_component} cisstEigenQtExternal)
endforeach ()

if (WIN32 AND CISST_BUILD_SHARED_LIBS AND CMAKE_CONFIGURATION_TYPES)
  ExternalProject_Add_Step (cisstEigenQtExternal runtime
    COMMAND ${CMAKE_COMMAND} -E make_directory "${EXECUTABLE_OUTPUT_PATH}/$<CONFIG>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_cisst_eigenqt_dlls} "${EXECUTABLE_OUTPUT_PATH}/$<CONFIG>"
    DEPENDEES install)
endif ()

# Run EigenQt's own install rules so exports, DLLs, and versioned symlinks retain
# their normal layout, including when cisst is installed with --prefix or DESTDIR.
ExternalProject_Get_Property (cisstEigenQtExternal BINARY_DIR)
install (CODE "
  execute_process (COMMAND \"${CMAKE_COMMAND}\" --install \"${BINARY_DIR}\"
                   --config \"\${CMAKE_INSTALL_CONFIG_NAME}\"
                   --prefix \"\${CMAKE_INSTALL_PREFIX}\"
                   RESULT_VARIABLE _cisst_eigenqt_install_result)
  if (NOT _cisst_eigenqt_install_result EQUAL 0)
    message (FATAL_ERROR \"EigenQt installation failed\")
  endif ()
" COMPONENT cisstCommon)
set (EigenQt_QT_VERSION "${CISST_QT_VERSION}")
message (STATUS "EigenQt will be built from ${CISST_EIGENQT_GIT_REPOSITORY} (${CISST_EIGENQT_GIT_TAG})")
