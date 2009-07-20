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

# set the library name
SET(LIBRARY cisstStereoVision)

# set dependencies
SET(DEPENDENCIES
    cisstCommon
    cisstOSAbstraction
    cisstVector
    )

# all source files
SET(SOURCE_FILES
    svlStreamManager.cpp
    svlSyncPoint.cpp
    svlImageBuffer.h               # private header
    svlImageBuffer.cpp
    svlSeries.cpp
    svlGenericFilter.cpp
    svlFileHandlers.cpp
    ftImageBMP.h                   # private header
    ftImageBMP.cpp
    ftImagePPM.h                   # private header
    ftImagePPM.cpp
    svlVideoCaptureSource.cpp
    svlImageFileSource.cpp
    svlImageFileWriter.cpp
    svlRenderTargets.cpp
    svlDummySource.cpp
    svlDeinterlacer.cpp
    ipDeinterlacing.h              # private header
    ipDeinterlacing.cpp
    svlConverters.h                # private header
    svlConverters.cpp
    svlComputationalStereo.cpp
    svlStereoDP.h                  # private header 
    svlStereoDP.cpp
    svlStereoDPMono.h              # private header
    svlStereoDPMono.cpp
    svlStereoColorMatching.cpp
    svlImageRectifier.cpp
    svlUnsharpMask.cpp
    svlImageSampler.cpp
    svlRGBSwapper.cpp
    svlStreamTypeConverter.cpp
    svlColorSpaceConverter.cpp
    svlImageCropper.cpp
    svlStreamBranchSource.cpp
    svlImageResizer.cpp
    svlPointTracker.cpp
    svlStereoImageJoiner.cpp
    svlImageFlipRotate.cpp
    # Trackers
    trackers/msbruteforce/svlTrackerMSBruteForce.cpp
    )

# all header files
SET(HEADER_FILES
    svlSyncPoint.h
    svlSeries.h
    svlExport.h
    svlStreamManager.h
    svlStreamDefs.h
    svlGenericFilter.h
    svlFileHandlers.h
    svlVideoCaptureSource.h
    svlImageFileSource.h
    svlImageFileWriter.h
    svlRenderTargets.h
    svlDummySource.h
    svlDeinterlacer.h
    svlComputationalStereo.h
    svlStereoColorMatching.h
    svlImageRectifier.h
    svlUnsharpMask.h
    svlImageSampler.h
    svlRGBSwapper.h
    svlStreamTypeConverter.h
    svlColorSpaceConverter.h
    svlImageCropper.h
    svlStreamBranchSource.h
    svlImageResizer.h
    svlPointTracker.h
    svlStereoImageJoiner.h
    svlImageFlipRotate.h
# Trackers
    trackers/svlTrackerMSBruteForce.h
    )


# cisstNumerical dependency
IF(BUILD_LIBS_cisstNumerical)
  SET(DEPENDENCIES
      ${DEPENDENCIES}
      cisstNumerical
      )
  SET(SOURCE_FILES
      ${SOURCE_FILES}
      # Trackers
      trackers/cisst/svlTrackerCisst.cpp
      trackers/cisst/trkCisstWSSD.h      # private header
      trackers/cisst/trkCisstWSSD.cpp
      trackers/cisst/trkCisstNCC.h       # private header
      trackers/cisst/trkCisstNCC.cpp
      trackers/cisst/trkCisstBase.h      # private header
      )
  SET(HEADER_FILES
      ${HEADER_FILES}
      # Trackers
      trackers/svlTrackerCisst.h
      )
ENDIF(BUILD_LIBS_cisstNumerical)


