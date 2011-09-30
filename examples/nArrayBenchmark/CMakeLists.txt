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

set (REQUIRED_CISST_LIBRARIES cisstCommon cisstVector cisstOSAbstraction)
find_package (cisst COMPONENTS ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND_AS_REQUIRED)

  include (${CISST_USE_FILE})

  # This project is designed to be built outside the Insight source tree.
  # Find ITK.
  find_package (ITK QUIET)

  if (ITK_FOUND)
    include (${ITK_USE_FILE})

    add_executable (Subarray_ITK_Benchmark Subarray_ITK_Benchmark.cpp )
    cisst_target_link_libraries (Subarray_ITK_Benchmark ${REQUIRED_CISST_LIBRARIES})
    target_link_libraries (Subarray_ITK_Benchmark ITKCommon)

    add_executable (Subarray_nArray_Benchmark Subarray_nArray_Benchmark.cpp)
    cisst_target_link_libraries (Subarray_nArray_Benchmark ${REQUIRED_CISST_LIBRARIES})

    add_executable (ImageAdd_ITK_Benchmark ImageAdd_ITK_Benchmark.cpp)
    cisst_target_link_libraries (ImageAdd_ITK_Benchmark ${REQUIRED_CISST_LIBRARIES})
    target_link_libraries (ImageAdd_ITK_Benchmark ITKCommon)

    add_executable (ImageAdd_nArray_Benchmark ImageAdd_nArray_Benchmark.cpp)
    cisst_target_link_libraries (ImageAdd_nArray_Benchmark ${REQUIRED_CISST_LIBRARIES})

  else (ITK_FOUND)
    message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR}: nArrayBenchmark examples Subarray_ITK_Benchmark, Subarray_nArray_Benchmark, ImageAdd_ITK_Benchmark and ImageAdd_nArray_Benchmark will not be compiled since they require ITK")
    mark_as_advanced (ITK_FOUND ITK_DIR)
  endif (ITK_FOUND)

  add_executable (optimizedEngines optimizedEngines.cpp)
  cisst_target_link_libraries (optimizedEngines ${REQUIRED_CISST_LIBRARIES})

else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)
