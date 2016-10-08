#
# CMakeLists for cisstStereoVision
#
# (C) Copyright 2003-2016 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (cisstStereoVision)

# create data type using the data generator
cisst_data_generator (cisstStereoVision
                      "${cisst_BINARY_DIR}/include" # where to save the file
                      "cisstStereoVision/"          # sub directory for include
                      ../svlFilterSourceVideoCaptureTypes.cdg
                      ../svlFilterImageOverlayTypes.cdg
                      ../svlFilterSourceImageFileTypes.cdg
                      ../svlFilterSplitterTypes.cdg
                      ../svlFilterSourceTextFileTypes.cdg
)
# to compile cisst generated code, need to find header file
include_directories (${CMAKE_CURRENT_BINARY_DIR})

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
    svlFilterSourceVideoCaptureTypesExtra.cpp
    svlClassServices.cpp
    svlSampleBlobs.cpp
    svlSampleCameraGeometry.cpp
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
    svlFilterAddLatency.cpp
    svlFilterCapFramerate.cpp
    svlFilterComputationalStereo.cpp
    svlFilterDisparityMapToSurface.cpp
    svlFilterFrameTimeSync.cpp
    svlFilterImageBorder.cpp
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
    svlFilterImageWindow.cpp
    svlFilterImageWindowTargetSelect.cpp
    svlFilterLightSourceBuddy.cpp
    svlFilterSampler.cpp
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
    svlWindowManagerBase.cpp
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
    svlSampleCameraGeometry.h
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
    svlFilterAddLatency.h
    svlFilterCapFramerate.h
    svlFilterComputationalStereo.h
    svlFilterDisparityMapToSurface.h
    svlFilterFrameTimeSync.h
    svlFilterImageBorder.h
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
    svlFilterImageWindow.h
    svlFilterImageWindowTargetSelect.h
    svlFilterLightSourceBuddy.h
    svlFilterSampler.h
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
    svlWindowManagerBase.h
    svlRequiredInterfaces.h

    # Trackers
    svlTrackerMSBruteForce.h
    )


# cisstNumerical dependency
IF(CISST_cisstNumerical AND CISST_HAS_CISSTNETLIB)
    SET(CISST_SVL_HAS_CISSTNETLIB "ON" CACHE INTERNAL "SVL cisstNetlib status")
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
ELSE(CISST_cisstNumerical AND CISST_HAS_CISSTNETLIB)
    SET(CISST_SVL_HAS_CISSTNETLIB "OFF" CACHE INTERNAL "SVL cisstNetlib status")
ENDIF(CISST_cisstNumerical AND CISST_HAS_CISSTNETLIB)


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

        cisst_set_package_settings (cisstStereoVision JPEG INCLUDE_DIRECTORIES JPEG_INCLUDE_DIR)
        cisst_set_package_settings (cisstStereoVision JPEG LIBRARIES           JPEG_LIBRARY)
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
        cisst_unset_all_package_settings (cisstStereoVision JPEG)

        IF(CISST_SVL_HAS_JPEG)
            MARK_AS_ADVANCED(CLEAR ${JPEG_OPTIONS})
            MESSAGE(FATAL_ERROR "JPEG required. Please specify its location.")
        ELSE(CISST_SVL_HAS_JPEG)
            MARK_AS_ADVANCED(FORCE ${JPEG_OPTIONS})
            OPTION(CISST_SVL_HAS_JPEG "Compile JPEG extensions" OFF)
        ENDIF(CISST_SVL_HAS_JPEG)
    ENDIF(JPEG_FOUND)

ELSE(CISST_SVL_HAS_JPEG OR NOT JPEG_OPTIONS)
    cisst_unset_all_package_settings (cisstStereoVision JPEG)

    MARK_AS_ADVANCED(FORCE ${JPEG_OPTIONS})
    OPTION(CISST_SVL_HAS_JPEG "Use JPEG extensions" OFF)
ENDIF(CISST_SVL_HAS_JPEG OR NOT JPEG_OPTIONS)


# try to find OpenCV automatically, but do not mark as required
set (OpenCV_COMPONENTS core calib3d contrib features2d gpu
                       legacy highgui ml imgproc objdetect ts video)
find_package (OpenCV 2 QUIET COMPONENTS ${OpenCV_COMPONENTS})

