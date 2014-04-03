#
#
# (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

set (REQUIRED_CISST_LIBRARIES cisstCommon cisstVector cisstOSAbstraction)
find_package (cisst COMPONENTS ${REQUIRED_CISST_LIBRARIES} QUIET)

if (cisst_FOUND_AS_REQUIRED)

  include (${CISST_USE_FILE})

  # This project is designed to be built outside the Insight source tree.
  # Find ITK.
  find_package (ITK QUIET)

  if (ITK_FOUND)
    include (${ITK_USE_FILE})

    add_executable (vctExSubarrayITKBenchmark Subarray_ITK_Benchmark.cpp)
    set_property (TARGET vctExSubarrayITKBenchmark PROPERTY FOLDER "cisstVector/examples")
    cisst_target_link_libraries (vctExSubarrayITKBenchmark ${REQUIRED_CISST_LIBRARIES})
    target_link_libraries (vctExSubarrayITKBenchmark ITKCommon)

    add_executable (vctExSubarrayNArrayBenchmark Subarray_nArray_Benchmark.cpp)
    set_property (TARGET vctExSubarrayNArrayBenchmark PROPERTY FOLDER "cisstVector/examples")
    cisst_target_link_libraries (vctExSubarrayNArrayBenchmark ${REQUIRED_CISST_LIBRARIES})

    add_executable (vctExImageAddITKBenchmark ImageAdd_ITK_Benchmark.cpp)
    set_property (TARGET vctExImageAddITKBenchmark PROPERTY FOLDER "cisstVector/examples")
    cisst_target_link_libraries (vctExImageAddITKBenchmark ${REQUIRED_CISST_LIBRARIES})
    target_link_libraries (vctExImageAddITKBenchmark ITKCommon)

    add_executable (vctExImageAddNArrayBenchmark ImageAdd_nArray_Benchmark.cpp)
    set_property (TARGET vctExImageAddNArrayBenchmark PROPERTY FOLDER "cisstVector/examples")
    cisst_target_link_libraries (vctExImageAddNArrayBenchmark ${REQUIRED_CISST_LIBRARIES})

  else (ITK_FOUND)
    message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR}: NArrayBenchmark will not be compiled, they require ITK")
    mark_as_advanced (ITK_FOUND ITK_DIR)
  endif (ITK_FOUND)

  add_executable (vctExOptimizedEngines optimizedEngines.cpp)
  set_property (TARGET vctExOptimizedEngines PROPERTY FOLDER "cisstVector/examples")
  cisst_target_link_libraries (vctExOptimizedEngines ${REQUIRED_CISST_LIBRARIES})

else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)
