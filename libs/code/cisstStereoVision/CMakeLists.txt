#
# $Id: $
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
    cisstImage
    cisstNumerical)

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
    svlVideoFileSource.cpp
    svlVideoFileWriter.cpp
    svlImageFileSource.cpp
    svlImageFileWriter.cpp
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
    trackers/cisst/svlTrackerCisst.cpp
    trackers/cisst/trkCisstWSSD.h      # private header
    trackers/cisst/trkCisstWSSD.cpp
    trackers/cisst/trkCisstNCC.h       # private header
    trackers/cisst/trkCisstNCC.cpp
    trackers/cisst/trkCisstBase.h      # private header
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
    svlVideoFileSource.h
    svlVideoFileWriter.h
    svlImageFileSource.h
    svlImageFileWriter.h
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
    trackers/svlTrackerCisst.h
    trackers/svlTrackerMSBruteForce.h
    )


# OpenCV
OPTION(CISST_SVL_HAS_OPENCV "Compile OpenCV extensions" OFF)
IF(CISST_SVL_HAS_OPENCV)
  FIND_PACKAGE(OpenCV)
  INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
  LINK_DIRECTORIES(${OpenCV_LINK_DIRECTORIES})
  LINK_LIBRARIES(${OpenCV_LIBRARIES})

  SET(SOURCE_FILES
      ${SOURCE_FILES}
      vidOCVSource.h        # private header
      vidOCVSource.cpp
      # Trackers
      trackers/opencv/svlTrackerOpenCV.cpp
      trackers/opencv/trkOCVColor.h        # private header
      trackers/opencv/trkOCVColor.cpp
      )
  SET(HEADER_FILES
      ${HEADER_FILES}
      # Trackers
      trackers/svlTrackerOpenCV.h
      )
ENDIF(CISST_SVL_HAS_OPENCV)


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
    )
  SET(HEADER_FILES
    ${HEADER_FILES}
    svlImageWindow.h
    )

  OPTION(CISST_SVL_HAS_DIRECTSHOW "Compile DirectShow extensions" OFF)
  IF(CISST_SVL_HAS_DIRECTSHOW)

    # Find Direct Show
    FIND_PACKAGE(DirectShow)
    INCLUDE_DIRECTORIES(${DIRECTSHOW_INCLUDE_DIR} ${DIRECTX_INCLUDE_DIR})

    # Find samples and compiled samples
    SET(CISST_SVL_DS_SDK_DIR "C:/Program Files/Microsoft Platform SDK for Windows Server 2003 R2/Samples")
    FIND_PATH(CISST_SVL_DS_CROSSBAR_PATH 
              NAMES crossbar.h
              PATHS "${CISST_SVL_DS_SDK_DIR}/Multimedia/DirectShow/Capture/AMCap")
    FIND_FILE(CISST_SVL_DS_CROSSBAR_OBJ
              NAMES crossbar.obj
              PATHS "${CISST_SVL_DS_SDK_DIR}/Multimedia/DirectShow/Capture/AMCap/XP32_RETAIL")
    CONFIGURE_FILE(${CISST_SVL_DS_CROSSBAR_OBJ}
                   ${CMAKE_CURRENT_SOURCE_DIR}/crossbar-copy.obj COPYONLY)
    FIND_PATH(CISST_SVL_DS_BASECLASSES_PATH 
              NAMES streams.h
              PATHS "${CISST_SVL_DS_SDK_DIR}/Multimedia/DirectShow/BaseClasses")
    FIND_LIBRARY(CISST_SVL_DS_strmbase_LIB
                 NAMES strmbase
                 PATHS "${CISST_SVL_DS_SDK_DIR}/Multimedia/DirectShow/BaseClasses/XP32_RETAIL")

    INCLUDE_DIRECTORIES(${CISST_SVL_DS_CROSSBAR_PATH} ${CISST_SVL_DS_BASECLASSES_PATH})

    SET(SOURCE_FILES
        ${SOURCE_FILES}
	    crossbar-copy.obj       # hack, file copied from SDK sample directory as CMake doesn't allow absolute paths
        vidDirectShowSource.h   # private header
        vidDirectShowSource.cpp
        )

  ENDIF(CISST_SVL_HAS_DIRECTSHOW)

  OPTION(CISST_SVL_HAS_SVS "Compile Videre Design SVS extensions" OFF)
  IF(CISST_SVL_HAS_SVS)

    SET(CISST_SVL_SVS_DIR "C:/Program Files/SRI International/SVS41")
    FIND_PATH(CISST_SVL_SVS_INCLUDE
              NAMES svs.h
              PATHS "${CISST_SVL_SVS_DIR}/src")
    INCLUDE_DIRECTORIES(${CISST_SVL_SVS_INCLUDE})
    FIND_LIBRARY(CISST_SVL_svsgrab_LIBRARY
                 NAMES svsgrab
                 PATHS "${CISST_SVL_SVS_DIR}/bin")
    FIND_LIBRARY(CISST_SVL_svs_LIBRARY
                 NAMES svs
                 PATHS "${CISST_SVL_SVS_DIR}/bin")
    SET(SOURCE_FILES
        ${SOURCE_FILES}
        vidSVSSource.cpp
        vidSVSSource.h          # private header
        )

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
    INCLUDE_DIRECTORIES(X11_INCLUDE_DIR)
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
# SET_SOURCE_FILES_PROPERTIES("${CISST_SVL_CONFIG_FILE}"
#                             PROPERTIES GENERATED TRUE)
# SET(HEADER_FILES ${HEADER_FILES} ${CISST_SVL_CONFIG_FILE})

# Finally create the library
CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)

# Add libraries to link with based on options
IF(WIN32)
  IF(CISST_SVL_HAS_DIRECTSHOW)
    TARGET_LINK_LIBRARIES(cisstStereoVision
                          ${DIRECTSHOW_strmiids_LIBRARY}
                          ${DIRECTSHOW_Vfw32_LIBRARY}
                          ${DIRECTSHOW_WinMM_LIBRARY}
                          ${CISST_SVL_DS_strmbase_LIB}
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


IF(CISST_SVL_HAS_X11)
  TARGET_LINK_LIBRARIES(cisstStereoVision ${X11_LIBRARIES})
ENDIF(CISST_SVL_HAS_X11)