if (OpenCV_FOUND)
    option (CISST_SVL_HAS_OPENCV2 "Compile OpenCV extensions" ON)
    if (CISST_SVL_HAS_OPENCV2)
        set (OpenCV2_PACKAGE_NAME "OpenCV")
        if (${OpenCV_VERSION} VERSION_GREATER 2.1.9)
            message ("OpenCV Version from 2.2.0. onwards.")
            cisst_set_package_settings (cisstStereoVision OpenCV2 INCLUDE_DIRECTORIES OpenCV_INCLUDE_DIRS)
            cisst_set_package_settings (cisstStereoVision OpenCV2 LIBRARIES           OpenCV_LIBS)
            cisst_set_package_settings (cisstStereoVision OpenCV2 PACKAGES            OpenCV2_PACKAGE_NAME)

            # OpenCV 2.0 does not define OpenCV_INCLUDE_DIRS
            # NOTE: OpenCV 2.0. and 2.1. do no specify correct lib path in the OpenCVConfig.cmake files
        elseif (${OpenCV_VERSION} VERSION_LESS 2.2.0 AND
                ${OpenCV_VERSION} VERSION_GREATER 1.9.9)
            message ("OpenCV Version: 2.0.0 and 2.1.0")
            set (OpenCV2_INCLUDE_DIRS "${THIS_OPENCV_CONFIG_PATH}/include"
                 "${THIS_OPENCV_CONFIG_PATH}/include/opencv")
            cisst_set_package_settings (cisstStereoVision OpenCV2 PACKAGES            OpenCV2_PACKAGE_NAME)
            cisst_set_package_settings (cisstStereoVision OpenCV2 INCLUDE_DIRECTORIES OpenCV2_INCLUDE_DIRS)
            cisst_set_package_settings (cisstStereoVision OpenCV2 LIBRARIES           OpenCV_LIBS)

        else (${OpenCV_VERSION} VERSION_GREATER 2.1.9)
            message (FATAL_ERROR "OpenCV Versions earlier than 2.0 are not supported by cisst.")
            cisst_set_package_settings (cisstStereoVision OpenCV2 INCLUDE_DIRECTORIES OpenCV_INCLUDE_DIRS)
            cisst_set_package_settings (cisstStereoVision OpenCV2 LIBRARIES           OpenCV_LIBS)

        endif (${OpenCV_VERSION} VERSION_GREATER 2.1.9)
    else (CISST_SVL_HAS_OPENCV2)
        cisst_unset_all_package_settings (cisstStereoVision OpenCV2)
    endif (CISST_SVL_HAS_OPENCV2)
else (OpenCV_FOUND)
    cisst_unset_all_package_settings (cisstStereoVision OpenCV2)
    option (CISST_SVL_HAS_OPENCV2 "Compile OpenCV extensions" OFF)
endif (OpenCV_FOUND)

if (CISST_SVL_HAS_OPENCV2 AND NOT OpenCV_FOUND)
  find_package (OpenCV 2 QUIET REQUIRED ${OpenCV_COMPONENTS})
endif (CISST_SVL_HAS_OPENCV2 AND NOT OpenCV_FOUND)


# if we require OpenCV2, add these files to compile
if (CISST_SVL_HAS_OPENCV2)
    set (SOURCE_FILES
         ${SOURCE_FILES}
         svlFilterImageZoom.cpp
         svlFilterToolTracker.cpp
         #svlVideoCodecOpenCV.h       # private header
         #svlVideoCodecOpenCV.cpp
         svlTrackerOCVMeanShift.cpp
         svlCCCalibrationGrid.cpp
         svlCCCornerDetector.cpp
         svlCCFileIO.cpp
         svlCCHandEyeCalibration.cpp
         svlCCOriginDetector.cpp
         svlFilterImageCameraCalibrationOpenCV.cpp
         )
    if (NOT WIN32)
        set (SOURCE_FILES
             ${SOURCE_FILES}
             svlVidCapSrcOpenCV.h    # private header
             svlVidCapSrcOpenCV.cpp
             )
    endif (NOT WIN32)
    set (HEADER_FILES
         ${HEADER_FILES}
         svlFilterImageZoom.h
         svlFilterToolTracker.h
         svlTrackerOCVMeanShift.h
         svlCCCalibrationGrid.h
         svlCCCornerDetector.h
         svlCCFileIO.h
         svlCCHandEyeCalibration.h
         svlCCOriginDetector.h
         svlFilterImageCameraCalibrationOpenCV.h
         )
endif (CISST_SVL_HAS_OPENCV2)

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

        cisst_set_package_settings (cisstStereoVision ZLIB INCLUDE_DIRECTORIES ZLIB_INCLUDE_DIR)
        cisst_set_package_settings (cisstStereoVision ZLIB LIBRARIES           ZLIB_LIBRARY)
        SET(SOURCE_FILES
            ${SOURCE_FILES}
            svlVideoCodecCVI.h              # private header
            svlVideoCodecCVI.cpp
            svlVideoCodecTCPStream.h        # private header
            svlVideoCodecTCPStream.cpp
            svlVideoCodecUDPStream.h        # private header
            svlVideoCodecUDPStream.cpp
            )
        MESSAGE(STATUS "ZLIB found - Switch to advanced view for details")
        OPTION(CISST_SVL_HAS_ZLIB "Use ZLIB extensions" ON)

        IF(WIN32 AND NOT SVL_GNU_OPTIONS)
            cisst_unset_all_package_settings (cisstStereoVision ZLIB)

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
        cisst_unset_all_package_settings (cisstStereoVision ZLIB)

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
            cisst_set_package_settings (cisstStereoVision PNG INCLUDE_DIRECTORIES PNG_PNG_INCLUDE_DIR)
            cisst_set_package_settings (cisstStereoVision PNG LIBRARIES           PNG_LIBRARY)
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
        cisst_unset_all_package_settings (cisstStereoVision PNG)

        MARK_AS_ADVANCED(FORCE ${PNG_OPTIONS})
        OPTION(CISST_SVL_HAS_PNG "Use PNG extensions" OFF)
    ENDIF(CISST_SVL_HAS_PNG OR NOT PNG_OPTIONS)

