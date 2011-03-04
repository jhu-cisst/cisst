#
# $Id$
#
# CMakeLists for cisstStereoVision
#
# (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# set dependencies
SET(DEPENDENCIES
    cisstCommon
    cisstOSAbstraction
    cisstVector
    cisstMultiTask
    )

# all source files
SET(SOURCE_FILES
  # Public System API
    svlTypes.cpp
    svlClassServices.cpp
    svlSampleBlobs.cpp
    svlSampleText.cpp
    svlSampleTargets.cpp
    svlSampleTransform3D.cpp
    svlSampleMatrixTypes.cpp
    svlSampleMatrix.cpp
    svlSampleImageTypes.cpp
    svlSampleImage.cpp
    svlSample.cpp
    svlFile.cpp
    svlStreamManager.cpp
    svlFilterBase.cpp
    svlFilterInput.cpp
    svlFilterOutput.cpp
    svlFilterSourceBase.cpp
    svlStreamProc.cpp
    svlSyncPoint.cpp
    svlSeries.cpp
    svlRenderTargets.cpp
    svlStreamBranchSource.cpp
    svlSampleQueue.cpp
    svlImageIO.cpp
    svlVideoIO.cpp
    svlCameraGeometry.cpp
    svlBufferMemory.cpp
    svlBufferSample.cpp
    svlBufferImage.cpp
    svlConverters.cpp
    svlImageProcessingHelper.h    # private header
    svlImageProcessingHelper.cpp
    svlImageProcessing.cpp
    svlDrawHelper.h               # private header
    svlDrawHelper.cpp
    svlDraw.cpp
    svlOverlayObjects.cpp

  # Filter API
    svlFilterComputationalStereo.cpp
    svlFilterDisparityMapToSurface.cpp
    svlFilterImageBlobDetector.cpp
    svlFilterImageBlobTracker.cpp
    svlFilterImageCenterFinder.cpp
    svlFilterImageChannelSwapper.cpp
    svlFilterImageColorConverter.cpp
    svlFilterImageColorSegmentation.cpp
    svlFilterImageConvolution.cpp
    svlFilterImageCropper.cpp
    svlFilterImageDeinterlacer.cpp
    svlFilterImageDilation.cpp
    svlFilterImageErosion.cpp
    svlFilterImageExposureCorrection.cpp
    svlFilterImageFileWriter.cpp
    svlFilterImageFlipRotate.cpp
    svlFilterImageOverlay.cpp
    svlFilterImageRectifier.cpp
    svlFilterImageResizer.cpp
    svlFilterImageSampler.cpp
    svlFilterImageThresholding.cpp
    svlFilterImageTracker.cpp
    svlFilterImageTranslation.cpp
    svlFilterImageUnsharpMask.cpp
    svlFilterLightSourceBuddy.cpp
    svlFilterSourceBuffer.cpp
    svlFilterSourceDummy.cpp
    svlFilterSourceImageFile.cpp
    svlFilterSourceTextFile.cpp
    svlFilterSourceVideoCapture.cpp
    svlFilterSourceVideoFile.cpp
    svlFilterSplitter.cpp
    svlFilterStereoImageJoiner.cpp
    svlFilterStereoImageOptimizer.cpp
    svlFilterStereoImageSplitter.cpp
    svlFilterStreamTypeConverter.cpp
    svlFilterVideoExposureManager.cpp
    svlFilterVideoFileWriter.cpp
    svlRequiredInterfaces.cpp

  # Private System API
    svlImageCodecBMP.h             # private header
    svlImageCodecBMP.cpp
    svlImageCodecPPM.h             # private header
    svlImageCodecPPM.cpp
    svlStereoDP.h                  # private header
    svlStereoDP.cpp
    svlStereoDPMono.h              # private header
    svlStereoDPMono.cpp

  # Trackers
    svlTrackerMSBruteForce.cpp
    )

# all header files
SET(HEADER_FILES
  # Public System API
    svlForwardDeclarations.h
    svlDefinitions.h
    svlProcInfo.h
    svlTypes.h
    svlTypeCheckers.h
    svlSampleBlobs.h
    svlSampleText.h
    svlSampleTargets.h
    svlSampleTransform3D.h
    svlSampleMatrixTypes.h
    svlSampleMatrixCustom.h
    svlSampleMatrix.h
    svlSampleImageTypes.h
    svlSampleImageCustom.h
    svlSampleImage.h
    svlSample.h
    svlFile.h
    svlStreamManager.h
    svlFilterBase.h
    svlFilterInput.h
    svlFilterOutput.h
    svlFilterSourceBase.h
    svlStreamProc.h
    svlSyncPoint.h
    svlSeries.h
    svlRenderTargets.h
    svlStreamBranchSource.h
    svlSampleQueue.h
    svlExport.h
    svlImageIO.h
    svlVideoIO.h
    svlCameraGeometry.h
    svlBufferMemory.h
    svlBufferSample.h
    svlBufferImage.h
    svlConverters.h
    svlImageProcessing.h
    svlDraw.h
    svlOverlayObjects.h

  # Filter API
    svlFilterComputationalStereo.h
    svlFilterDisparityMapToSurface.h
    svlFilterImageBlobDetector.h
    svlFilterImageBlobTracker.h
    svlFilterImageCenterFinder.h
    svlFilterImageChannelSwapper.h
    svlFilterImageColorConverter.h
    svlFilterImageColorSegmentation.h
    svlFilterImageConvolution.h
    svlFilterImageCropper.h
    svlFilterImageDeinterlacer.h
    svlFilterImageDilation.h
    svlFilterImageErosion.h
    svlFilterImageExposureCorrection.h
    svlFilterImageFileWriter.h
    svlFilterImageFlipRotate.h
    svlFilterImageOverlay.h
    svlFilterImageRectifier.h
    svlFilterImageResizer.h
    svlFilterImageSampler.h
    svlFilterImageThresholding.h
    svlFilterImageTracker.h
    svlFilterImageTranslation.h
    svlFilterImageUnsharpMask.h
    svlFilterLightSourceBuddy.h
    svlFilterSourceBuffer.h
    svlFilterSourceDummy.h
    svlFilterSourceImageFile.h
    svlFilterSourceTextFile.h
    svlFilterSourceVideoCapture.h
    svlFilterSourceVideoFile.h
    svlFilterSplitter.h
    svlFilterStereoImageJoiner.h
    svlFilterStereoImageOptimizer.h
    svlFilterStereoImageSplitter.h
    svlFilterStreamTypeConverter.h
    svlFilterVideoExposureManager.h
    svlFilterVideoFileWriter.h
    svlRequiredInterfaces.h

  # Trackers
    svlTrackerMSBruteForce.h
    )


# cisstNumerical dependency
IF(BUILD_LIBS_cisstNumerical AND CISST_HAS_CISSTNETLIB)
    SET(DEPENDENCIES
        ${DEPENDENCIES}
        cisstNumerical
        )
    SET(SOURCE_FILES
        ${SOURCE_FILES}
        svlTrackerCisst.cpp
        trackers/cisst/svlTrackerCisstWSSD.h      # private header
        trackers/cisst/svlTrackerCisstWSSD.cpp
        trackers/cisst/svlTrackerCisstNCC.h       # private header
        trackers/cisst/svlTrackerCisstNCC.cpp
        trackers/cisst/svlTrackerCisstBase.h      # private header
        )
        SET(HEADER_FILES
        ${HEADER_FILES}
        svlTrackerCisst.h
        )
