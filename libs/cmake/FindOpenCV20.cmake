# To find OpenCV 2.0 library visit http://opencv.willowgarage.com/wiki/
#
# The follwoing variables are optionally searched for defaults
#  OpenCV20_ROOT_DIR:                   Base directory of OpenCV 2.0 tree to use.
#  OpenCV20_FIND_REQUIRED_COMPONENTS:   FIND_PACKAGE(OpenCV20 COMPONENTS ...) 
#      where COMPONENTS:                CV CXCORE CVAUX HIGHGUI CVCAM ...
#
# The following are set after configuration is done: 
#  OpenCV20_FOUND
#  OpenCV20_INCLUDE_DIRS
#  OpenCV20_LIBRARIES
#  OpenCV20_LINK_DIRECTORIES
#
# ---------------------
# Original version:
#   2004/05 Jan Woetzel, Friso, Daniel Grest 
#   2006/01 complete rewrite by Jan Woetzel
#   2006/09 2nd rewrite introducing ROOT_DIR and PATH_SUFFIXES 
#           to handle multiple installed versions gracefully by Jan Woetzel
#
#   tested with:
#   -OpenCV 0.97 (beta5a):  MSVS 7.1, gcc 3.3, gcc 4.1
#   -OpenCV 0.99 (1.0rc1):  MSVS 7.1
#
#   www.mip.informatik.uni-kiel.de/~jw
# ---------------------
#
# $Id$
# ERC-CISST version:
#  
# Balazs [2010-03-19]:
# - Modified for OpenCV 2.0
# - Tested:
#    - Ubuntu 9.10 64 bit
#    - Windows XP 64
#    - OS X Snow Leopard (OpenCV 2.0 installed by MacPorts)
# Anton [original]:
# - Removed deprecated code starting with cap. OPENCV
# - Removed debugging code and messages
# - Removed path and options specifics to previous authors setups
#
# This file should be removed when CMake will provide an equivalent


# required cv components with header and library if COMPONENTS unspecified
IF(NOT OpenCV20_FIND_COMPONENTS)
    # default
    SET(OpenCV20_FIND_REQUIRED_COMPONENTS CV CXCORE CVAUX HIGHGUI)
    IF(WIN32)
        LIST(APPEND OpenCV20_FIND_REQUIRED_COMPONENTS CVCAM) #WIN32 only actually
    ENDIF(WIN32)  
ENDIF(NOT OpenCV20_FIND_COMPONENTS)


# typical root dirs of installations, exactly one of them is used
IF(WIN32)
    SET(OpenCV20_POSSIBLE_ROOT_DIRS
        "$ENV{OpenCV_ROOT_DIR}"
        "$ENV{SystemDrive}/OpenCV2.0"                     # Windows: OpenCV 2.0 default installation dir
        "$ENV{SystemDrive}/Program Files/OpenCV2.0"       # 32 bit ProgramFiles dir on Win32;  64 bit ProgramFiles dir on Win64
        "$ENV{SystemDrive}/Program Files (x86)/OpenCV2.0" # 32 bit ProgramFiles dir on Win64
        )
ELSE(WIN32)
    SET(OpenCV20_POSSIBLE_ROOT_DIRS
        /usr/local                          # Linux: default dir by CMake
        /usr                                # Linux
        /opt/local                          # OS X: default MacPorts location
        )
ENDIF(WIN32)


# select exactly ONE OpenCV 2.0 base directory
# to avoid mixing different version headers and libs
FIND_PATH(OpenCV20_ROOT_DIR 
          NAMES include/opencv/cv.h     # Windows and Linux
          PATHS ${OpenCV20_POSSIBLE_ROOT_DIRS}
          )