ELSE(CISST_SVL_HAS_ZLIB AND ZLIB_FOUND)
    cisst_unset_all_package_settings (cisstStereoVision PNG)

    MARK_AS_ADVANCED(FORCE ${PNG_OPTIONS})
    OPTION(CISST_SVL_HAS_PNG "Use PNG extensions" OFF)
    SET(CISST_SVL_HAS_PNG OFF CACHE BOOL "PNG disabled" FORCE)
    MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_PNG)
ENDIF(CISST_SVL_HAS_ZLIB AND ZLIB_FOUND)


IF(CISST_SVL_HAS_FFMPEG OR NOT FFMPEG_OPTIONS)
    SET(FFMPEG_OPTIONS
        FFMPEG_avcodec_INCLUDE_DIR
        FFMPEG_avformat_INCLUDE_DIR
        FFMPEG_avutil_INCLUDE_DIR
        FFMPEG_swscale_INCLUDE_DIR
        FFMPEG_avcodec_LIBRARY
        FFMPEG_avformat_LIBRARY
        FFMPEG_avutil_LIBRARY
        FFMPEG_swscale_LIBRARY
        CACHE INTERNAL "List of FFMPEG options"
        )

    SET(FFMPEG_FOUND "NO")
    FIND_PATH(FFMPEG_avcodec_INCLUDE_DIR libavcodec/avcodec.h
              PATHS
                "/usr/include"
                "/usr/include/ffmpeg"
                "/usr/include"
                "/opt/local/include"
                "/opt/local/include/ffmpeg"
                "/opt/local/include"
              DOC "What is the path where avcodec.h can be found"
              )
    FIND_PATH(FFMPEG_avformat_INCLUDE_DIR libavformat/avformat.h
              PATHS
                "/usr/include"
                "/usr/include/ffmpeg"
                "/usr/include"
                "/opt/local/include"
                "/opt/local/include/ffmpeg"
                "/opt/local/include"
              DOC "What is the path where avformat.h can be found"
              )
    FIND_PATH(FFMPEG_avutil_INCLUDE_DIR libavutil/avutil.h
              PATHS
                "/usr/include"
                "/usr/include/ffmpeg"
                "/usr/include"
                "/opt/local/include"
                "/opt/local/include/ffmpeg"
                "/opt/local/include"
              DOC "What is the path where avutil.h can be found"
              )
    FIND_PATH(FFMPEG_swscale_INCLUDE_DIR libswscale/swscale.h
              PATHS
                "/usr/include"
                "/usr/include/ffmpeg"
                "/usr/include"
                "/opt/local/include"
                "/opt/local/include/ffmpeg"
                "/opt/local/include"
              DOC "What is the path where swscale.h can be found"
              )
    FIND_LIBRARY(FFMPEG_avcodec_LIBRARY avcodec
                 "/usr/lib"
                 "/opt/local/lib"
                 DOC "Where can the FFMPEG avcodec library be found"
                 )
    FIND_LIBRARY(FFMPEG_avformat_LIBRARY avformat
                 "/usr/lib"
                 "/opt/local/lib"
                 DOC "Where can the FFMPEG avformat library be found"
                 )
    FIND_LIBRARY(FFMPEG_avutil_LIBRARY avutil
                 "/usr/lib"
                 "/opt/local/lib"
                 DOC "Where can the FFMPEG avutil library be found"
                 )
    FIND_LIBRARY(FFMPEG_swscale_LIBRARY swscale
                 "/usr/lib"
                 "/opt/local/lib"
                 DOC "Where can the FFMPEG swscale library be found"
                 )
    IF(FFMPEG_avcodec_INCLUDE_DIR AND FFMPEG_avformat_INCLUDE_DIR AND FFMPEG_avutil_INCLUDE_DIR AND FFMPEG_swscale_INCLUDE_DIR)
        IF(FFMPEG_avcodec_LIBRARY AND FFMPEG_avformat_LIBRARY AND FFMPEG_avutil_LIBRARY AND FFMPEG_swscale_LIBRARY)
            SET(FFMPEG_FOUND "YES")
        ENDIF(FFMPEG_avcodec_LIBRARY AND FFMPEG_avformat_LIBRARY AND FFMPEG_avutil_LIBRARY AND FFMPEG_swscale_LIBRARY)
    ENDIF(FFMPEG_avcodec_INCLUDE_DIR AND FFMPEG_avformat_INCLUDE_DIR AND FFMPEG_avutil_INCLUDE_DIR AND FFMPEG_swscale_INCLUDE_DIR)

    IF(FFMPEG_FOUND)
        MARK_AS_ADVANCED(FORCE ${FFMPEG_OPTIONS})

        set (CISST_SVL_ALL_FFMPEG_INCLUDE "${FFMPEG_avcodec_INCLUDE_DIR}"
                                          "${FFMPEG_avformat_INCLUDE_DIR}"
                                          "${FFMPEG_avutil_INCLUDE_DIR}"
                                          "${FFMPEG_swscale_INCLUDE_DIR}")
        cisst_set_package_settings (cisstStereoVision FFMPEG INCLUDE_DIRECTORIES CISST_SVL_ALL_FFMPEG_INCLUDE)
        set (CISST_SVL_ALL_FFMPEG_LIBRARIES "${FFMPEG_avcodec_LIBRARY}"
                                            "${FFMPEG_avformat_LIBRARY}"
                                            "${FFMPEG_avutil_LIBRARY}"
                                            "${FFMPEG_swscale_LIBRARY}")
        cisst_set_package_settings (cisstStereoVision FFMPEG LIBRARIES CISST_SVL_ALL_FFMPEG_LIBRARIES)

        SET(SOURCE_FILES
            ${SOURCE_FILES}
            svlVideoCodecFFMPEG.h     # private header
            svlVideoCodecFFMPEG.cpp
            )
        MESSAGE(STATUS "FFMPEG found - Switch to advanced view for details")
        OPTION(CISST_SVL_HAS_FFMPEG "Compile FFMPEG extensions" ON)

    ELSE(FFMPEG_FOUND)
        cisst_unset_all_package_settings (cisstStereoVision FFMPEG)

        IF(CISST_SVL_HAS_FFMPEG)
            MARK_AS_ADVANCED(CLEAR ${FFMPEG_OPTIONS})
            MESSAGE(FATAL_ERROR "FFMPEG required. Please specify its location.")
        ELSE(CISST_SVL_HAS_FFMPEG)
            MARK_AS_ADVANCED(FORCE ${FFMPEG_OPTIONS})
            OPTION(CISST_SVL_HAS_FFMPEG "Compile FFMPEG extensions" OFF)
        ENDIF(CISST_SVL_HAS_FFMPEG)
    ENDIF(FFMPEG_FOUND)