# OpenCV
OPTION(CISST_SVL_HAS_OPENCV "Compile OpenCV extensions" OFF)
IF(CISST_SVL_HAS_OPENCV)
    FIND_PACKAGE(OpenCV COMPONENTS CV CXCORE CVAUX HIGHGUI)
    IF(OpenCV_FOUND)
        MARK_AS_ADVANCED(FORCE
                         OpenCV_ROOT_DIR
                         OpenCV_INCLUDE_DIRS
                         OpenCV_CV_INCLUDE_DIR
                         OpenCV_CXCORE_INCLUDE_DIR
                         OpenCV_CVAUX_INCLUDE_DIR
                        #OpenCV_CVCAM_INCLUDE_DIR
                         OpenCV_HIGHGUI_INCLUDE_DIR
                         OpenCV_LIBRARIES
                         OpenCV_CV_LIBRARY
                         OpenCV_CXCORE_LIBRARY
                         OpenCV_CVAUX_LIBRARY
                        #OpenCV_CVCAM_LIBRARY
                         OpenCV_CVHAARTRAINING_LIBRARY
                         OpenCV_CXTS_LIBRARY
                         OpenCV_HIGHGUI_LIBRARY
                         OpenCV_ML_LIBRARY
                        #OpenCV_TRS_LIBRARY
                         )

        INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
        SET(SOURCE_FILES
            ${SOURCE_FILES}
            svlFilterToolTracker.cpp
            vidOCVSource.h        # private header
            vidOCVSource.cpp
            # Trackers
            trackers/opencv/svlTrackerOpenCV.cpp
            trackers/opencv/trkOCVColor.h        # private header
            trackers/opencv/trkOCVColor.cpp
            )
        SET(HEADER_FILES
            ${HEADER_FILES}
            svlFilterToolTracker.h
            # Trackers
            trackers/svlTrackerOpenCV.h
            )
    ELSE(OpenCV_FOUND)
        MARK_AS_ADVANCED(CLEAR
                         OpenCV_ROOT_DIR
                         OpenCV_INCLUDE_DIRS
                         OpenCV_CV_INCLUDE_DIR
                         OpenCV_CXCORE_INCLUDE_DIR
                         OpenCV_CVAUX_INCLUDE_DIR
                        #OpenCV_CVCAM_INCLUDE_DIR
                         OpenCV_HIGHGUI_INCLUDE_DIR
                         OpenCV_LIBRARIES
                         OpenCV_CV_LIBRARY
                         OpenCV_CXCORE_LIBRARY
                         OpenCV_CVAUX_LIBRARY
                        #OpenCV_CVCAM_LIBRARY
                         OpenCV_CVHAARTRAINING_LIBRARY
                         OpenCV_CXTS_LIBRARY
                         OpenCV_HIGHGUI_LIBRARY
                         OpenCV_ML_LIBRARY
                        #OpenCV_TRS_LIBRARY
                         )
    ENDIF(OpenCV_FOUND)
ENDIF(CISST_SVL_HAS_OPENCV)


OPTION(CISST_SVL_HAS_ZLIB "Use ZLIB extensions" OFF)
IF(CISST_SVL_HAS_ZLIB)
    FIND_PACKAGE(ZLIB REQUIRED)
    IF(ZLIB_FOUND)
        MARK_AS_ADVANCED(FORCE
                         ZLIB_INCLUDE_DIR
                         ZLIB_LIBRARY
                         )

        INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})

        IF(NOT WIN32)
            SET(SOURCE_FILES
                ${SOURCE_FILES}
                svlVideoFileSource.cpp
                )
            SET(HEADER_FILES
                ${HEADER_FILES}
                svlVideoFileSource.h
                )
        ENDIF(NOT WIN32)

        SET(SOURCE_FILES
            ${SOURCE_FILES}
            svlVideoFileWriter.cpp
            )
        SET(HEADER_FILES
            ${HEADER_FILES}
            svlVideoFileWriter.h
            )
    ELSE(ZLIB_FOUND)
        MARK_AS_ADVANCED(CLEAR
                         ZLIB_INCLUDE_DIR
                         ZLIB_LIBRARY
                         )
    ENDIF(ZLIB_FOUND)
ENDIF(CISST_SVL_HAS_ZLIB)


# TO DO: add windows support
IF(UNIX)

# Matrox Imaging Library
OPTION(CISST_SVL_HAS_MIL "Compile MIL extensions" OFF)
IF(CISST_SVL_HAS_MIL)

    SET(MIL_FOUND "NO")
    FIND_PATH(MIL_INCLUDE_DIR mil.h
              "/opt/matrox_imaging/mil/include"
              DOC "What is the path where the file mil.h can be found"
              )
    FIND_LIBRARY(MIL_mil_LIBRARY mil
                 "/opt/matrox_imaging/mil/lib"
                 DOC "Where can the MIL mil library be found"
                 )
    IF(MIL_INCLUDE_DIR)
        IF(MIL_mil_LIBRARY)
            SET(MIL_FOUND "YES")
        ENDIF(MIL_mil_LIBRARY)
    ENDIF(MIL_INCLUDE_DIR)

    IF(MIL_FOUND)
        MARK_AS_ADVANCED(FORCE
                         MIL_INCLUDE_DIR
                         MIL_mil_LIBRARY
                         )

        INCLUDE_DIRECTORIES(${MIL_INCLUDE_DIR})
        SET(SOURCE_FILES
            ${SOURCE_FILES}
            vidMILDevice.h      # private header
            vidMILDevice.cpp
            )
    ELSE(MIL_FOUND)
        MARK_AS_ADVANCED(CLEAR
                         MIL_INCLUDE_DIR
                         MIL_mil_LIBRARY
                         )
    ENDIF(MIL_FOUND)
ENDIF(CISST_SVL_HAS_MIL)