# find include files
FIND_PATH(OpenCV20_CV_INCLUDE_DIR       NAMES cv.h      PATHS "${OpenCV20_ROOT_DIR}/include/opencv")
FIND_PATH(OpenCV20_CXCORE_INCLUDE_DIR   NAMES cxcore.h  PATHS "${OpenCV20_ROOT_DIR}/include/opencv")
FIND_PATH(OpenCV20_CVAUX_INCLUDE_DIR    NAMES cvaux.h   PATHS "${OpenCV20_ROOT_DIR}/include/opencv")
FIND_PATH(OpenCV20_HIGHGUI_INCLUDE_DIR  NAMES highgui.h PATHS "${OpenCV20_ROOT_DIR}/include/opencv")
FIND_PATH(OpenCV20_CVCAM_INCLUDE_DIR    NAMES cvcam.h   PATHS "${OpenCV20_ROOT_DIR}/include/opencv")


# find sbsolute path to all libraries 
# some are optional; some may not exist on Linux
SET(OPENCV20_LIBRARY_SEARCH_PATHS
    "${OpenCV20_ROOT_DIR}/lib"
    "${OpenCV20_ROOT_DIR}/lib/Release"
    "${OpenCV20_ROOT_DIR}/lib/Debug"
    )

IF(UNIX)
    FIND_LIBRARY(OpenCV20_CV_LIBRARY                NAMES cv                PATHS ${OPENCV20_LIBRARY_SEARCH_PATHS})
    FIND_LIBRARY(OpenCV20_CVAUX_LIBRARY             NAMES cvaux             PATHS ${OPENCV20_LIBRARY_SEARCH_PATHS})
    FIND_LIBRARY(OpenCV20_CVCAM_LIBRARY             NAMES cvcam             PATHS ${OPENCV20_LIBRARY_SEARCH_PATHS})
    FIND_LIBRARY(OpenCV20_CVHAARTRAINING_LIBRARY    NAMES cvhaartraining    PATHS ${OPENCV20_LIBRARY_SEARCH_PATHS})
    FIND_LIBRARY(OpenCV20_CXCORE_LIBRARY            NAMES cxcore            PATHS ${OPENCV20_LIBRARY_SEARCH_PATHS})
    FIND_LIBRARY(OpenCV20_CXTS_LIBRARY              NAMES cxts              PATHS ${OPENCV20_LIBRARY_SEARCH_PATHS})
    FIND_LIBRARY(OpenCV20_HIGHGUI_LIBRARY           NAMES highgui           PATHS ${OPENCV20_LIBRARY_SEARCH_PATHS})
    FIND_LIBRARY(OpenCV20_ML_LIBRARY                NAMES ml                PATHS ${OPENCV20_LIBRARY_SEARCH_PATHS})
    FIND_LIBRARY(OpenCV20_TRS_LIBRARY               NAMES trs               PATHS ${OPENCV20_LIBRARY_SEARCH_PATHS})
ELSE(UNIX)
    FIND_LIBRARY(OpenCV20_CV_LIBRARY                NAMES cv200             PATHS ${OPENCV20_LIBRARY_SEARCH_PATHS})
    FIND_LIBRARY(OpenCV20_CVAUX_LIBRARY             NAMES cvaux200          PATHS ${OPENCV20_LIBRARY_SEARCH_PATHS})
    FIND_LIBRARY(OpenCV20_CVCAM_LIBRARY             NAMES cvcam200          PATHS ${OPENCV20_LIBRARY_SEARCH_PATHS})
    FIND_LIBRARY(OpenCV20_CVHAARTRAINING_LIBRARY    NAMES cvhaartraining    PATHS ${OPENCV20_LIBRARY_SEARCH_PATHS})
    FIND_LIBRARY(OpenCV20_CXCORE_LIBRARY            NAMES cxcore200         PATHS ${OPENCV20_LIBRARY_SEARCH_PATHS})
    FIND_LIBRARY(OpenCV20_CXTS_LIBRARY              NAMES cxts200           PATHS ${OPENCV20_LIBRARY_SEARCH_PATHS})
    FIND_LIBRARY(OpenCV20_HIGHGUI_LIBRARY           NAMES highgui200        PATHS ${OPENCV20_LIBRARY_SEARCH_PATHS})
    FIND_LIBRARY(OpenCV20_ML_LIBRARY                NAMES ml200             PATHS ${OPENCV20_LIBRARY_SEARCH_PATHS})
    FIND_LIBRARY(OpenCV20_TRS_LIBRARY               NAMES trs200            PATHS ${OPENCV20_LIBRARY_SEARCH_PATHS})