ELSE(CISST_SVL_HAS_FFMPEG OR NOT FFMPEG_OPTIONS)
    cisst_unset_all_package_settings (cisstStereoVision FFMPEG)

    MARK_AS_ADVANCED(FORCE ${FFMPEG_OPTIONS})
    OPTION(CISST_SVL_HAS_FFMPEG "Compile FFMPEG extensions" OFF)
    SET(CISST_SVL_HAS_FFMPEG OFF CACHE BOOL "FFMPEG disabled" FORCE)
    MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_FFMPEG)
ENDIF(CISST_SVL_HAS_FFMPEG OR NOT FFMPEG_OPTIONS)


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

        cisst_set_package_settings (cisstStereoVision MIL INCLUDE_DIRECTORIES MIL_INCLUDE_DIR)
        cisst_set_package_settings (cisstStereoVision MIL LIBRARIES           MIL_mil_LIBRARY)
        SET(SOURCE_FILES
            ${SOURCE_FILES}
            svlVidCapSrcMIL.h      # private header
            svlVidCapSrcMIL.cpp
            )
        MESSAGE(STATUS "Matrox Imaging Library (MIL) found - Switch to advanced view for details")
        OPTION(CISST_SVL_HAS_MIL "Compile MIL extensions" ON)

    ELSE(MIL_FOUND)
        cisst_unset_all_package_settings (cisstStereoVision MIL)

        IF(CISST_SVL_HAS_MIL)
            MARK_AS_ADVANCED(CLEAR ${MIL_OPTIONS})
            MESSAGE(FATAL_ERROR "Matrox Imaging Library (MIL) required. Please specify its location.")
        ELSE(CISST_SVL_HAS_MIL)
            MARK_AS_ADVANCED(FORCE ${MIL_OPTIONS})
            OPTION(CISST_SVL_HAS_MIL "Compile Matrox Imaging Library (MIL) extensions" OFF)
        ENDIF(CISST_SVL_HAS_MIL)
    ENDIF(MIL_FOUND)

