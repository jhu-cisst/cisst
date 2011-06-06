#
# $Id$
#
# CMakeLists for cisstVector
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
     vctAngleRotation2.cpp
     vctAxisAngleRotation3.cpp
     vctEulerRotation3.cpp
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
set (HEADER_FILES
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

     vctEulerRotation3.h
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

# Add vctPlot2D base class if any rendering code is available, i.e. OpenGL or VTK
if (CISST_HAS_OPENGL OR CISST_HAS_VTK)
  set (SOURCE_FILES
       ${SOURCE_FILES}
       vctPlot2DBase.cpp
       vctPlot2DOpenGL.cpp
       )
  set (HEADER_FILES
       ${HEADER_FILES}
       vctPlot2DBase.h
       vctPlot2DOpenGL.h
       )
endif (CISST_HAS_OPENGL OR CISST_HAS_VTK)


cisst_add_library (
  LIBRARY cisstVector
  DEPENDENCIES cisstCommon
  SOURCE_FILES ${SOURCE_FILES}
  HEADER_FILES ${HEADER_FILES})


# Qt widgets
if (CISST_HAS_OPENGL AND CISST_HAS_QT)

  # To include Qt generated files
  include_directories (${CMAKE_CURRENT_BINARY_DIR})
  qt4_wrap_cpp (QT_WRAPPED_CPP
                ../../include/cisstVector/vctPlot2DOpenGLQtWidget.h)

  cisst_add_library (
    LIBRARY cisstVectorQt
    LIBRARY_DIR cisstVector
    DEPENDENCIES cisstCommon
                 cisstVector
    SOURCE_FILES vctPlot2DOpenGLQtWidget.cpp
    HEADER_FILES vctPlot2DOpenGLQtWidget.h
                 vctExportQt.h
    ADDITIONAL_SOURCE_FILES ${QT_WRAPPED_CPP})

  target_link_libraries (cisstVectorQt ${QT_LIBRARIES})

endif (CISST_HAS_OPENGL AND CISST_HAS_QT)


# FLTK widgets
if (CISST_HAS_OPENGL AND CISST_HAS_FLTK)

  cisst_add_library (
    LIBRARY cisstVectorFLTK
    LIBRARY_DIR cisstVector
    DEPENDENCIES cisstCommon
                 cisstVector
    SOURCE_FILES vctPlot2DOpenGLFLTK.cpp
    HEADER_FILES vctPlot2DOpenGLFLTK.h
                 vctExportFLTK.h)

endif (CISST_HAS_OPENGL AND CISST_HAS_FLTK)

# Python wrappers
if (CISST_HAS_SWIG_PYTHON)
  cisst_add_swig_module(MODULE cisstVector
                        INTERFACE_DIRECTORY ../../include/cisstVector
			CISST_LIBRARIES cisstCommon cisstVector)
endif (CISST_HAS_SWIG_PYTHON)
