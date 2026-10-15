cmake_minimum_required (VERSION 3.16)
project (cisstDownstreamSwig LANGUAGES CXX)

find_package (cisst 1.5 REQUIRED COMPONENTS cisstCommon)
if (SWIG_FOUND)
  message (FATAL_ERROR "Finding cisst unexpectedly discovered SWIG")
endif ()

option (BUILD_WRAPPER "Generate and build a Python wrapper" ON)
if (BUILD_WRAPPER)
  # Python is required by this consumer even if cisst was built without Python.
  find_package (Python REQUIRED COMPONENTS Interpreter Development)
  set (PYTHON_ALL_INCLUDE_DIRS ${Python_INCLUDE_DIRS})
  set (PYTHON_ALL_LIBRARIES ${Python_LIBRARIES})
  set (LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/lib")
  set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}")
  cisst_add_swig_module (MODULE sample)
  # These target settings must reach both SWIG and the generated C++ wrapper.
  target_include_directories (samplePython PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/headers")
  set_property (TARGET samplePython APPEND PROPERTY SWIG_COMPILE_OPTIONS -DTEST_SWIG_TARGET_OPTIONS)
  enable_testing ()
  add_test (NAME import_wrapper COMMAND "${Python_EXECUTABLE}" -c
            "import sys; sys.path.insert(0, r'$<TARGET_FILE_DIR:samplePython>'); import samplePython; assert samplePython.answer() == 42")
endif ()