ELSE(CISST_SVL_HAS_MIL OR NOT MIL_OPTIONS)
    cisst_unset_all_package_settings (cisstStereoVision MIL)

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
        )
    set (CISST_VFW32_LIBRARY vfw32)
    cisst_set_package_settings (cisstStereoVision VideoForWindows LIBRARIES CISST_VFW32_LIBRARY)

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
            set (CISST_SVL_ALL_DIRECTSHOW_INCLUDE
                 "${DIRECTSHOW_INCLUDE_DIR}"
                 "${DIRECTX_INCLUDE_DIR}")
            cisst_set_package_settings (cisstStereoVision DirectShow INCLUDE_DIRECTORIES CISST_SVL_ALL_DIRECTSHOW_INCLUDE)
            set (CISST_SVL_ALL_DIRECTSHOW_LIBRARIES
                 "${DIRECTSHOW_strmiids_LIBRARY}"
                 "${DIRECTSHOW_WinMM_LIBRARY}")
            cisst_set_package_settings (cisstStereoVision DirectShow LIBRARIES CISST_SVL_ALL_DIRECTSHOW_LIBRARIES)

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
            cisst_unset_all_package_settings (cisstStereoVision DirectShow)

            IF(CISST_SVL_HAS_DIRECTSHOW)
                MARK_AS_ADVANCED(CLEAR ${DIRECTSHOW_OPTIONS})
                MESSAGE(FATAL_ERROR "DirectShow required. Please specify its location.")
            ELSE(CISST_SVL_HAS_DIRECTSHOW)
                MARK_AS_ADVANCED(FORCE ${DIRECTSHOW_OPTIONS})
                OPTION(CISST_SVL_HAS_DIRECTSHOW "Compile DirectShow extensions" OFF)
            ENDIF(CISST_SVL_HAS_DIRECTSHOW)
        ENDIF(DIRECTSHOW_FOUND)

    ELSE(CISST_SVL_HAS_DIRECTSHOW OR NOT DIRECTSHOW_OPTIONS)
        cisst_unset_all_package_settings (cisstStereoVision DirectShow)

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

            cisst_set_package_settings (cisstStereoVision SVS INCLUDE_DIRECTORIES SVS_INCLUDE_DIR)
            set (CISST_SVL_ALL_SVS_LIBRARIES
                 "${SVS_svs_LIBRARY}"
                 "${SVS_svsgrab_LIBRARY}")
            cisst_set_package_settings (cisstStereoVision SVS LIBRARIES CISST_SVL_ALL_SVS_LIBRARIES)

            SET(SOURCE_FILES
                ${SOURCE_FILES}
                svlVidCapSrcSVS.h      # private header
                svlVidCapSrcSVS.cpp
                )
            MESSAGE(STATUS "Videre Design SVS (SVS) found - Switch to advanced view for details")
            OPTION(CISST_SVL_HAS_SVS "Compile Videre Design (SVS) extensions" ON)

        ELSE(SVS_FOUND)
            cisst_unset_all_package_settings (cisstStereoVision SVS)

            IF(CISST_SVL_HAS_SVS)
                MARK_AS_ADVANCED(CLEAR ${SVS_OPTIONS})
                MESSAGE(FATAL_ERROR "Videre Design SVS (SVS) required. Please specify its location.")
            ELSE(CISST_SVL_HAS_SVS)
                MARK_AS_ADVANCED(FORCE ${SVS_OPTIONS})
                OPTION(CISST_SVL_HAS_SVS "Compile Videre Design (SVS) extensions" OFF)
            ENDIF(CISST_SVL_HAS_SVS)
        ENDIF(SVS_FOUND)

    ELSE(CISST_SVL_HAS_SVS OR NOT SVS_OPTIONS)
        cisst_unset_all_package_settings (cisstStereoVision SVS)

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

            cisst_set_package_settings (cisstStereoVision dc1394 INCLUDE_DIRECTORIES DC1394_INCLUDE_DIR)
            cisst_set_package_settings (cisstStereoVision dc1394 LIBRARIES           DC1394_dc1394_LIBRARY)
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
                OPTION(CISST_SVL_DC1394_ENABLE_800Mbps "Enable IEEE1394B (800Mbps)" ON)
            ENDIF(CMAKE_SYSTEM_NAME MATCHES Darwin)

        ELSE(DC1394_FOUND)
            cisst_unset_all_package_settings (cisstStereoVision dc1394)

            IF(CISST_SVL_HAS_DC1394)
                MARK_AS_ADVANCED(CLEAR ${DC1394_OPTIONS})
                MESSAGE(FATAL_ERROR "DC1394 required. Please specify its location.")
            ELSE(CISST_SVL_HAS_DC1394)
                MARK_AS_ADVANCED(FORCE ${DC1394_OPTIONS})
                OPTION(CISST_SVL_HAS_DC1394 "Compile DC1394 extensions" OFF)
            ENDIF(CISST_SVL_HAS_DC1394)
        ENDIF(DC1394_FOUND)

    ELSE(CISST_SVL_HAS_DC1394 OR NOT DC1394_OPTIONS)
        cisst_unset_all_package_settings (cisstStereoVision dc1394)

        MARK_AS_ADVANCED(FORCE ${DC1394_OPTIONS})
        OPTION(CISST_SVL_HAS_DC1394 "Compile LibDC1394 extensions" OFF)
    ENDIF(CISST_SVL_HAS_DC1394 OR NOT DC1394_OPTIONS)


    IF(CISST_SVL_HAS_X11 OR NOT DEFINED SVL_X11_FOUND)
        FIND_PACKAGE(X11)
        IF(X11_FOUND)
            SET(SVL_X11_FOUND ${X11_FOUND} CACHE INTERNAL "SVL X11 found")

            cisst_set_package_settings (cisstStereoVision X11 INCLUDE_DIRECTORIES X11_INCLUDE_DIR)
            cisst_set_package_settings (cisstStereoVision X11 LIBRARIES           X11_LIBRARIES)
            SET(SOURCE_FILES
                ${SOURCE_FILES}
                winX11.h                 # private header
                winX11.cpp
                )
            MESSAGE(STATUS "X11 found - Switch to advanced view for details")
            OPTION(CISST_SVL_HAS_X11 "Compile X11 extensions" ON)

            IF(CMAKE_SYSTEM_NAME MATCHES Linux)
                # Xv video acceleration is not supported on Darwin (as of yet)
                # Search for Xvideo extensions for fast image drawing
                IF(X11_Xv_FOUND AND X11_Xext_FOUND)
                    cisst_set_package_settings (cisstStereoVision Xext LIBRARIES X11_Xext_LIB)
                    cisst_set_package_settings (cisstStereoVision Xv   LIBRARIES X11_Xv_LIB)
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
            cisst_unset_all_package_settings (cisstStereoVision X11)
            cisst_unset_all_package_settings (cisstStereoVision Xext)
            cisst_unset_all_package_settings (cisstStereoVision Xv)

            IF(CISST_SVL_HAS_X11)
                MESSAGE(FATAL_ERROR "X11 required. Please specify its location.")
            ELSE(CISST_SVL_HAS_X11)
                OPTION(CISST_SVL_HAS_X11 "Compile X11 extensions" OFF)
            ENDIF(CISST_SVL_HAS_X11)
        ENDIF(X11_FOUND)

    ELSE(CISST_SVL_HAS_X11 OR NOT DEFINED SVL_X11_FOUND)
        cisst_unset_all_package_settings (cisstStereoVision X11)
        cisst_unset_all_package_settings (cisstStereoVision Xext)
        cisst_unset_all_package_settings (cisstStereoVision Xv)

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