ELSE(UNIX)
  SET(CISST_SVL_HAS_MIL "OFF" CACHE INTERNAL "SVL Has MIL")
ENDIF(UNIX)


# Windows specific
IF(WIN32)
  # generic windows specific files
  SET(SOURCE_FILES
    ${SOURCE_FILES}
    winWin32.h               # private header
    winWin32.cpp
    VfWAvi.h                 # private header
    VfWAvi.cpp
    svlImageWindow.cpp
    svlVideoFileSource.cpp
    svlVideoFileWriterAVI.cpp
    )
  SET(HEADER_FILES
    ${HEADER_FILES}
    svlImageWindow.h
    svlVideoFileSource.h
    svlVideoFileWriterAVI.h
    )

  OPTION(CISST_SVL_HAS_DIRECTSHOW "Compile DirectShow extensions" OFF)
  IF(CISST_SVL_HAS_DIRECTSHOW)
    FIND_PACKAGE(DirectShow REQUIRED)
    IF(DIRECTSHOW_FOUND)
        MARK_AS_ADVANCED(FORCE
                         DIRECTSHOW_INCLUDE_DIR
                         DIRECTX_INCLUDE_DIR
                         DIRECTSHOW_strmiids_LIBRARY
                         DIRECTSHOW_quartz_LIBRARY
                         DIRECTSHOW_Vfw32_LIBRARY
                         DIRECTSHOW_WinMM_LIBRARY 
                         )

        INCLUDE_DIRECTORIES(${DIRECTSHOW_INCLUDE_DIR} ${DIRECTX_INCLUDE_DIR})
        SET(SOURCE_FILES
            ${SOURCE_FILES}
            vidDirectShowInputSelector.h    # private header
            vidDirectShowInputSelector.cpp
            vidDirectShowSource.h           # private header
            vidDirectShowSource.cpp
            )
    ELSE(DIRECTSHOW_FOUND)
        MARK_AS_ADVANCED(CLEAR
                         DIRECTSHOW_INCLUDE_DIR
                         DIRECTX_INCLUDE_DIR
                         DIRECTSHOW_strmiids_LIBRARY
                         DIRECTSHOW_quartz_LIBRARY
                         DIRECTSHOW_Vfw32_LIBRARY
                         DIRECTSHOW_WinMM_LIBRARY 
                         )
    ENDIF(DIRECTSHOW_FOUND)
  ENDIF(CISST_SVL_HAS_DIRECTSHOW)

  OPTION(CISST_SVL_HAS_SVS "Compile Videre Design SVS extensions" OFF)
  IF(CISST_SVL_HAS_SVS)

    SET(SVS_FOUND "NO")
    FIND_PATH(SVS_INCLUDE_DIR svs.h
              "C:/Program Files/SRI International/SVS42/src"
              "C:/Program Files/SRI International/SVS41/src"
              DOC "What is the path where the file svs.h can be found"
              )
    FIND_LIBRARY(SVS_svs_LIBRARY svs
                 "C:/Program Files/SRI International/SVS42/bin"
                 "C:/Program Files/SRI International/SVS41/bin"
                 DOC "Where can the SVS svs library be found"
                 )
    FIND_LIBRARY(SVS_svsgrab_LIBRARY svsgrab
                 "C:/Program Files/SRI International/SVS42/bin"
                 "C:/Program Files/SRI International/SVS41/bin"
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
        MARK_AS_ADVANCED(FORCE
                         SVS_INCLUDE_DIR
                         SVS_svs_LIBRARY
                         SVS_svsgrab_LIBRARY
                         )

        INCLUDE_DIRECTORIES(${SVS_INCLUDE_DIR})
        SET(SOURCE_FILES
            ${SOURCE_FILES}
            vidSVSSource.h      # private header
            vidSVSSource.cpp
            )
    ELSE(SVS_FOUND)
        MARK_AS_ADVANCED(CLEAR
                         SVS_INCLUDE_DIR
                         SVS_svs_LIBRARY
                         SVS_svsgrab_LIBRARY
                         )
    ENDIF(SVS_FOUND)
  ENDIF(CISST_SVL_HAS_SVS)

  ADD_DEFINITIONS(-DWIN32_LEAN_AND_MEAN)

ELSE(WIN32)

  #Turn Windows specific switches OFF on other platforms
  SET(CISST_SVL_HAS_DIRECTSHOW "OFF" CACHE INTERNAL "SVL Has DirectShow")
  SET(CISST_SVL_HAS_SVS        "OFF" CACHE INTERNAL "SVL Has SVS")

ENDIF(WIN32)


# UNIX specific
IF(UNIX)
  #Linux specific
  IF(CMAKE_SYSTEM_NAME MATCHES Linux)
    OPTION(CISST_SVL_HAS_VIDEO4LINUX2 "Compile Video4Linux2 extensions" OFF)
    IF(CISST_SVL_HAS_VIDEO4LINUX2)
      SET(SOURCE_FILES
          ${SOURCE_FILES}
          vidV4L2Source.h           # private header
          vidV4L2Source.cpp
          )
    ENDIF(CISST_SVL_HAS_VIDEO4LINUX2)

    OPTION(CISST_SVL_HAS_LIBDC1394 "Compile LibDC1394 extensions" OFF)
    IF(CISST_SVL_HAS_LIBDC1394)
      SET(SOURCE_FILES
          ${SOURCE_FILES}
          vidDC1394Source.h         # private header
          vidDC1394Source.cpp
          )
    ENDIF(CISST_SVL_HAS_LIBDC1394)
  ELSE(CMAKE_SYSTEM_NAME MATCHES Linux)
    #Turn Linux specific switches OFF on other platforms
    SET(CISST_SVL_HAS_VIDEO4LINUX2 "OFF" CACHE INTERNAL "SVL Has Video4Linux2")
    SET(CISST_SVL_HAS_LIBDC1394    "OFF" CACHE INTERNAL "SVL Has LibDC1394")
  ENDIF(CMAKE_SYSTEM_NAME MATCHES Linux)
  # Linux and Darwin
  OPTION(CISST_SVL_HAS_X11 "Compile X11 extensions" OFF)
  IF(CISST_SVL_HAS_X11)
    FIND_PACKAGE(X11 REQUIRED)
    INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR})
    SET(SOURCE_FILES
        ${SOURCE_FILES}
        winX11.h                 # private header
        winX11.cpp
        svlImageWindow.cpp
        )
    SET(HEADER_FILES
        ${HEADER_FILES}
        svlImageWindow.h
        )
  ENDIF(CISST_SVL_HAS_X11)