ENDIF(BUILD_LIBS_cisstNumerical AND CISST_HAS_CISSTNETLIB)


## WebGUI
#IF(SWIG_FOUND AND CISST_HAS_XML)
#    SET(SOURCE_FILES
#        ${SOURCE_FILES}
#        svlWebPublisher.cpp
#        svlWebObjectBase.cpp
#        svlWebXMLObject.cpp
#        svlWebFileObject.cpp
#        )
#    SET(HEADER_FILES
#        ${HEADER_FILES}
#        svlWebPublisher.h
#        svlWebObjectBase.h
#        svlWebXMLObject.h
#        svlWebFileObject.h
#        )
#    SET(SVL_ADDITIONAL_LIBRARIES
#        "${SVL_ADDITIONAL_LIBRARIES}"
#        "${CMAKE_THREAD_LIBS_INIT}"
#        "${PYTHON_LIBRARIES}"
##        "debug ${PYTHON_DEBUG_LIBRARIES}"
##        "optimized ${PYTHON_LIBRARIES}"
#        )
#    SET(CISST_SVL_HAS_WEBGUI "ON" CACHE INTERNAL "WebGUI switch")
#ENDIF(SWIG_FOUND AND CISST_HAS_XML)

IF(WIN32)
    SET(PROGRAMFILES_DIR "$ENV{SystemDrive}/Program Files")
    SET(PROGRAMFILES_X86_DIR "$ENV{SystemDrive}/Program Files (x86)")
ENDIF(WIN32)


# OpenCV 2.2+
IF(CISST_SVL_HAS_OPENCV2 OR NOT OpenCV2_OPTIONS)
    SET(OpenCV2_OPTIONS
        OpenCV2_INCLUDE_DIRS
        OpenCV2_LIBRARIES
        CACHE INTERNAL "List of OpenCV 2.2+ options"
        )

    FIND_PACKAGE(OpenCV2)
    IF(OpenCV2_FOUND)
        MARK_AS_ADVANCED(FORCE ${OpenCV2_OPTIONS})

        SET(SVL_ADDITIONAL_INCLUDE_DIRECTORIES
            "${SVL_ADDITIONAL_INCLUDE_DIRECTORIES}"
            "${OpenCV2_INCLUDE_DIRS}"
            )
        SET(SVL_ADDITIONAL_LIBRARIES
            "${SVL_ADDITIONAL_LIBRARIES}"
            "${OpenCV2_LIBRARIES}"
            )
        SET(SOURCE_FILES
            ${SOURCE_FILES}
            svlFilterImageZoom.cpp
            svlFilterToolTracker.cpp
            svlVideoCodecOpenCV.h       # private header
            svlVideoCodecOpenCV.cpp
            svlTrackerOCVMeanShift.cpp
            )
        IF(NOT WIN32)
            SET(SOURCE_FILES
                ${SOURCE_FILES}
                svlVidCapSrcOpenCV.h    # private header
                svlVidCapSrcOpenCV.cpp
                )
        ENDIF(NOT WIN32)
        SET(HEADER_FILES
            ${HEADER_FILES}
            svlFilterImageZoom.h
            svlFilterToolTracker.h
            svlTrackerOCVMeanShift.h
            )
        MESSAGE(STATUS "OpenCV 2.2+ found - Switch to advanced view for details")
        OPTION(CISST_SVL_HAS_OPENCV2 "Use OpenCV 2.2+ extensions" ON)

    ELSE(OpenCV2_FOUND)
        IF(CISST_SVL_HAS_OPENCV2)
            MARK_AS_ADVANCED(CLEAR ${OpenCV2_OPTIONS})
            MESSAGE(FATAL_ERROR "OpenCV 2.2+ required. Please specify its location.")
        ELSE(CISST_SVL_HAS_OPENCV2)
            MARK_AS_ADVANCED(FORCE ${OpenCV2_OPTIONS})
            OPTION(CISST_SVL_HAS_OPENCV2 "Compile OpenCV 2.2+ extensions" OFF)
        ENDIF(CISST_SVL_HAS_OPENCV2)
    ENDIF(OpenCV2_FOUND)

ELSE(CISST_SVL_HAS_OPENCV2 OR NOT OpenCV2_OPTIONS)
    MARK_AS_ADVANCED(FORCE ${OpenCV2_OPTIONS})
    OPTION(CISST_SVL_HAS_OPENCV2 "Use OpenCV 2.2+ extensions" OFF)
ENDIF(CISST_SVL_HAS_OPENCV2 OR NOT OpenCV2_OPTIONS)