#----------------------------------------------------------
#   Module  : CUDA support for cisstStereoVision library
#   Author  : Min Yang Jung & Balazs Vagvolgyi
#   Created : June 24, 2010
#----------------------------------------------------------

MACRO(SVL_CUDA_ADD_LIBRARY generated_files cuda_target)
    CUDA_ADD_CUDA_INCLUDE_ONCE()
    CUDA_GET_SOURCES_AND_OPTIONS(_sources _cmake_options _options ${ARGN})
    CUDA_BUILD_SHARED_LIBRARY(_cuda_shared_flag ${ARGN})
    CUDA_WRAP_SRCS(${cuda_target} OBJ _generated_files "${_sources}" ${_cmake_options} ${_cuda_shared_flag} OPTIONS ${_options})
    ADD_LIBRARY(${cuda_target} ${_cmake_options} ${_generated_files} ${_sources})
    TARGET_LINK_LIBRARIES(${cuda_target} ${CUDA_LIBRARIES} )
    SET_TARGET_PROPERTIES(${cuda_target} PROPERTIES LINKER_LANGUAGE ${CUDA_C_OR_CXX})
    SET(${generated_files} ${_generated_files})
ENDMACRO(SVL_CUDA_ADD_LIBRARY generated_files cuda_target)

#IF(CISST_SVL_HAS_CUDA OR NOT CUDA_OPTIONS)
IF(CISST_SVL_HAS_CUDA)
#    SET(CUDA_OPTIONS
#        CUDA_SDK_ROOT_DIR
#        CUDA_INCLUDE_DIRS
#        CUDA_LIBRARIES
#        CUDA_ATTACH_VS_BUILD_RULE_TO_CUDA_FILE
#        CUDA_BUILD_CUBIN
#        CUDA_BUILD_EMULATION
#        CUDA_TOOLKIT_ROOT_DIR
#        CUDA_VERBOSE_BUILD
#        CACHE INTERNAL "List of CUDA options"
#        )

    FIND_PACKAGE(CUDA)
    IF(CUDA_FOUND)
        FIND_LIBRARY(CUDA_CUTIL_LIBRARY cutil "${CUDA_SDK_ROOT_DIR}/C/lib")

        CUDA_ADD_LIBRARY(cisstStereoVisionCUDA
                         cuda/svlCUDACommon.cu
#                         ../svlCUDACommon.h
#                         cuda/svlCUDACommon.cpp
                         STATIC
                         )

        cisst_set_package_settings (cisstStereoVisionCUDA CUDA INCLUDE_DIRECTORIES CUDA_INCLUDE_DIRS)
        cisst_set_package_settings (cisstStereoVisionCUDA CUDA LIBRARIES CUDA_CUTIL_LIBRARY)