ELSE(UNIX)

  # Turn Unix specific switches OFF on other platforms
  SET(CISST_SVL_HAS_VIDEO4LINUX2 "OFF" CACHE INTERNAL "SVL Has Video4Linux2")
  SET(CISST_SVL_HAS_LIBDC1394    "OFF" CACHE INTERNAL "SVL Has LibDC1394")
  SET(CISST_SVL_HAS_X11          "OFF" CACHE INTERNAL "SVL Has X11")

ENDIF(UNIX)

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
SET(HEADERS ${CISST_SVL_CONFIG_FILE})

# Create the CMake config file
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/cisstStereoVisionBuild.cmake.in
               ${cisst_BINARY_DIR}/cisstStereoVisionBuild.cmake
               @ONLY)

# Finally create the library
CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)

# Add libraries to link with based on options
IF(WIN32)
  IF(CISST_SVL_HAS_DIRECTSHOW)
    TARGET_LINK_LIBRARIES(cisstStereoVision
                          ${DIRECTSHOW_strmiids_LIBRARY}
                          ${DIRECTSHOW_Vfw32_LIBRARY}
                          ${DIRECTSHOW_WinMM_LIBRARY}
                          )
    SET(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:libc.lib /NODEFAULTLIB:libcmt.lib /NODEFAULTLIB:libcd.lib /NODEFAULTLIB:libcmtd.lib")
  ENDIF(CISST_SVL_HAS_DIRECTSHOW)

  IF(CISST_SVL_HAS_SVS)
    TARGET_LINK_LIBRARIES(cisstStereoVision
                          ${CISST_SVL_svs_LIBRARY}
                          ${CISST_SVL_svsgrab_LIBRARY}
                          )

  ENDIF(CISST_SVL_HAS_SVS)
ENDIF(WIN32)

IF(CISST_SVL_HAS_OPENCV)
  TARGET_LINK_LIBRARIES(cisstStereoVision ${OpenCV_LIBRARIES})
ENDIF(CISST_SVL_HAS_OPENCV)

IF(CISST_SVL_HAS_ZLIB)
    IF(ZLIB_FOUND)
        TARGET_LINK_LIBRARIES(cisstStereoVision ${ZLIB_LIBRARIES})
    ENDIF(ZLIB_FOUND)
ENDIF(CISST_SVL_HAS_ZLIB)

IF(CISST_SVL_HAS_MIL)
  TARGET_LINK_LIBRARIES(cisstStereoVision ${CISST_SVL_MIL_LIBRARY})
ENDIF(CISST_SVL_HAS_MIL)

IF(CISST_SVL_HAS_X11)
  TARGET_LINK_LIBRARIES(cisstStereoVision ${X11_LIBRARIES})
ENDIF(CISST_SVL_HAS_X11)