# OpenCV
IF(NOT OpenCV2_FOUND AND (CISST_SVL_HAS_OPENCV OR NOT OpenCV_OPTIONS))
    SET(OpenCV_OPTIONS
        OpenCV_ROOT_DIR
        OpenCV_INCLUDE_DIRS
        OpenCV_CV_INCLUDE_DIR
        OpenCV_CXCORE_INCLUDE_DIR
        OpenCV_CVAUX_INCLUDE_DIR
        OpenCV_HIGHGUI_INCLUDE_DIR
        OpenCV_LIBRARIES
        OpenCV_CV_LIBRARY
        OpenCV_CXCORE_LIBRARY
        OpenCV_CVAUX_LIBRARY
        OpenCV_CVHAARTRAINING_LIBRARY
        OpenCV_CXTS_LIBRARY
        OpenCV_HIGHGUI_LIBRARY
        OpenCV_ML_LIBRARY
        CACHE INTERNAL "List of OpenCV options"
        )

    FIND_PACKAGE(OpenCV21 COMPONENTS CV CXCORE CVAUX HIGHGUI)
    IF(OpenCV21_FOUND)
        # Copy 'OpenCV21_' variable values to 'OpenCV_' variables
        SET(OpenCV_FOUND YES)
        SET(OpenCV_ROOT_DIR                 ${OpenCV21_ROOT_DIR})
        SET(OpenCV_INCLUDE_DIRS             ${OpenCV21_INCLUDE_DIRS})
        SET(OpenCV_CV_INCLUDE_DIR           ${OpenCV21_CV_INCLUDE_DIR})
        SET(OpenCV_CXCORE_INCLUDE_DIR       ${OpenCV21_CXCORE_INCLUDE_DIR})
        SET(OpenCV_CVAUX_INCLUDE_DIR        ${OpenCV21_CVAUX_INCLUDE_DIR})
        SET(OpenCV_HIGHGUI_INCLUDE_DIR      ${OpenCV21_HIGHGUI_INCLUDE_DIR})
        SET(OpenCV_LIBRARIES                ${OpenCV21_LIBRARIES})
        SET(OpenCV_CV_LIBRARY               ${OpenCV21_CV_LIBRARY})
        SET(OpenCV_CXCORE_LIBRARY           ${OpenCV21_CXCORE_LIBRARY})
        SET(OpenCV_CVAUX_LIBRARY            ${OpenCV21_CVAUX_LIBRARY})
        SET(OpenCV_CVHAARTRAINING_LIBRARY   ${OpenCV21_CVHAARTRAINING_LIBRARY})
        SET(OpenCV_CXTS_LIBRARY             ${OpenCV21_CXTS_LIBRARY})
        SET(OpenCV_HIGHGUI_LIBRARY          ${OpenCV21_HIGHGUI_LIBRARY})
        SET(OpenCV_ML_LIBRARY               ${OpenCV21_ML_LIBRARY})
    ELSE(OpenCV21_FOUND)
        FIND_PACKAGE(OpenCV20 COMPONENTS CV CXCORE CVAUX HIGHGUI)
        IF(OpenCV20_FOUND)
            # Copy 'OpenCV20_' variable values to 'OpenCV_' variables
            SET(OpenCV_FOUND YES)
            SET(OpenCV_ROOT_DIR                 ${OpenCV20_ROOT_DIR})
            SET(OpenCV_INCLUDE_DIRS             ${OpenCV20_INCLUDE_DIRS})
            SET(OpenCV_CV_INCLUDE_DIR           ${OpenCV20_CV_INCLUDE_DIR})
            SET(OpenCV_CXCORE_INCLUDE_DIR       ${OpenCV20_CXCORE_INCLUDE_DIR})
            SET(OpenCV_CVAUX_INCLUDE_DIR        ${OpenCV20_CVAUX_INCLUDE_DIR})
            SET(OpenCV_HIGHGUI_INCLUDE_DIR      ${OpenCV20_HIGHGUI_INCLUDE_DIR})
            SET(OpenCV_LIBRARIES                ${OpenCV20_LIBRARIES})
            SET(OpenCV_CV_LIBRARY               ${OpenCV20_CV_LIBRARY})
            SET(OpenCV_CXCORE_LIBRARY           ${OpenCV20_CXCORE_LIBRARY})
            SET(OpenCV_CVAUX_LIBRARY            ${OpenCV20_CVAUX_LIBRARY})
            SET(OpenCV_CVHAARTRAINING_LIBRARY   ${OpenCV20_CVHAARTRAINING_LIBRARY})
            SET(OpenCV_CXTS_LIBRARY             ${OpenCV20_CXTS_LIBRARY})
            SET(OpenCV_HIGHGUI_LIBRARY          ${OpenCV20_HIGHGUI_LIBRARY})
            SET(OpenCV_ML_LIBRARY               ${OpenCV20_ML_LIBRARY})
        ELSE(OpenCV20_FOUND)
            FIND_PACKAGE(OpenCV COMPONENTS CV CXCORE CVAUX HIGHGUI)
        ENDIF(OpenCV20_FOUND)
    ENDIF(OpenCV21_FOUND)

    # Remove 'OpenCV2x_' variables, if any
    UNSET(OpenCV21_ROOT_DIR CACHE)
    UNSET(OpenCV21_INCLUDE_DIRS CACHE)
    UNSET(OpenCV21_CV_INCLUDE_DIR CACHE)
    UNSET(OpenCV21_CXCORE_INCLUDE_DIR CACHE)
    UNSET(OpenCV21_CVAUX_INCLUDE_DIR CACHE)
    UNSET(OpenCV21_CVCAM_INCLUDE_DIR CACHE)
    UNSET(OpenCV21_HIGHGUI_INCLUDE_DIR CACHE)
    UNSET(OpenCV21_LIBRARIES CACHE)
    UNSET(OpenCV21_CV_LIBRARY CACHE)
    UNSET(OpenCV21_CXCORE_LIBRARY CACHE)
    UNSET(OpenCV21_CVAUX_LIBRARY CACHE)
    UNSET(OpenCV21_CVCAM_LIBRARY CACHE)
    UNSET(OpenCV21_CVHAARTRAINING_LIBRARY CACHE)
    UNSET(OpenCV21_CXTS_LIBRARY CACHE)
    UNSET(OpenCV21_HIGHGUI_LIBRARY CACHE)
    UNSET(OpenCV21_ML_LIBRARY CACHE)
    UNSET(OpenCV21_TRS_LIBRARY CACHE)
    UNSET(OpenCV20_ROOT_DIR CACHE)
    UNSET(OpenCV20_INCLUDE_DIRS CACHE)
    UNSET(OpenCV20_CV_INCLUDE_DIR CACHE)
    UNSET(OpenCV20_CXCORE_INCLUDE_DIR CACHE)
    UNSET(OpenCV20_CVAUX_INCLUDE_DIR CACHE)
    UNSET(OpenCV20_CVCAM_INCLUDE_DIR CACHE)
    UNSET(OpenCV20_HIGHGUI_INCLUDE_DIR CACHE)
    UNSET(OpenCV20_LIBRARIES CACHE)
    UNSET(OpenCV20_CV_LIBRARY CACHE)
    UNSET(OpenCV20_CXCORE_LIBRARY CACHE)
    UNSET(OpenCV20_CVAUX_LIBRARY CACHE)
    UNSET(OpenCV20_CVCAM_LIBRARY CACHE)
    UNSET(OpenCV20_CVHAARTRAINING_LIBRARY CACHE)
    UNSET(OpenCV20_CXTS_LIBRARY CACHE)
    UNSET(OpenCV20_HIGHGUI_LIBRARY CACHE)
    UNSET(OpenCV20_ML_LIBRARY CACHE)
    UNSET(OpenCV20_TRS_LIBRARY CACHE)

    IF(OpenCV_FOUND)
        MARK_AS_ADVANCED(FORCE ${OpenCV_OPTIONS})

        SET(SVL_ADDITIONAL_INCLUDE_DIRECTORIES
            "${SVL_ADDITIONAL_INCLUDE_DIRECTORIES}"
            "${OpenCV_INCLUDE_DIRS}"
            )
        SET(SVL_ADDITIONAL_LIBRARIES
            "${SVL_ADDITIONAL_LIBRARIES}"
            "${OpenCV_LIBRARIES}"
            )
        SET(SOURCE_FILES
            ${SOURCE_FILES}
            svlFilterImageZoom.cpp
            svlFilterToolTracker.cpp
            svlVideoCodecOpenCV.h       # private header
            svlVideoCodecOpenCV.cpp
            svlTrackerOCVMeanShift.cpp
            )
        IF(NOT WIN32)
            SET(SOURCE_FILES
                ${SOURCE_FILES}
                svlVidCapSrcOpenCV.h        # private header
                svlVidCapSrcOpenCV.cpp
            )
        ENDIF(NOT WIN32)
        SET(HEADER_FILES
            ${HEADER_FILES}
            svlFilterImageZoom.h
            svlFilterToolTracker.h
            svlTrackerOCVMeanShift.h
            )
        MESSAGE(STATUS "OpenCV found - Switch to advanced view for details")
        OPTION(CISST_SVL_HAS_OPENCV "Compile OpenCV extensions" ON)

    ELSE(OpenCV_FOUND)
        IF(CISST_SVL_HAS_OPENCV)
            MARK_AS_ADVANCED(CLEAR ${OpenCV_OPTIONS})
            MESSAGE(FATAL_ERROR "OpenCV required. Please specify its location.")
        ELSE(CISST_SVL_HAS_OPENCV)
            MARK_AS_ADVANCED(FORCE ${OpenCV_OPTIONS})
            OPTION(CISST_SVL_HAS_OPENCV "Compile OpenCV extensions" OFF)
        ENDIF(CISST_SVL_HAS_OPENCV)
    ENDIF(OpenCV_FOUND)