#        CUDA_INCLUDE_DIRECTORIES("${CUDA_SDK_ROOT_DIR}/common/inc"
#                                 "${CUDA_SDK_ROOT_DIR}/../shared/inc")
#        SET(CUDA_ATTACH_VS_BUILD_RULE_TO_CUDA_FILE ON)
#        SET(CUDA_SOURCES
#            cuda/svlCUDACommon.cu
#            )
#        # Add a special target to clean 'nvcc' generated files.
#        CUDA_BUILD_CLEAN_TARGET()
#        SVL_CUDA_ADD_LIBRARY(CUDA_GENERATED_LIBRARIES
#                             cisstStereoVisionCUDA
#                             "${CUDA_SOURCES}"
#                             STATIC
#                             OPTIONS ""#-DSTUFF=aaa"
#                             RELEASE --use_fast_math -DNDEBUG
#                             DEBUG -g -DDEBUG
#                             )
#
#        MARK_AS_ADVANCED(FORCE ${CUDA_OPTIONS})
#
#        SET(SVL_ADDITIONAL_INCLUDE_DIRECTORIES
#            "${SVL_ADDITIONAL_INCLUDE_DIRECTORIES}"
#            "${CUDA_SDK_ROOT_DIR}/common/inc"
#            "${CUDA_SDK_ROOT_DIR}/../shared/inc"
#            "${CUDA_INCLUDE_DIRS}"
#            )
#        SET(SVL_ADDITIONAL_LIBRARIES
#            "${SVL_ADDITIONAL_LIBRARIES}"
#            "${CUDA_LIBRARIES}"
##            "${CUDA_GENERATED_LIBRARIES}"
#            )

#        SET(SVL_ADDITIONAL_LIBRARIES
#            "${SVL_ADDITIONAL_LIBRARIES}"
#            cisstStereoVisionCUDA
#            )
        SET(HEADER_FILES
            ${HEADER_FILES}
            svlSampleCUDAImageTypes.h
            svlSampleCUDAImageCustom.h
            svlSampleCUDAImage.h
            svlFilterCUDATest.h
#            svlCUDACommon.h
            )
        SET(SOURCE_FILES
            ${SOURCE_FILES}
            svlSampleCUDAImageTypes.cpp
            svlSampleCUDAImage.cpp
            svlFilterCUDATest.cpp
#            cuda/svlCUDACommon.cpp
            )
        MESSAGE(STATUS "CUDA found - Switch to advanced view for details")
        OPTION(CISST_SVL_HAS_CUDA "Compile CUDA extensions" ON)

    ELSE(CUDA_FOUND)
        cisst_unset_all_package_settings (cisstStereoVisionCUDA CUDA)

        IF(CISST_SVL_HAS_CUDA)
#            MARK_AS_ADVANCED(CLEAR ${CUDA_OPTIONS})
            MESSAGE(FATAL_ERROR "CUDA required. Please specify its location.")
        ELSE(CISST_SVL_HAS_CUDA)
#            MARK_AS_ADVANCED(FORCE ${CUDA_OPTIONS})
            OPTION(CISST_SVL_HAS_CUDA "Compile CUDA extensions" OFF)
        ENDIF(CISST_SVL_HAS_CUDA)
    ENDIF(CUDA_FOUND)

#ELSE(CISST_SVL_HAS_CUDA OR NOT CUDA_OPTIONS)
ELSE(CISST_SVL_HAS_CUDA)
    cisst_unset_all_package_settings (cisstStereoVisionCUDA CUDA)
#    MARK_AS_ADVANCED(FORCE ${CUDA_OPTIONS})
    OPTION(CISST_SVL_HAS_CUDA "Compile CUDA extensions" OFF)
#ENDIF(CISST_SVL_HAS_CUDA OR NOT CUDA_OPTIONS)
ENDIF(CISST_SVL_HAS_CUDA)

