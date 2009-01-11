#
# $Id: CMakeLists.txt,v 1.36 2008/03/13 20:42:50 anton Exp $
#
# CMakeLists for cisstVector
#
# (C) Copyright 2003-2008 Johns Hopkins University (JHU), All Rights
# Reserved.
# 
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

# set the library name
SET(LIBRARY cisstVector)

# set dependencies
SET(DEPENDENCIES
    cisstCommon)

# all source files
SET(SOURCE_FILES
    vctAngleRotation2.cpp
    vctAxisAngleRotation3.cpp
    vctFrameBase.cpp
    vctFrame4x4ConstBase.cpp
    vctMatrixRotation2.cpp
    vctMatrixRotation2Base.cpp
    vctMatrixRotation3.cpp
    vctMatrixRotation3Base.cpp
    vctMatrixRotation3ConstBase.cpp
    vctPrintf.cpp
    vctQuaternion.cpp
    vctQuaternionBase.cpp
    vctQuaternionRotation3.cpp
    vctQuaternionRotation3Base.cpp
    vctRandom.cpp
    vctRodriguezRotation3.cpp
    vctRodriguezRotation3Base.cpp
    vctTypes.cpp
    )

# all header files
SET(HEADER_FILES
    vctAngleRotation2.h
    vctAxisAngleRotation3.h
    vctBarycentricVector.h
    vctBinaryOperations.h
    vctContainerTraits.h
    vctDeterminant.h

    vctDynamicConstMatrixBase.h
    vctDynamicConstMatrixRef.h
    vctDynamicConstNArrayBase.h
    vctDynamicConstNArrayRef.h
    vctDynamicConstVectorBase.h
    vctDynamicConstVectorRef.h

    vctDynamicCompactLoopEngines.h

    vctDynamicMatrix.h
    vctDynamicMatrixBase.h
    vctDynamicMatrixLoopEngines.h
    vctDynamicMatrixOwner.h
    vctDynamicMatrixRef.h
    vctDynamicMatrixRefOwner.h
    vctDynamicMatrixTypes.h

    vctDynamicNArray.h
    vctDynamicNArrayBase.h
    vctDynamicNArrayLoopEngines.h
    vctDynamicNArrayOwner.h
    vctDynamicNArrayRef.h
    vctDynamicNArrayRefOwner.h

    vctDynamicVector.h
    vctDynamicVectorBase.h
    vctDynamicVectorLoopEngines.h
    vctDynamicVectorOwner.h
    vctDynamicVectorRef.h
    vctDynamicVectorRefOwner.h
    vctDynamicVectorTypes.h

    vctExport.h
    vctFastCopy.h
 
    vctFixedSizeConstVectorBase.h
    vctFixedSizeConstVectorRef.h
    vctFixedSizeConstMatrixBase.h
    vctFixedSizeConstMatrixRef.h

    vctFixedSizeMatrix.h
    vctFixedSizeMatrixBase.h
    vctFixedSizeMatrixLoopEngines.h
    vctFixedSizeMatrixRef.h
    vctFixedSizeMatrixTraits.h
    vctFixedSizeMatrixTypes.h

    vctFixedSizeVector.h
    vctFixedSizeVectorBase.h
    vctFixedSizeVectorRef.h
    vctFixedSizeVectorRecursiveEngines.h
    vctFixedSizeVectorTraits.h
    vctFixedSizeVectorTypes.h

    vctFixedStrideMatrixIterator.h
    vctFixedStrideVectorIterator.h
    vctFrameBase.h
    vctFrame4x4.h
    vctFrame4x4Base.h
    vctFrame4x4ConstBase.h

    vctForwardDeclarations.h
    vctMatrixRotation2.h
    vctMatrixRotation2Base.h
    vctMatrixRotation3.h
    vctMatrixRotation3Ref.h
    vctMatrixRotation3Base.h
    vctMatrixRotation3ConstRef.h
    vctMatrixRotation3ConstBase.h
    vctPrintf.h
    vctQuaternion.h
    vctQuaternionBase.h
    vctQuaternionRotation3.h
    vctQuaternionRotation3Base.h
    vctRandom.h
    vctRandomFixedSizeVector.h
    vctRandomFixedSizeMatrix.h
    vctRandomDynamicVector.h
    vctRandomDynamicMatrix.h
    vctRandomDynamicNArray.h
    vctRandomTransformations.h
    vctRodriguezRotation3.h
    vctRodriguezRotation3Base.h
    vctStoreBackBinaryOperations.h
    vctStoreBackUnaryOperations.h
    vctTransformationTypes.h
    vctTypes.h
    vctUnaryOperations.h

    vctVarStrideNArrayIterator.h
    vctVarStrideMatrixIterator.h
    vctVarStrideVectorIterator.h
    )


CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)
