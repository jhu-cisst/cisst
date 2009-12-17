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
  # Public System API
    svlStreamDefs.cpp
    svlStreamManager.cpp
    svlSyncPoint.cpp
    svlSeries.cpp
    svlRenderTargets.cpp
    svlStreamBranchSource.cpp
    svlSampleQueue.cpp
    svlFileHandlers.cpp
    svlCameraGeometry.cpp
    svlConverters.cpp

  # Filter API
    svlFilterSourceDummy.cpp
    svlFilterVideoDeinterlacer.cpp
    svlFilterSourceVideoCapture.cpp
    svlFilterSourceImageFile.cpp
    svlFilterImageFileWriter.cpp
    svlFilterComputationalStereo.cpp
    svlFilterDisparityMapToSurface.cpp
    svlFilterStereoImageOptimizer.cpp
    svlFilterImageRectifier.cpp
    svlFilterUnsharpMask.cpp
    svlFilterImageSampler.cpp
    svlFilterRGBSwapper.cpp
    svlFilterStreamTypeConverter.cpp
    svlFilterColorSpaceConverter.cpp
    svlFilterImageCropper.cpp
    svlFilterImageResizer.cpp
    svlFilterPointTracker.cpp
    svlFilterStereoImageJoiner.cpp
    svlFilterImageFlipRotate.cpp

  # Private System API
    svlImageBuffer.h               # private header
    svlImageBuffer.cpp
    ftImageBMP.h                   # private header
    ftImageBMP.cpp
    ftImagePPM.h                   # private header
    ftImagePPM.cpp
    ipDeinterlacing.h              # private header
    ipDeinterlacing.cpp
    svlStereoDP.h                  # private header 
    svlStereoDP.cpp
    svlStereoDPMono.h              # private header
    svlStereoDPMono.cpp

  # Trackers
    trackers/msbruteforce/svlTrackerMSBruteForce.cpp
    )

# all header files
SET(HEADER_FILES
  # Public System API
    svlStreamDefs.h
    svlStreamManager.h
    svlSyncPoint.h
    svlSeries.h
    svlRenderTargets.h
    svlStreamBranchSource.h
    svlSampleQueue.h
    svlExport.h
    svlFileHandlers.h
    svlCameraGeometry.h
    svlConverters.h

  # Filter API
    svlFilterSourceDummy.h
    svlFilterVideoDeinterlacer.h
    svlFilterSourceVideoCapture.h
    svlFilterSourceImageFile.h
    svlFilterImageFileWriter.h
    svlFilterComputationalStereo.h
    svlFilterDisparityMapToSurface.h
    svlFilterStereoImageOptimizer.h
    svlFilterImageRectifier.h
    svlFilterUnsharpMask.h
    svlFilterImageSampler.h
    svlFilterRGBSwapper.h
    svlFilterStreamTypeConverter.h
    svlFilterColorSpaceConverter.h
    svlFilterImageCropper.h
    svlFilterImageResizer.h
    svlFilterPointTracker.h
    svlFilterStereoImageJoiner.h
    svlFilterImageFlipRotate.h

  # Trackers
    trackers/svlTrackerMSBruteForce.h
    )


# cisstNumerical dependency
IF(BUILD_LIBS_cisstNumerical AND CISST_HAS_CISSTNETLIB)
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
ENDIF(BUILD_LIBS_cisstNumerical AND CISST_HAS_CISSTNETLIB)


# OpenCV
IF(CISST_SVL_HAS_OPENCV OR NOT OpenCV_OPTIONS)
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
       #OpenCV_CVCAM_INCLUDE_DIR
       #OpenCV_CVCAM_LIBRARY
       #OpenCV_TRS_LIBRARY
        CACHE INTERNAL "List of OpenCV options"
        )

    FIND_PACKAGE(OpenCV COMPONENTS CV CXCORE CVAUX HIGHGUI)
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

ELSE(CISST_SVL_HAS_OPENCV OR NOT OpenCV_OPTIONS)
    MARK_AS_ADVANCED(FORCE ${OpenCV_OPTIONS})
    OPTION(CISST_SVL_HAS_OPENCV "Compile OpenCV extensions" OFF)
ENDIF(CISST_SVL_HAS_OPENCV OR NOT OpenCV_OPTIONS)


