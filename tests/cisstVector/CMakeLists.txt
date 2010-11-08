#
# $Id$
#
# CMakeLists for cisstVector tests
#
# (C) Copyright 2003-2010 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# all source files
set (SOURCE_FILES
     vctArrayIteratorTest.cpp
     vctAxisAngleRotation3Test.cpp
     vctDeterminantTest.cpp
     vctDouble3Test.cpp

     vctDynamicMatrixTest.cpp
     vctDynamicMatrixRefTest.cpp

     vctDynamicNArrayTest.cpp
     vctDynamicNArrayRefTest.cpp

     vctDynamicVectorTest.cpp
     vctDynamicVectorRefTest.cpp

     vctFixedSizeVectorTest.cpp
     vctFixedSizeVectorRefTest.cpp

     vctFixedSizeMatrixTest.cpp
     vctFixedSizeMatrixRefTest.cpp

     vctFixedStrideMatrixIteratorTest.cpp
     vctFixedStrideVectorIteratorTest.cpp

     vctFrameBaseTest.cpp
     vctFrame4x4Test.cpp
     vctMatrixRotation2Test.cpp
#    vctMatrixRotation2BaseTest.cpp
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

     vctDynamicMatrixTest.h
     vctDynamicMatrixRefTest.h

     vctDynamicNArrayTest.h
     vctDynamicNArrayRefTest.h

     vctDynamicVectorTest.h
     vctDynamicVectorRefTest.h

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
#    vctMatrixRotation2BaseTest.h
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

# Add executable for C++ tests
add_executable (cisstVectorTests ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries (cisstVectorTests cisstTestsMain)
cisst_target_link_libraries (cisstVectorTests cisstCommon cisstVector)

# To generate a CTest list of tests
cisst_add_test (cisstVectorTests ITERATIONS 10 INSTANCES 2)

