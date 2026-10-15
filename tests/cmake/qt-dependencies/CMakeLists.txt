cmake_minimum_required (VERSION 3.16)
project (cisstQtDependencies LANGUAGES CXX)

# Configure with -Dcisst_DIR=<build or install package directory>.
# MODE=core can be used with -DCMAKE_DISABLE_FIND_PACKAGE_Qt6=ON (or Qt5).
# MODE=qt, repeated, and all require the matching Qt installation.
set (MODE core CACHE STRING "Consumer scenario: core, qt, repeated, or all")
set (_core cisstCommon cisstVector cisstOSAbstraction cisstMultiTask cisstParameterTypes)
if (MODE STREQUAL "all")
  find_package (cisst 1.5 REQUIRED)
elseif (MODE STREQUAL "qt")
  find_package (cisst 1.5 REQUIRED COMPONENTS cisstMultiTaskQt)
else ()
  find_package (cisst 1.5 REQUIRED COMPONENTS ${_core})
  if (TARGET Qt5::Core OR TARGET Qt6::Core)
    message (FATAL_ERROR "Core components unexpectedly discovered Qt")
  endif ()

  # Exercise transitive links, generator expressions, and cycles independently
  # of which optional libraries the local cisst build happens to contain.
  add_library (qt_test_first INTERFACE)
  add_library (qt_test_second INTERFACE)
  target_link_libraries (qt_test_first INTERFACE qt_test_second)
  target_link_libraries (qt_test_second INTERFACE qt_test_first)
  _cisst_requires_qt (_needs_qt qt_test_first)
  if (_needs_qt)
    message (FATAL_ERROR "A dependency cycle without Qt was classified as Qt")
  endif ()
  target_link_libraries (qt_test_second INTERFACE "$<LINK_ONLY:Qt6::Core>")
  _cisst_requires_qt (_needs_qt qt_test_first)
  if (NOT _needs_qt)
    message (FATAL_ERROR "Transitive Qt dependency in LINK_ONLY was missed")
  endif ()

  if (MODE STREQUAL "repeated")
    find_package (cisst 1.5 REQUIRED COMPONENTS cisstMultiTaskQt)
    # A later core-only lookup must also work with Qt already loaded.
    find_package (cisst 1.5 REQUIRED COMPONENTS ${_core})
  elseif (NOT MODE STREQUAL "core")
    message (FATAL_ERROR "Unknown MODE: ${MODE}")
  endif ()
endif ()

add_executable (qt_dependency_consumer main.cpp)
target_link_libraries (qt_dependency_consumer PRIVATE ${cisst_LIBRARIES})
if (NOT MODE STREQUAL "core")
  if (NOT TARGET Qt${CISST_QT_VERSION}::Widgets)
    message (FATAL_ERROR "Qt consumer did not discover Qt Widgets")
  endif ()
  target_compile_definitions (qt_dependency_consumer PRIVATE TEST_QT)
  target_link_libraries (qt_dependency_consumer PRIVATE cisst::cisstMultiTaskQt)
endif ()