ELSE(NOT OpenCV2_FOUND AND (CISST_SVL_HAS_OPENCV OR NOT OpenCV_OPTIONS))
    MARK_AS_ADVANCED(FORCE ${OpenCV_OPTIONS})
    OPTION(CISST_SVL_HAS_OPENCV "Compile OpenCV extensions" OFF)
ENDIF(NOT OpenCV2_FOUND AND (CISST_SVL_HAS_OPENCV OR NOT OpenCV_OPTIONS))


# JPEG
IF(CISST_SVL_HAS_JPEG OR NOT JPEG_OPTIONS)
    SET(JPEG_OPTIONS
        JPEG_INCLUDE_DIR
        JPEG_LIBRARY
        CACHE INTERNAL "List of JPEG options"
        )

    IF(SVL_GNU_INCLUDE_DIR AND SVL_GNU_LIBRARY_DIR AND NOT JPEG_FOUND)
        FIND_PATH(JPEG_INCLUDE_DIR jpeglib.h "${SVL_GNU_INCLUDE_DIR}")
        FIND_FILE(JPEG_LIBRARY jpeg.lib "${SVL_GNU_LIBRARY_DIR}")
    ENDIF(SVL_GNU_INCLUDE_DIR AND SVL_GNU_LIBRARY_DIR AND NOT JPEG_FOUND)

    FIND_PACKAGE(JPEG)
    IF(JPEG_FOUND)
        MARK_AS_ADVANCED(FORCE ${JPEG_OPTIONS})

        SET(SVL_ADDITIONAL_INCLUDE_DIRECTORIES
            "${SVL_ADDITIONAL_INCLUDE_DIRECTORIES}"
            "${JPEG_INCLUDE_DIR}"
            )
        SET(SVL_ADDITIONAL_LIBRARIES
            "${SVL_ADDITIONAL_LIBRARIES}"
            "${JPEG_LIBRARY}"
            )
        SET(SOURCE_FILES
            ${SOURCE_FILES}
            svlImageCodecJPEG.h           # private header
            svlImageCodecJPEG.cpp
            )
        MESSAGE(STATUS "JPEG found - Switch to advanced view for details")
        OPTION(CISST_SVL_HAS_JPEG "Use JPEG extensions" ON)

        IF(WIN32 AND NOT SVL_GNU_OPTIONS)
            SET(SVL_GNU_OPTIONS
                SVL_GNU_INCLUDE_DIR
                SVL_GNU_LIBRARY_DIR
                CACHE INTERNAL "List of GNU options"
                )
            SET(SVL_GNU_INCLUDE_DIR "${JPEG_INCLUDE_DIR}" CACHE PATH "GNU include directory")
            GET_FILENAME_COMPONENT(SVL_GNU_LIBRARY_DIR "${JPEG_LIBRARY}" PATH CACHE)
            MARK_AS_ADVANCED(FORCE SVL_GNU_INCLUDE_DIR SVL_GNU_LIBRARY_DIR)
        ENDIF(WIN32 AND NOT SVL_GNU_OPTIONS)

    ELSE(JPEG_FOUND)
        IF(CISST_SVL_HAS_JPEG)
            MARK_AS_ADVANCED(CLEAR ${JPEG_OPTIONS})
            MESSAGE(FATAL_ERROR "JPEG required. Please specify its location.")
        ELSE(CISST_SVL_HAS_JPEG)
            MARK_AS_ADVANCED(FORCE ${JPEG_OPTIONS})
            OPTION(CISST_SVL_HAS_JPEG "Compile JPEG extensions" OFF)
        ENDIF(CISST_SVL_HAS_JPEG)
    ENDIF(JPEG_FOUND)

ELSE(CISST_SVL_HAS_JPEG OR NOT JPEG_OPTIONS)
    MARK_AS_ADVANCED(FORCE ${JPEG_OPTIONS})
    OPTION(CISST_SVL_HAS_JPEG "Use JPEG extensions" OFF)
ENDIF(CISST_SVL_HAS_JPEG OR NOT JPEG_OPTIONS)


# ZLIB
IF(CISST_SVL_HAS_ZLIB OR NOT ZLIB_OPTIONS)
    SET(ZLIB_OPTIONS
        ZLIB_INCLUDE_DIR
        ZLIB_LIBRARY
        CACHE INTERNAL "List of ZLIB options"
        )

    IF(SVL_GNU_INCLUDE_DIR AND SVL_GNU_LIBRARY_DIR AND NOT ZLIB_FOUND)
        FIND_PATH(ZLIB_INCLUDE_DIR zlib.h "${SVL_GNU_INCLUDE_DIR}")
        FIND_FILE(ZLIB_LIBRARY zlib.lib "${SVL_GNU_LIBRARY_DIR}")
    ENDIF(SVL_GNU_INCLUDE_DIR AND SVL_GNU_LIBRARY_DIR AND NOT ZLIB_FOUND)

    FIND_PACKAGE(ZLIB)
    IF(ZLIB_FOUND)
        MARK_AS_ADVANCED(FORCE ${ZLIB_OPTIONS})

        SET(SVL_ADDITIONAL_INCLUDE_DIRECTORIES
            "${SVL_ADDITIONAL_INCLUDE_DIRECTORIES}"
            "${ZLIB_INCLUDE_DIR}"
            )
        SET(SVL_ADDITIONAL_LIBRARIES
            "${SVL_ADDITIONAL_LIBRARIES}"
            "${ZLIB_LIBRARY}"
            )
        SET(SOURCE_FILES
            ${SOURCE_FILES}
            svlVideoCodecCVI.h              # private header
            svlVideoCodecCVI.cpp
            svlVideoCodecTCPStream.h        # private header
            svlVideoCodecTCPStream.cpp
            )
        MESSAGE(STATUS "ZLIB found - Switch to advanced view for details")
        OPTION(CISST_SVL_HAS_ZLIB "Use ZLIB extensions" ON)

        IF(WIN32 AND NOT SVL_GNU_OPTIONS)
            SET(SVL_GNU_OPTIONS
                SVL_GNU_INCLUDE_DIR
                SVL_GNU_LIBRARY_DIR
                CACHE INTERNAL "List of GNU options"
                )
            SET(SVL_GNU_INCLUDE_DIR "${ZLIB_INCLUDE_DIR}" CACHE PATH "GNU include directory")
            GET_FILENAME_COMPONENT(SVL_GNU_LIBRARY_DIR "${ZLIB_LIBRARY}" PATH CACHE)
            MARK_AS_ADVANCED(FORCE SVL_GNU_INCLUDE_DIR SVL_GNU_LIBRARY_DIR)
        ENDIF(WIN32 AND NOT SVL_GNU_OPTIONS)

    ELSE(ZLIB_FOUND)
        IF(CISST_SVL_HAS_ZLIB)
            MARK_AS_ADVANCED(CLEAR ${ZLIB_OPTIONS})
            MESSAGE(FATAL_ERROR "ZLIB required. Please specify its location.")
        ELSE(CISST_SVL_HAS_ZLIB)
            MARK_AS_ADVANCED(FORCE ${ZLIB_OPTIONS})
            OPTION(CISST_SVL_HAS_ZLIB "Compile ZLIB extensions" OFF)
        ENDIF(CISST_SVL_HAS_ZLIB)
    ENDIF(ZLIB_FOUND)