# Options for BlackMagic DeckLink SDK
option (CISST_SVL_HAS_BM_DECKLINK "Compile BlackMagic(BM) DeckLink extensions" OFF)
if (CISST_SVL_HAS_BM_DECKLINK)
    find_package (DeckLink REQUIRED)
    if (DeckLink_FOUND)
        include (${DeckLink_USE_FILE})
        set (SOURCE_FILES
             ${SOURCE_FILES}
             svlVidCapSrcBMD.h      # private header
             svlVidCapSrcBMD.cpp
             )
        set (SVL_ADDITIONAL_LIBRARIES ${SVL_ADDITIONAL_LIBRARIES} ${DeckLink_LIBRARIES})
        mark_as_advanced (DeckLink_INCLUDE_DIR DeckLink_LIBRARIES)
        cisst_set_package_settings (cisstStereoVision decklink INCLUDE_DIRECTORIES DeckLink_INCLUDE_DIR)
        cisst_set_package_settings (cisstStereoVision decklink LIBRARIES           DeckLink_LIBRARIES)
    else (DeckLink_FOUND)
        cisst_unset_all_package_settings (cisstStereoVision decklink)
    endif (DeckLink_FOUND)
else (CISST_SVL_HAS_BM_DECKLINK)
    cisst_unset_all_package_settings (cisstStereoVision decklink)
endif (CISST_SVL_HAS_BM_DECKLINK)


# Set SVL options to 'advanced' to hide them from beginner user
#MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_WEBGUI)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_JPEG)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_OPENCV)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_OPENCV2)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_ZLIB)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_PNG)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_MIL)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_DIRECTSHOW)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_SVS)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_VIDEO4LINUX2)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_DC1394)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_FFMPEG)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_X11)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_XV)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_CUDA)
MARK_AS_ADVANCED(FORCE CISST_SVL_HAS_BM_DECKLINK)

IF(UNIX)
    MARK_AS_ADVANCED(FORCE CISST_SVL_DC1394_ENABLE_800Mbps)
ENDIF(UNIX)

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}${SVL_ADDITIONAL_EXE_LINKER_FLAGS}")

SET(SOURCE_FILES
    ${SOURCE_FILES}
    svlInitializer.cpp
    svlInitializerMacros.h
    svlVidCapSrcInitializer.h
    svlVidCapSrcInitializer.cpp
    svlImageCodecInitializer.h
    svlImageCodecInitializer.cpp
    svlVideoCodecInitializer.h
    svlVideoCodecInitializer.cpp
    )

set (HEADER_FILES
     ${HEADER_FILES}
     svlInitializer.h)

# Add svlFilterImageOpenGL is OpenGL is used
if (CISST_HAS_OPENGL)
  set (SOURCE_FILES
       ${SOURCE_FILES}
       svlFilterImageOpenGL.cpp
       )
  set (HEADER_FILES
       ${HEADER_FILES}
       svlFilterImageOpenGL.h
       )
endif (CISST_HAS_OPENGL)

# Create the config file
set (CISST_SVL_CONFIG_FILE ${cisst_BINARY_DIR}/include/cisstStereoVision/svlConfig.h)
configure_file (${cisstStereoVisionLibs_SOURCE_DIR}/svlConfig.h.in
                ${CISST_SVL_CONFIG_FILE}
                @ONLY)
install (FILES ${CISST_SVL_CONFIG_FILE}
         DESTINATION include/cisstStereoVision
         COMPONENT cisstStereoVision)

# Add the config file to the project
set_source_files_properties ("${CISST_SVL_CONFIG_FILE}"
                             PROPERTIES GENERATED TRUE)
set (ADDITIONAL_HEADER_FILES ${CISST_SVL_CONFIG_FILE})

# Create the CMake config file
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cisstStereoVisionBuild.cmake.in
                ${CISST_CMAKE_BINARY_DIR}/cisstStereoVisionBuild.cmake
                @ONLY)
install (FILES ${CISST_CMAKE_BINARY_DIR}/cisstStereoVisionBuild.cmake
         DESTINATION ${CISST_CMAKE_INSTALL_SUFFIX}
         COMPONENT cisstStereoVision)

# Finally create the library
cisst_add_library (
  LIBRARY cisstStereoVision
  FOLDER cisstStereoVision
  DEPENDENCIES ${DEPENDENCIES}
  SOURCE_FILES ${SOURCE_FILES}
  HEADER_FILES ${HEADER_FILES}
  ADDITIONAL_SOURCE_FILES ${cisstStereoVision_CISST_DG_SRCS}
  ADDITIONAL_HEADER_FILES ${ADDITIONAL_HEADER_FILES} ${cisstStereoVision_CISST_DG_HDRS})

install (FILES ${cisstStereoVision_CISST_DG_HDRS_FULLPATH}
         DESTINATION "include/cisstStereoVision")

# Link `cisstStereoVision` against CUDA generated libraries
IF(CISST_SVL_HAS_CUDA)
#    TARGET_LINK_LIBRARIES(cisstStereoVision
#                          ${CUDA_CUTIL_LIBRARY}
#                          ${CUDA_GENERATED_LIBRARIES}
#                          )
ENDIF(CISST_SVL_HAS_CUDA)