# ZLIB
IF(CISST_SVL_HAS_ZLIB OR NOT ZLIB_OPTIONS)
    SET(ZLIB_OPTIONS
        ZLIB_INCLUDE_DIR
        ZLIB_LIBRARY
        CACHE INTERNAL "List of ZLIB options"
        )

    FIND_PACKAGE(ZLIB)
    IF(ZLIB_FOUND)
        MARK_AS_ADVANCED(FORCE ${ZLIB_OPTIONS})

        SET(SVL_ADDITIONAL_INCLUDE_DIRECTORIES
            "${SVL_ADDITIONAL_INCLUDE_DIRECTORIES}"
            "${ZLIB_INCLUDE_DIR}"
            )
        SET(SVL_ADDITIONAL_LIBRARIES
            "${SVL_ADDITIONAL_LIBRARIES}"
            "${ZLIB_LIBRARIES}"
            )

        IF(NOT WIN32)
            SET(SOURCE_FILES
                ${SOURCE_FILES}
                svlFilterSourceVideoFile.cpp
                )
            SET(HEADER_FILES
                ${HEADER_FILES}
                svlFilterSourceVideoFile.h
                )
        ENDIF(NOT WIN32)

        SET(SOURCE_FILES
            ${SOURCE_FILES}
            svlFilterVideoFileWriter.cpp
            )
        SET(HEADER_FILES
            ${HEADER_FILES}
            svlFilterVideoFileWriter.h
            )
        MESSAGE(STATUS "ZLIB found - Switch to advanced view for details")
        OPTION(CISST_SVL_HAS_ZLIB "Use ZLIB extensions" ON)

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
              "C:/Program Files/Matrox Imaging/Mil/Include"
              "D:/Program Files/Matrox Imaging/Mil/Include"
              DOC "What is the path where the file mil.h can be found"
              )
    FIND_LIBRARY(MIL_mil_LIBRARY mil
                 "/opt/matrox_imaging/mil/lib"
                 "C:/Program Files/Matrox Imaging/Mil/LIB"
                 "D:/Program Files/Matrox Imaging/Mil/LIB"
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
            vidMILDevice.h      # private header
            vidMILDevice.cpp
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
        winWin32.h               # private header
        winWin32.cpp
        VfWAvi.h                 # private header
        VfWAvi.cpp
        svlFilterImageWindow.cpp
        svlFilterSourceVideoFile.cpp
        svlFilterVideoFileWriterAVI.cpp
        )
    SET(HEADER_FILES
        ${HEADER_FILES}
        svlFilterImageWindow.h
        svlFilterSourceVideoFile.h
        svlFilterVideoFileWriterAVI.h
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
                vidDirectShowInputSelector.h    # private header
                vidDirectShowInputSelector.cpp
                vidDirectShowSource.h           # private header
                vidDirectShowSource.cpp
                )
            MESSAGE(STATUS "DirectShow found - Switch to advanced view for details")
            OPTION(CISST_SVL_HAS_DIRECTSHOW "Compile DirectShow extensions" ON)

        ELSE(DIRECTSHOW_FOUND)
            IF(CISST_SVL_HAS_DIRECTSHOW)
                MARK_AS_ADVANCED(CLEAR ${DIRECTSHOW_OPTIONS})
                MESSAGE(FATAL_ERROR "DirectShow required. Please specify its location.")
            ELSE(CISST_SVL_HAS_DIRECTSHOW)
                MARK_AS_ADVANCED(FORCE ${DIRECTSHOW_OPTIONS})
                OPTION(CISST_SVL_HAS_MIL "Compile DirectShow extensions" OFF)
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
                  "C:/Program Files/SRI SVS42/src"
                  "C:/Program Files/SRI International/SVS42/src"
                  "C:/Program Files/SRI International/SVS41/src"
                  "D:/Program Files/SRI SVS42/src"
                  "D:/Program Files/SRI International/SVS42/src"
                  "D:/Program Files/SRI International/SVS41/src"
                  DOC "What is the path where the file svs.h can be found"
                  )
        FIND_LIBRARY(SVS_svs_LIBRARY svs
                     "C:/Program Files/SRI SVS42/bin"
                     "C:/Program Files/SRI International/SVS42/bin"
                     "C:/Program Files/SRI International/SVS41/bin"
                     "D:/Program Files/SRI SVS42/bin"
                     "D:/Program Files/SRI International/SVS42/bin"
                     "D:/Program Files/SRI International/SVS41/bin"
                     DOC "Where can the SVS svs library be found"
                     )
        FIND_LIBRARY(SVS_svsgrab_LIBRARY svsgrab
                     "C:/Program Files/SRI SVS42/bin"
                     "C:/Program Files/SRI International/SVS42/bin"
                     "C:/Program Files/SRI International/SVS41/bin"
                     "D:/Program Files/SRI SVS42/bin"
                     "D:/Program Files/SRI International/SVS42/bin"
                     "D:/Program Files/SRI International/SVS41/bin"
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
                vidSVSSource.h      # private header
                vidSVSSource.cpp
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
                vidV4L2Source.h           # private header
                vidV4L2Source.cpp
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
                vidDC1394Source.h         # private header
                vidDC1394Source.cpp
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


    IF(CISST_SVL_HAS_X11 OR NOT DEFINED X11_FOUND)
        FIND_PACKAGE(X11)
        IF(X11_FOUND)
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
                )
            SET(HEADER_FILES
                ${HEADER_FILES}
                svlFilterImageWindow.h
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

    ELSE(CISST_SVL_HAS_X11 OR NOT DEFINED X11_FOUND)
        OPTION(CISST_SVL_HAS_X11 "Compile X11 extensions" OFF)
        SET(CISST_SVL_HAS_XV "OFF" CACHE INTERNAL "SVL Has Xvideo")
    ENDIF(CISST_SVL_HAS_X11 OR NOT DEFINED X11_FOUND)

ELSE(UNIX)

    # Turn Unix specific switches OFF on other platforms
    SET(CISST_SVL_HAS_VIDEO4LINUX2 "OFF" CACHE INTERNAL "SVL Has Video4Linux2")
    SET(CISST_SVL_HAS_DC1394       "OFF" CACHE INTERNAL "SVL Has DC1394")
    SET(CISST_SVL_HAS_X11          "OFF" CACHE INTERNAL "SVL Has X11")
    SET(CISST_SVL_HAS_XV           "OFF" CACHE INTERNAL "SVL Has Xvideo")

ENDIF(UNIX)

# Exporting settings to applications that link against cisst
CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_INCLUDE_DIRECTORIES SVL_ADDITIONAL_INCLUDE_DIRECTORIES)
CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_LINK_DIRECTORIES SVL_ADDITIONAL_LINK_DIRECTORIES)
CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_LIBRARIES SVL_ADDITIONAL_LIBRARIES)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}${SVL_ADDITIONAL_EXE_LINKER_FLAGS}")

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
CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)