ENDIF(UNIX)

# Logic selecting required libs and headers
SET(OpenCV20_FOUND ON)
IF(NOT OpenCV20_FIND_COMPONENTS)
    FOREACH(NAME ${OpenCV20_FIND_REQUIRED_COMPONENTS})
        # only good if header and library both found   
        IF(OpenCV20_${NAME}_INCLUDE_DIR AND OpenCV20_${NAME}_LIBRARY)
            LIST(APPEND OpenCV20_INCLUDE_DIRS ${OpenCV20_${NAME}_INCLUDE_DIR})
            LIST(APPEND OpenCV20_LIBRARIES    ${OpenCV20_${NAME}_LIBRARY})
        ELSE(OpenCV20_${NAME}_INCLUDE_DIR AND OpenCV20_${NAME}_LIBRARY)
            SET(OpenCV20_FOUND OFF)
        ENDIF(OpenCV20_${NAME}_INCLUDE_DIR AND OpenCV20_${NAME}_LIBRARY)
    ENDFOREACH(NAME)
ELSE(NOT OpenCV20_FIND_COMPONENTS)
    FOREACH(NAME ${OpenCV20_FIND_COMPONENTS})
        # only good if header and library both found   
        IF(OpenCV20_${NAME}_INCLUDE_DIR AND OpenCV20_${NAME}_LIBRARY)
            LIST(APPEND OpenCV20_INCLUDE_DIRS ${OpenCV20_${NAME}_INCLUDE_DIR})
            LIST(APPEND OpenCV20_LIBRARIES    ${OpenCV20_${NAME}_LIBRARY})
        ELSE(OpenCV20_${NAME}_INCLUDE_DIR AND OpenCV20_${NAME}_LIBRARY)
            SET(OpenCV20_FOUND OFF)
        ENDIF(OpenCV20_${NAME}_INCLUDE_DIR AND OpenCV20_${NAME}_LIBRARY)
    ENDFOREACH(NAME)
ENDIF(NOT OpenCV20_FIND_COMPONENTS)


# get the link directory for rpath to be used with LINK_DIRECTORIES: 
IF(OpenCV20_CV_LIBRARY)
    GET_FILENAME_COMPONENT(OpenCV20_LINK_DIRECTORIES ${OpenCV20_CV_LIBRARY} PATH)
ENDIF(OpenCV20_CV_LIBRARY)


MARK_AS_ADVANCED(FORCE
                 OpenCV20_ROOT_DIR
                 OpenCV20_INCLUDE_DIRS
                 OpenCV20_CV_INCLUDE_DIR
                 OpenCV20_CXCORE_INCLUDE_DIR
                 OpenCV20_CVAUX_INCLUDE_DIR
                 OpenCV20_CVCAM_INCLUDE_DIR
                 OpenCV20_HIGHGUI_INCLUDE_DIR
                 OpenCV20_LIBRARIES
                 OpenCV20_CV_LIBRARY
                 OpenCV20_CXCORE_LIBRARY
                 OpenCV20_CVAUX_LIBRARY
                 OpenCV20_CVCAM_LIBRARY
                 OpenCV20_CVHAARTRAINING_LIBRARY
                 OpenCV20_CXTS_LIBRARY
                 OpenCV20_HIGHGUI_LIBRARY
                 OpenCV20_ML_LIBRARY
                 OpenCV20_TRS_LIBRARY
                 )


# display help message
IF(NOT OpenCV20_FOUND)
    # make FIND_PACKAGE friendly
    IF(OpenCV20_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "OpenCV 2.0 not found. Please specify it's location with the OpenCV_ROOT_DIR env. variable.")
    ELSE(OpenCV20_FIND_REQUIRED)
        MESSAGE(STATUS "OpenCV 2.0 not found.")
    ENDIF(OpenCV20_FIND_REQUIRED)
ENDIF(NOT OpenCV20_FOUND)