ELSE(CISST_SVL_HAS_ZLIB OR NOT ZLIB_OPTIONS)
    MARK_AS_ADVANCED(FORCE ${ZLIB_OPTIONS})
    OPTION(CISST_SVL_HAS_ZLIB "Use ZLIB extensions" OFF)
ENDIF(CISST_SVL_HAS_ZLIB OR NOT ZLIB_OPTIONS)


# PNG
IF(CISST_SVL_HAS_ZLIB AND ZLIB_FOUND)
    MARK_AS_ADVANCED(CLEAR CISST_SVL_HAS_PNG)

    IF(CISST_SVL_HAS_PNG OR NOT PNG_OPTIONS)
        SET(PNG_OPTIONS
            PNG_DEFINITIONS
            PNG_PNG_INCLUDE_DIR
            PNG_INCLUDE_DIR
            PNG_LIBRARY
            PNG_LIBRARIES
            CACHE INTERNAL "List of PNG options"
            )

        IF(SVL_GNU_INCLUDE_DIR AND SVL_GNU_LIBRARY_DIR)
            FIND_PATH(PNG_PNG_INCLUDE_DIR png.h "${SVL_GNU_INCLUDE_DIR}")
            FIND_FILE(PNG_LIBRARY libpng.lib "${SVL_GNU_LIBRARY_DIR}")
        ENDIF(SVL_GNU_INCLUDE_DIR AND SVL_GNU_LIBRARY_DIR)

        FIND_PACKAGE(PNG)
        IF(PNG_FOUND)
            MARK_AS_ADVANCED(FORCE ${PNG_OPTIONS})

            ADD_DEFINITIONS(${PNG_DEFINITIONS})
            SET(SVL_ADDITIONAL_INCLUDE_DIRECTORIES
                "${SVL_ADDITIONAL_INCLUDE_DIRECTORIES}"
                "${PNG_PNG_INCLUDE_DIR}"
                )
            SET(SVL_ADDITIONAL_LIBRARIES
                "${SVL_ADDITIONAL_LIBRARIES}"
                "${PNG_LIBRARY}"
                )
            SET(SOURCE_FILES
                ${SOURCE_FILES}
                svlImageCodecPNG.h            # private header
                svlImageCodecPNG.cpp
                )
            MESSAGE(STATUS "PNG found - Switch to advanced view for details")
            OPTION(CISST_SVL_HAS_PNG "Use PNG extensions" ON)

        ELSE(PNG_FOUND)
            IF(CISST_SVL_HAS_PNG)
                MARK_AS_ADVANCED(CLEAR ${PNG_OPTIONS})
                MESSAGE(FATAL_ERROR "PNG required. Please specify its location.")
            ELSE(CISST_SVL_HAS_PNG)
                MARK_AS_ADVANCED(FORCE ${PNG_OPTIONS})
                OPTION(CISST_SVL_HAS_PNG "Compile PNG extensions" OFF)
            ENDIF(CISST_SVL_HAS_PNG)
        ENDIF(PNG_FOUND)

    ELSE(CISST_SVL_HAS_PNG OR NOT PNG_OPTIONS)
        MARK_AS_ADVANCED(FORCE ${PNG_OPTIONS})
        OPTION(CISST_SVL_HAS_PNG "Use PNG extensions" OFF)
    ENDIF(CISST_SVL_HAS_PNG OR NOT PNG_OPTIONS)

ELSE(CISST_SVL_HAS_ZLIB AND ZLIB_FOUND)
    MARK_AS_ADVANCED(FORCE ${PNG_OPTIONS})
    OPTION(CISST_SVL_HAS_PNG "Use PNG extensions" OFF)
    SET(CISST_SVL_HAS_PNG OFF CACHE BOOL "PNG disabled" FORCE)
    MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_PNG)
ENDIF(CISST_SVL_HAS_ZLIB AND ZLIB_FOUND)


# Matrox Imaging Library
IF(CISST_SVL_HAS_MIL OR NOT MIL_OPTIONS)
    SET(MIL_OPTIONS
        MIL_INCLUDE_DIR
        MIL_mil_LIBRARY
        CACHE INTERNAL "List of MIL options"
        )

    SET(MIL_FOUND "NO")
    FIND_PATH(MIL_INCLUDE_DIR mil.h
              "/opt/matrox_imaging/mil/include"
              "${PROGRAMFILES_DIR}/Matrox Imaging/Mil/Include"
              "${PROGRAMFILES_X86_DIR}/Matrox Imaging/Mil/Include"
              DOC "What is the path where the file mil.h can be found"
              )
    FIND_LIBRARY(MIL_mil_LIBRARY mil
                 "/opt/matrox_imaging/mil/lib"
                 "${PROGRAMFILES_DIR}/Matrox Imaging/Mil/LIB"
                 "${PROGRAMFILES_X86_DIR}/Matrox Imaging/Mil/LIB"
                 DOC "Where can the MIL mil library be found"
                 )
    IF(MIL_INCLUDE_DIR)
        IF(MIL_mil_LIBRARY)
            SET(MIL_FOUND "YES")
        ENDIF(MIL_mil_LIBRARY)
    ENDIF(MIL_INCLUDE_DIR)

    IF(MIL_FOUND)
        MARK_AS_ADVANCED(FORCE ${MIL_OPTIONS})

        SET(SVL_ADDITIONAL_INCLUDE_DIRECTORIES
            "${SVL_ADDITIONAL_INCLUDE_DIRECTORIES}"
            "${MIL_INCLUDE_DIR}"
            )
        SET(SVL_ADDITIONAL_LIBRARIES
            "${SVL_ADDITIONAL_LIBRARIES}"
            "${MIL_mil_LIBRARY}"
            )
        SET(SOURCE_FILES
            ${SOURCE_FILES}
            svlVidCapSrcMIL.h      # private header
            svlVidCapSrcMIL.cpp
            )
        MESSAGE(STATUS "Matrox Imaging Library (MIL) found - Switch to advanced view for details")
        OPTION(CISST_SVL_HAS_MIL "Compile MIL extensions" ON)

    ELSE(MIL_FOUND)
        IF(CISST_SVL_HAS_MIL)
            MARK_AS_ADVANCED(CLEAR ${MIL_OPTIONS})
            MESSAGE(FATAL_ERROR "Matrox Imaging Library (MIL) required. Please specify its location.")
        ELSE(CISST_SVL_HAS_MIL)
            MARK_AS_ADVANCED(FORCE ${MIL_OPTIONS})
            OPTION(CISST_SVL_HAS_MIL "Compile Matrox Imaging Library (MIL) extensions" OFF)
        ENDIF(CISST_SVL_HAS_MIL)
    ENDIF(MIL_FOUND)

ELSE(CISST_SVL_HAS_MIL OR NOT MIL_OPTIONS)
    MARK_AS_ADVANCED(FORCE ${MIL_OPTIONS})
    OPTION(CISST_SVL_HAS_MIL "Compile Matrox Imaging Library (MIL) extensions" OFF)
