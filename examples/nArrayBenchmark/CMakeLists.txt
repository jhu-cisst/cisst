#
# $Id$
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

if (BUILD_LIBS_cisstVector AND BUILD_LIBS_cisstOSAbstraction)

  # This project is designed to be built outside the Insight source tree.
  # Find ITK.
  find_package (ITK)

  if (ITK_FOUND)
    include (${ITK_USE_FILE})

    add_executable (Subarray_ITK_Benchmark Subarray_ITK_Benchmark.cpp )
    cisst_target_link_libraries (Subarray_ITK_Benchmark cisstCommon cisstOSAbstraction)
    target_link_libraries (Subarray_ITK_Benchmark ITKCommon)

    add_executable (Subarray_nArray_Benchmark Subarray_nArray_Benchmark.cpp)
    cisst_target_link_libraries (Subarray_nArray_Benchmark cisstCommon cisstVector cisstOSAbstraction)

    add_executable (ImageAdd_ITK_Benchmark ImageAdd_ITK_Benchmark.cpp)
    cisst_target_link_libraries (ImageAdd_ITK_Benchmark cisstCommoncisstOSAbstraction)
    target_link_libraries (ImageAdd_ITK_Benchmark ITKCommon)

    add_executable (ImageAdd_nArray_Benchmark ImageAdd_nArray_Benchmark.cpp)
    cisst_target_link_libraries (ImageAdd_nArray_Benchmark cisstCommon cisstVector cisstOSAbstraction)

  else (ITK_FOUND)
    message (WARNING " nArrayBenchmark examples Subarray_ITK_Benchmark, Subarray_nArray_Benchmark, ImageAdd_ITK_Benchmark and ImageAdd_nArray_Benchmark will not be compiled since they require ITK")
    mark_as_advanced (ITK_FOUND ITK_DIR)
  endif (ITK_FOUND)

  add_executable (optimizedEngines optimizedEngines.cpp)
  cisst_target_link_libraries (optimizedEngines cisstCommon cisstVector cisstOSAbstraction)

else (BUILD_LIBS_cisstVector AND BUILD_LIBS_cisstOSAbstraction)
  message (WARNING " nArrayBenchmark examples will not be compiled since they require BUILD_LIBS_cisstVector and BUILD_LIBS_cisstOSAbstraction")
endif (BUILD_LIBS_cisstVector AND BUILD_LIBS_cisstOSAbstraction)