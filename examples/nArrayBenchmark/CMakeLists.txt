#
# $Id: CMakeLists.txt,v 1.3 2007/08/03 03:53:06 anton Exp $
#
# (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
# Reserved.
# 
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

# This project is designed to be built outside the Insight source tree.
# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})

  ADD_EXECUTABLE(Subarray_ITK_Benchmark Subarray_ITK_Benchmark.cpp )
  CISST_REQUIRES("Subarray_ITK_Benchmark" "cisstCommon;cisstOSAbstraction")
  TARGET_LINK_LIBRARIES(Subarray_ITK_Benchmark ITKCommon)

  ADD_EXECUTABLE(Subarray_nArray_Benchmark Subarray_nArray_Benchmark.cpp)
  CISST_REQUIRES("Subarray_nArray_Benchmark" "cisstCommon;cisstVector;cisstOSAbstraction")

  ADD_EXECUTABLE(ImageAdd_ITK_Benchmark ImageAdd_ITK_Benchmark.cpp )
  CISST_REQUIRES("ImageAdd_ITK_Benchmark" "cisstCommon;cisstOSAbstraction")
  TARGET_LINK_LIBRARIES(ImageAdd_ITK_Benchmark ITKCommon)

  ADD_EXECUTABLE(ImageAdd_nArray_Benchmark ImageAdd_nArray_Benchmark.cpp)
  CISST_REQUIRES("ImageAdd_nArray_Benchmark" "cisstCommon;cisstVector;cisstOSAbstraction")

ENDIF(ITK_FOUND)

ADD_EXECUTABLE(optimizedEngines optimizedEngines.cpp)
CISST_REQUIRES("optimizedEngines" "cisstCommon;cisstVector;cisstOSAbstraction")