ENDIF(CISST_SVL_HAS_MIL OR NOT MIL_OPTIONS)


# Windows specific
IF(WIN32)
    # generic windows specific files
    SET(SOURCE_FILES
        ${SOURCE_FILES}
        winWin32.h              # private header
        winWin32.cpp
        svlVideoCodecVfW32.h    # private header
        svlVideoCodecVfW32.cpp
        svlFilterImageWindow.cpp
        svlFilterImageWindowTargetSelect.cpp
        )
    SET(HEADER_FILES
        ${HEADER_FILES}
        svlFilterImageWindow.h
        svlFilterImageWindowTargetSelect.h
        )
    SET(SVL_ADDITIONAL_LIBRARIES
        "${SVL_ADDITIONAL_LIBRARIES}"
        vfw32
        )

    # set windows specific linker flags enabling C++ code compilation on .NET compilers
    SET(SVL_ADDITIONAL_EXE_LINKER_FLAGS
        "${SVL_ADDITIONAL_EXE_LINKER_FLAGS} /NODEFAULTLIB:libc.lib /NODEFAULTLIB:libcmt.lib /NODEFAULTLIB:libcd.lib /NODEFAULTLIB:libcmtd.lib"
        )


    IF(CISST_SVL_HAS_DIRECTSHOW OR NOT DIRECTSHOW_OPTIONS)
        SET(DIRECTSHOW_OPTIONS
            DIRECTSHOW_INCLUDE_DIR
            DIRECTX_INCLUDE_DIR
            DIRECTSHOW_strmiids_LIBRARY
            DIRECTSHOW_WinMM_LIBRARY
            CACHE INTERNAL "List of DirectShow options"
            )

        FIND_PACKAGE(DirectShow)
        IF(DIRECTSHOW_FOUND)
            MARK_AS_ADVANCED(FORCE ${DIRECTSHOW_OPTIONS})

            SET(SVL_ADDITIONAL_INCLUDE_DIRECTORIES
                "${SVL_ADDITIONAL_INCLUDE_DIRECTORIES}"
                "${DIRECTSHOW_INCLUDE_DIR}"
                "${DIRECTX_INCLUDE_DIR}"
                )
            SET(SVL_ADDITIONAL_LIBRARIES
                "${SVL_ADDITIONAL_LIBRARIES}"
                "${DIRECTSHOW_strmiids_LIBRARY}"
                "${DIRECTSHOW_WinMM_LIBRARY}"
                )

            SET(SOURCE_FILES
                ${SOURCE_FILES}
                svlVidCapSrcDirectShowInputSelector.h       # private header
                svlVidCapSrcDirectShowInputSelector.cpp
                svlVidCapSrcDirectShow.h                    # private header
                svlVidCapSrcDirectShow.cpp
                )
            MESSAGE(STATUS "DirectShow found - Switch to advanced view for details")
            OPTION(CISST_SVL_HAS_DIRECTSHOW "Compile DirectShow extensions" ON)

        ELSE(DIRECTSHOW_FOUND)
            IF(CISST_SVL_HAS_DIRECTSHOW)
                MARK_AS_ADVANCED(CLEAR ${DIRECTSHOW_OPTIONS})
                MESSAGE(FATAL_ERROR "DirectShow required. Please specify its location.")
            ELSE(CISST_SVL_HAS_DIRECTSHOW)
                MARK_AS_ADVANCED(FORCE ${DIRECTSHOW_OPTIONS})
                OPTION(CISST_SVL_HAS_DIRECTSHOW "Compile DirectShow extensions" OFF)
            ENDIF(CISST_SVL_HAS_DIRECTSHOW)
        ENDIF(DIRECTSHOW_FOUND)

    ELSE(CISST_SVL_HAS_DIRECTSHOW OR NOT DIRECTSHOW_OPTIONS)
        MARK_AS_ADVANCED(FORCE ${DIRECTSHOW_OPTIONS})
        OPTION(CISST_SVL_HAS_DIRECTSHOW "Compile DirectShow extensions" OFF)
    ENDIF(CISST_SVL_HAS_DIRECTSHOW OR NOT DIRECTSHOW_OPTIONS)


    IF(CISST_SVL_HAS_SVS OR NOT SVS_OPTIONS)
        SET(SVS_OPTIONS
            SVS_INCLUDE_DIR
            SVS_svs_LIBRARY
            SVS_svsgrab_LIBRARY
            CACHE INTERNAL "List of SVS options"
            )

        SET(SVS_FOUND "NO")
        FIND_PATH(SVS_INCLUDE_DIR svs.h
                  "${PROGRAMFILES_DIR}/SRI SVS42/src"
                  "${PROGRAMFILES_DIR}/SRI International/SVS42/src"
                  "${PROGRAMFILES_DIR}/SRI International/SVS41/src"
                  "${PROGRAMFILES_X86_DIR}/SRI SVS42/src"
                  "${PROGRAMFILES_X86_DIR}/SRI International/SVS42/src"
                  "${PROGRAMFILES_X86_DIR}/SRI International/SVS41/src"
                  DOC "What is the path where the file svs.h can be found"
                  )
        FIND_LIBRARY(SVS_svs_LIBRARY svs
                     "${PROGRAMFILES_DIR}/SRI SVS42/bin"
                     "${PROGRAMFILES_DIR}/SRI International/SVS42/bin"
                     "${PROGRAMFILES_DIR}/SRI International/SVS41/bin"
                     "${PROGRAMFILES_X86_DIR}/SRI SVS42/bin"
                     "${PROGRAMFILES_X86_DIR}/SRI International/SVS42/bin"
                     "${PROGRAMFILES_X86_DIR}/SRI International/SVS41/bin"
                     DOC "Where can the SVS svs library be found"
                     )
        FIND_LIBRARY(SVS_svsgrab_LIBRARY svsgrab
                     "${PROGRAMFILES_DIR}/SRI SVS42/bin"
                     "${PROGRAMFILES_DIR}/SRI International/SVS42/bin"
                     "${PROGRAMFILES_DIR}/SRI International/SVS41/bin"
                     "${PROGRAMFILES_X86_DIR}/SRI SVS42/bin"
                     "${PROGRAMFILES_X86_DIR}/SRI International/SVS42/bin"
                     "${PROGRAMFILES_X86_DIR}/SRI International/SVS41/bin"
                     DOC "Where can the SVS svsgrab library be found"
                     )
        IF(SVS_INCLUDE_DIR)
            IF(SVS_svs_LIBRARY)
                IF(SVS_svsgrab_LIBRARY)
                    SET(SVS_FOUND "YES")
                ENDIF(SVS_svsgrab_LIBRARY)
            ENDIF(SVS_svs_LIBRARY)
        ENDIF(SVS_INCLUDE_DIR)

        IF(SVS_FOUND)
            MARK_AS_ADVANCED(FORCE ${SVS_OPTIONS})

            SET(SVL_ADDITIONAL_INCLUDE_DIRECTORIES
                "${SVL_ADDITIONAL_INCLUDE_DIRECTORIES}"
                "${SVS_INCLUDE_DIR}"
                )
            SET(SVL_ADDITIONAL_LIBRARIES
                "${SVL_ADDITIONAL_LIBRARIES}"
                "${SVS_svs_LIBRARY}"
                "${SVS_svsgrab_LIBRARY}"
                )

            SET(SOURCE_FILES
                ${SOURCE_FILES}
                svlVidCapSrcSVS.h      # private header
                svlVidCapSrcSVS.cpp
                )
            MESSAGE(STATUS "Videre Design SVS (SVS) found - Switch to advanced view for details")
            OPTION(CISST_SVL_HAS_SVS "Compile Videre Design (SVS) extensions" ON)

        ELSE(SVS_FOUND)
            IF(CISST_SVL_HAS_SVS)
                MARK_AS_ADVANCED(CLEAR ${SVS_OPTIONS})
                MESSAGE(FATAL_ERROR "Videre Design SVS (SVS) required. Please specify its location.")
            ELSE(CISST_SVL_HAS_SVS)
                MARK_AS_ADVANCED(FORCE ${SVS_OPTIONS})
                OPTION(CISST_SVL_HAS_SVS "Compile Videre Design (SVS) extensions" OFF)
            ENDIF(CISST_SVL_HAS_SVS)
        ENDIF(SVS_FOUND)

    ELSE(CISST_SVL_HAS_SVS OR NOT SVS_OPTIONS)
        MARK_AS_ADVANCED(FORCE ${SVS_OPTIONS})
        OPTION(CISST_SVL_HAS_SVS "Compile Videre Design (SVS) extensions" OFF)
    ENDIF(CISST_SVL_HAS_SVS OR NOT SVS_OPTIONS)

    ADD_DEFINITIONS(-DWIN32_LEAN_AND_MEAN)

