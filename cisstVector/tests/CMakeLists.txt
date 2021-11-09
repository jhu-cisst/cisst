#
# CMakeLists for cisstVector tests
#
# (C) Copyright 2003-2019 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# paths for headers/libraries
cisst_set_directories (cisstCommon cisstVector cisstTestsDriver)

# all source files
set (SOURCE_FILES
     vctArrayIteratorTest.cpp
     vctAxisAngleRotation3Test.cpp
     vctDeterminantTest.cpp
     vctDouble3Test.cpp

     vctDataFunctionsFixedSizeVectorTest.cpp
     vctDataFunctionsDynamicVectorTest.cpp
     vctDataFunctionsFixedSizeMatrixTest.cpp
     vctDataFunctionsDynamicMatrixTest.cpp
     vctDataFunctionsTransformationsTest.cpp

     vctDynamicMatrixTest.cpp
     vctDynamicMatrixRefTest.cpp

     vctDynamicNArrayTest.cpp
     vctDynamicNArrayRefTest.cpp

     vctDynamicVectorTest.cpp
     vctDynamicVectorRefTest.cpp

     vctEulerRotation3Test.cpp

     vctFixedSizeVectorTest.cpp
     vctFixedSizeVectorRefTest.cpp

     vctFixedSizeMatrixTest.cpp
     vctFixedSizeMatrixRefTest.cpp

     vctFixedStrideMatrixIteratorTest.cpp
     vctFixedStrideVectorIteratorTest.cpp

     vctFrameBaseTest.cpp
     vctFrame4x4Test.cpp
     vctMatrixRotation2Test.cpp
     vctMatrixRotation3Test.cpp
     vctQuaternionTest.cpp
     vctQuaternionBaseTest.cpp
     vctQuaternionRotation3Test.cpp
     vctQuaternionRotation3BaseTest.cpp
     vctRodriguezRotation3Test.cpp

     vctVarStrideMatrixIteratorTest.cpp
     vctVarStrideNArrayIteratorTest.cpp
     vctVarStrideVectorIteratorTest.cpp
    )

# all header files
set (HEADER_FILES
     vctArrayIteratorTest.h
     vctAxisAngleRotation3Test.h
     vctDeterminantTest.h
     vctDouble3Test.h

     vctDataFunctionsFixedSizeVectorTest.h
     vctDataFunctionsDynamicVectorTest.h
     vctDataFunctionsFixedSizeMatrixTest.h
     vctDataFunctionsDynamicMatrixTest.h
     vctDataFunctionsTransformationsTest.h

     vctDynamicMatrixTest.h
     vctDynamicMatrixRefTest.h

     vctDynamicNArrayTest.h
     vctDynamicNArrayRefTest.h

     vctDynamicVectorTest.h
     vctDynamicVectorRefTest.h

     vctEulerRotation3Test.cpp

     vctFixedSizeVectorTest.h
     vctFixedSizeVectorRefTest.h

     vctFixedSizeMatrixTest.h
     vctFixedSizeMatrixRefTest.h

     vctFixedStrideMatrixIteratorTest.h
     vctFixedStrideVectorIteratorTest.h

     vctFrameBaseTest.h
     vctFrame4x4Test.h

     vctGenericContainerTest.h
     vctGenericIteratorTest.h
     vctGenericMatrixTest.h
     vctGenericNArrayTest.h
     vctGenericRotationTest.h
     vctGenericVectorTest.h

     vctMatrixRotation2Test.h
     vctMatrixRotation3Test.h
     vctQuaternionTest.h
     vctQuaternionBaseTest.h
     vctQuaternionRotation3Test.h
     vctQuaternionRotation3BaseTest.h
     vctRodriguezRotation3Test.h

     vctVarStrideMatrixIteratorTest.h
     vctVarStrideNArrayIteratorTest.h
     vctVarStrideVectorIteratorTest.h
     )

if (CISST_HAS_OPENGL OR CISST_HAS_VTK)
  set (HEADER_FILES
       ${HEADER_FILES}
       vctPlot2DBaseTest.h)
  set (SOURCE_FILES
       ${SOURCE_FILES}
       vctPlot2DBaseTest.cpp)
endif (CISST_HAS_OPENGL OR CISST_HAS_VTK)

if (CISST_HAS_JSON)
  set (HEADER_FILES
       ${HEADER_FILES}
       vctDataFunctionsFixedSizeVectorJSONTest.h
       vctDataFunctionsDynamicVectorJSONTest.h
       vctDataFunctionsFixedSizeMatrixJSONTest.h
       vctDataFunctionsDynamicMatrixJSONTest.h
       vctDataFunctionsTransformationsJSONTest.h)

  set (SOURCE_FILES
       ${SOURCE_FILES}
       vctDataFunctionsFixedSizeVectorJSONTest.cpp
       vctDataFunctionsDynamicVectorJSONTest.cpp
       vctDataFunctionsFixedSizeMatrixJSONTest.cpp
       vctDataFunctionsDynamicMatrixJSONTest.cpp
       vctDataFunctionsTransformationsJSONTest.cpp)
endif (CISST_HAS_JSON)

# Add executable for C++ tests
add_executable (cisstVectorTests ${SOURCE_FILES} ${HEADER_FILES})
set_property (TARGET cisstVectorTests PROPERTY FOLDER "cisstVector/tests")
target_link_libraries (cisstVectorTests cisstTestsDriver)
cisst_target_link_libraries (cisstVectorTests cisstCommon cisstVector cisstTestsDriver)

# To generate a CTest list of tests
cisst_add_test (cisstVectorTests ITERATIONS 10 INSTANCES 2)