ELSE(WIN32)

    #Turn Windows specific switches OFF on other platforms
    SET(CISST_SVL_HAS_DIRECTSHOW "OFF" CACHE INTERNAL "SVL Has DirectShow")
    SET(CISST_SVL_HAS_SVS        "OFF" CACHE INTERNAL "SVL Has SVS")

ENDIF(WIN32)


# UNIX specific
IF(UNIX)
    # Linux specific
    IF(CMAKE_SYSTEM_NAME MATCHES Linux)

        OPTION(CISST_SVL_HAS_VIDEO4LINUX2 "Compile Video4Linux2 extensions" ON)
        IF(CISST_SVL_HAS_VIDEO4LINUX2)
            SET(SOURCE_FILES
                ${SOURCE_FILES}
                svlVidCapSrcV4L2.h           # private header
                svlVidCapSrcV4L2.cpp
                )
        ENDIF(CISST_SVL_HAS_VIDEO4LINUX2)

    ELSE(CMAKE_SYSTEM_NAME MATCHES Linux)

        #Turn Linux specific switches OFF on other platforms
        SET(CISST_SVL_HAS_VIDEO4LINUX2 "OFF" CACHE INTERNAL "SVL Has Video4Linux2")

    ENDIF(CMAKE_SYSTEM_NAME MATCHES Linux)


    # Linux and Darwin
    IF(CISST_SVL_HAS_DC1394 OR NOT DC1394_OPTIONS)
        SET(DC1394_OPTIONS
            DC1394_INCLUDE_DIR
            DC1394_dc1394_LIBRARY
            CACHE INTERNAL "List of DC1394 options"
            )

        SET(DC1394_FOUND "NO")
        FIND_PATH(DC1394_INCLUDE_DIR
                  NAMES
                    dc1394/dc1394.h
                  PATHS
                    "/usr/include"
                    "/opt/local/include"
                  DOC "What is the path where dc1394/dc1394.h can be found"
                  )
        FIND_LIBRARY(DC1394_dc1394_LIBRARY dc1394
                     "/usr/lib"
                     "/opt/local/lib"
                     DOC "Where can the DC1394 library be found"
                     )
        IF(DC1394_INCLUDE_DIR)
            IF(DC1394_dc1394_LIBRARY)
                SET(DC1394_FOUND "YES")
            ENDIF(DC1394_dc1394_LIBRARY)
        ENDIF(DC1394_INCLUDE_DIR)

        IF(DC1394_FOUND)
            MARK_AS_ADVANCED(FORCE ${DC1394_OPTIONS})

            SET(SVL_ADDITIONAL_INCLUDE_DIRECTORIES
                "${SVL_ADDITIONAL_INCLUDE_DIRECTORIES}"
                "${DC1394_INCLUDE_DIR}"
                )
            SET(SVL_ADDITIONAL_LIBRARIES
                "${SVL_ADDITIONAL_LIBRARIES}"
                "${DC1394_dc1394_LIBRARY}"
                )
            SET(SOURCE_FILES
                ${SOURCE_FILES}
                svlVidCapSrcDC1394.h         # private header
                svlVidCapSrcDC1394.cpp
                )
            MESSAGE(STATUS "DC1394 found - Switch to advanced view for details")
            OPTION(CISST_SVL_HAS_DC1394 "Compile DC1394 extensions" ON)

            # Can't auto-detect IEEE1394B on Darwin
            IF(CMAKE_SYSTEM_NAME MATCHES Darwin)
                OPTION(CISST_SVL_DC1394_ENABLE_800Mbps "Enable IEEE1394B (800Mbps)" OFF)
            ELSE(CMAKE_SYSTEM_NAME MATCHES Darwin)
                SET(CISST_SVL_DC1394_ENABLE_800Mbps "ON" CACHE INTERNAL "Enable IEEE1394B (800Mbps)")
            ENDIF(CMAKE_SYSTEM_NAME MATCHES Darwin)

        ELSE(DC1394_FOUND)
            IF(CISST_SVL_HAS_DC1394)
                MARK_AS_ADVANCED(CLEAR ${DC1394_OPTIONS})
                MESSAGE(FATAL_ERROR "DC1394 required. Please specify its location.")
            ELSE(CISST_SVL_HAS_DC1394)
                MARK_AS_ADVANCED(FORCE ${DC1394_OPTIONS})
                OPTION(CISST_SVL_HAS_DC1394 "Compile DC1394 extensions" OFF)
            ENDIF(CISST_SVL_HAS_DC1394)
        ENDIF(DC1394_FOUND)

    ELSE(CISST_SVL_HAS_DC1394 OR NOT DC1394_OPTIONS)
        MARK_AS_ADVANCED(FORCE ${DC1394_OPTIONS})
        OPTION(CISST_SVL_HAS_DC1394 "Compile LibDC1394 extensions" OFF)
    ENDIF(CISST_SVL_HAS_DC1394 OR NOT DC1394_OPTIONS)


    IF(CISST_SVL_HAS_X11 OR NOT DEFINED SVL_X11_FOUND)
        FIND_PACKAGE(X11)
        IF(X11_FOUND)
            SET(SVL_X11_FOUND ${X11_FOUND})

            SET(SVL_ADDITIONAL_INCLUDE_DIRECTORIES
                "${SVL_ADDITIONAL_INCLUDE_DIRECTORIES}"
                "${X11_INCLUDE_DIR}"
                )
            SET(SVL_ADDITIONAL_LIBRARIES
                "${SVL_ADDITIONAL_LIBRARIES}"
                "${X11_LIBRARIES}"
                )
            SET(SOURCE_FILES
                ${SOURCE_FILES}
                winX11.h                 # private header
                winX11.cpp
                svlFilterImageWindow.cpp
                svlFilterImageWindowTargetSelect.cpp
                )
            SET(HEADER_FILES
                ${HEADER_FILES}
                svlFilterImageWindow.h
                svlFilterImageWindowTargetSelect.h
                )
            MESSAGE(STATUS "X11 found - Switch to advanced view for details")
            OPTION(CISST_SVL_HAS_X11 "Compile X11 extensions" ON)

            IF(CMAKE_SYSTEM_NAME MATCHES Linux)
                # Xv video acceleration is not supported on Darwin (as of yet)
                # Search for Xvideo extensions for fast image drawing
                IF(X11_Xv_FOUND AND X11_Xext_FOUND)
                    SET(SVL_ADDITIONAL_LIBRARIES
                        "${SVL_ADDITIONAL_LIBRARIES}"
                        "${X11_Xext_LIB}"
                        "${X11_Xv_LIB}"
                        )
                    MESSAGE(STATUS "X11 libraries Xv and Xext were found")
                    OPTION(CISST_SVL_HAS_XV "Compile Xvideo extensions" ON)
                ELSE(X11_Xv_FOUND AND X11_Xext_FOUND)
                    IF(CISST_SVL_HAS_XV)
                        MESSAGE(FATAL_ERROR "X11 libraries Xv and Xext are required. Please specify their location.")
                    ELSE(CISST_SVL_HAS_XV)
                        OPTION(CISST_SVL_HAS_XV "Compile Xvideo extensions" OFF)
                    ENDIF(CISST_SVL_HAS_XV)
                ENDIF(X11_Xv_FOUND AND X11_Xext_FOUND)
            ELSE(CMAKE_SYSTEM_NAME MATCHES Linux)
                SET(CISST_SVL_HAS_XV "OFF" CACHE INTERNAL "SVL Has Xvideo")
            ENDIF(CMAKE_SYSTEM_NAME MATCHES Linux)

        ELSE(X11_FOUND)
            IF(CISST_SVL_HAS_X11)
                MESSAGE(FATAL_ERROR "X11 required. Please specify its location.")
            ELSE(CISST_SVL_HAS_X11)
                OPTION(CISST_SVL_HAS_X11 "Compile X11 extensions" OFF)
            ENDIF(CISST_SVL_HAS_X11)
        ENDIF(X11_FOUND)

    ELSE(CISST_SVL_HAS_X11 OR NOT DEFINED SVL_X11_FOUND)
        OPTION(CISST_SVL_HAS_X11 "Compile X11 extensions" OFF)
        SET(CISST_SVL_HAS_XV "OFF" CACHE INTERNAL "SVL Has Xvideo")
    ENDIF(CISST_SVL_HAS_X11 OR NOT DEFINED SVL_X11_FOUND)

ELSE(UNIX)

    # Turn Unix specific switches OFF on other platforms
    SET(CISST_SVL_HAS_VIDEO4LINUX2 "OFF" CACHE INTERNAL "SVL Has Video4Linux2")
    SET(CISST_SVL_HAS_DC1394       "OFF" CACHE INTERNAL "SVL Has DC1394")
    SET(CISST_SVL_HAS_X11          "OFF" CACHE INTERNAL "SVL Has X11")
    SET(CISST_SVL_HAS_XV           "OFF" CACHE INTERNAL "SVL Has Xvideo")

ENDIF(UNIX)

# Set SVL options to 'advanced' to hide them from beginner user
#MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_WEBGUI)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_OPENCV)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_OPENCV2)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_JPEG)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_ZLIB)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_PNG)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_MIL)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_DIRECTSHOW)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_SVS)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_VIDEO4LINUX2)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_DC1394)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_X11)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_XV)
IF(CMAKE_SYSTEM_NAME MATCHES Darwin)
    MARK_AS_ADVANCED(FORCE CISST_SVL_DC1394_ENABLE_800Mbps)
ENDIF(CMAKE_SYSTEM_NAME MATCHES Darwin)

# Exporting settings to applications that link against cisst
CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_INCLUDE_DIRECTORIES SVL_ADDITIONAL_INCLUDE_DIRECTORIES)
CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_LINK_DIRECTORIES SVL_ADDITIONAL_LINK_DIRECTORIES)
CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_LIBRARIES SVL_ADDITIONAL_LIBRARIES)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}${SVL_ADDITIONAL_EXE_LINKER_FLAGS}")

SET(SOURCE_FILES
    ${SOURCE_FILES}
    svlInitializer.cpp
    svlVidCapSrcInitializer.h
    svlVidCapSrcInitializer.cpp
    svlImageCodecInitializer.h
    svlImageCodecInitializer.cpp
    svlVideoCodecInitializer.h
    svlVideoCodecInitializer.cpp
    )
SET(HEADER_FILES
    ${HEADER_FILES}
    svlInitializer.h)

# Create the config file
SET(CISST_SVL_CONFIG_FILE ${cisst_BINARY_DIR}/libs/include/cisstStereoVision/svlConfig.h)
CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/include/cisstStereoVision/svlConfig.h.in
               ${CISST_SVL_CONFIG_FILE}
               @ONLY)
INSTALL_FILES(/include/cisstStereoVision
               ".h"
               ${CISST_SVL_CONFIG_FILE})

# Add the config file to the project
SET_SOURCE_FILES_PROPERTIES("${CISST_SVL_CONFIG_FILE}"
                            PROPERTIES GENERATED TRUE)
SET(ADDITIONAL_HEADER_FILES ${CISST_SVL_CONFIG_FILE})

# Finally create the library
cisst_add_library (
  LIBRARY cisstStereoVision
  DEPENDENCIES ${DEPENDENCIES}
  SOURCE_FILES ${SOURCE_FILES}
  HEADER_FILES ${HEADER_FILES})

# Python wrappers
if (CISST_HAS_SWIG_PYTHON)
  cisst_add_swig_module(MODULE cisstStereoVision
                        INTERFACE_DIRECTORY ../../include/cisstStereoVision
			CISST_LIBRARIES cisstStereoVision
                                        ${DEPENDENCIES})
endif (CISST_HAS_SWIG_PYTHON)


# Qt widgets
if (CISST_HAS_OPENGL AND CISST_HAS_QT)

  # to include Qt generated files
  include_directories (${CMAKE_CURRENT_BINARY_DIR})
  qt4_wrap_cpp (QT_WRAPPED_CPP
                ../../include/cisstStereoVision/svlFilterImageQtWidget.h)

  cisst_add_library (
    LIBRARY cisstStereoVisionQt
    LIBRARY_DIR cisstStereoVision
    DEPENDENCIES cisstCommon
                 cisstVector
                 cisstStereoVision
    SOURCE_FILES svlFilterImageQtWidget.cpp
    HEADER_FILES svlFilterImageQtWidget.h
                 svlExportQt.h
    ADDITIONAL_SOURCE_FILES ${QT_WRAPPED_CPP})

  target_link_libraries (cisstStereoVisionQt ${QT_LIBRARIES})

endif (CISST_HAS_OPENGL AND CISST_HAS_QT)
