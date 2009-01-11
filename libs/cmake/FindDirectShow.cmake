#
# $Id: FindDirectShow.cmake 8 2009-01-04 21:13:48Z adeguet1 $
#
# Author(s):  Anton Deguet
# Created on: 2007-02-23
#
# (C) Copyright 2007-2007 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

#
# This file contains code found on the Wiki page of CMake
#

# - Test for DirectShow on Windows.
# Once loaded this will define
#   DIRECTSHOW_FOUND        - system has DirectShow
#   DIRECTSHOW_INCLUDE_DIR  - include directory for DirectShow
#   DIRECTSHOW_LIBRARIES    - libraries you need to link to

SET(DIRECTSHOW_FOUND "NO")

# DirectShow is only available on Windows platforms
IF(WIN32)
  # Find DirectX Include Directory
  FIND_PATH(DIRECTX_INCLUDE_DIR ddraw.h
    "C:/Program Files/Microsoft Visual Studio .NET 2003/Vc7/PlatformSDK/Include"
    "C:/Program Files/Microsoft DirectX SDK (February 2006)/Include"
    "C:/Program Files/Microsoft DirectX 9.0 SDK (June 2005)/Include"
    "C:/DXSDK/Include"
    DOC "What is the path where the file ddraw.h can be found"
  )

  # if DirectX found, then find DirectShow include directory
  IF(DIRECTX_INCLUDE_DIR)
    FIND_PATH(DIRECTSHOW_INCLUDE_DIR dshow.h
      "C:/Program Files/Microsoft Platform SDK for Windows Server 2003 R2/Include"
      "C:/Program Files/Microsoft Visual Studio .NET 2003/Vc7/PlatformSDK/Include"
      "C:/Program Files/Microsoft Platform SDK/Include"
      "C:/DXSDK/Include"
      DOC "What is the path where the file dshow.h can be found"
    )

    # if DirectShow include dir found, then find DirectShow libraries
    IF(DIRECTSHOW_INCLUDE_DIR)
      FIND_LIBRARY(DIRECTSHOW_strmiids_LIBRARY strmiids
        "C:/Program Files/Microsoft Platform SDK for Windows Server 2003 R2/Lib"
        "C:/Program Files/Microsoft Visual Studio .NET 2003/Vc7/PlatformSDK/Lib"
        "C:/Program Files/Microsoft Platform SDK/Lib"
        "C:/DXSDK/Include/Lib"
        DOC "Where can the DirectShow strmiids library be found"
      )
      FIND_LIBRARY(DIRECTSHOW_quartz_LIBRARY quartz
        "C:/Program Files/Microsoft Platform SDK for Windows Server 2003 R2/Lib"
        "C:/Program Files/Microsoft Visual Studio .NET 2003/Vc7/PlatformSDK/Lib"
        "C:/Program Files/Microsoft Platform SDK/Lib"
        "C:/DXSDK/Include/Lib"
        DOC "Where can the DirectShow quartz library be found"
      )
      FIND_LIBRARY(DIRECTSHOW_Vfw32_LIBRARY Vfw32
        "C:/Program Files/Microsoft Platform SDK for Windows Server 2003 R2/Lib"
        "C:/Program Files/Microsoft Visual Studio .NET 2003/Vc7/PlatformSDK/Lib"
        "C:/Program Files/Microsoft Platform SDK/Lib"
        "C:/DXSDK/Include/Lib"
        DOC "Where can the DirectShow Vfw32 library be found"
      )
      FIND_LIBRARY(DIRECTSHOW_WinMM_LIBRARY WinMM
        "C:/Program Files/Microsoft Platform SDK for Windows Server 2003 R2/Lib"
        "C:/Program Files/Microsoft Visual Studio .NET 2003/Vc7/PlatformSDK/Lib"
        "C:/Program Files/Microsoft Platform SDK/Lib"
        "C:/DXSDK/Include/Lib"
        DOC "Where can the DirectShow WinMM library be found"
      )

      # if DirectShow libraries found, then we're ok
      IF(DIRECTSHOW_strmiids_LIBRARY)
        IF(DIRECTSHOW_quartz_LIBRARY)
          # everything found
          SET(DIRECTSHOW_FOUND "YES")
        ENDIF(DIRECTSHOW_quartz_LIBRARY)
      ENDIF(DIRECTSHOW_strmiids_LIBRARY)
    ENDIF(DIRECTSHOW_INCLUDE_DIR)
  ENDIF(DIRECTX_INCLUDE_DIR)
ENDIF(WIN32)


#---------------------------------------------------------------------
IF(DIRECTSHOW_FOUND)
  SET(DIRECTSHOW_INCLUDE_DIR
    ${DIRECTSHOW_INCLUDE_DIR}
    ${DIRECTX_INCLUDE_DIR}
  )

  SET(DIRECTSHOW_LIBRARIES
    ${DIRECTSHOW_strmiids_LIBRARY}
    ${DIRECTSHOW_quartz_LIBRARY}
  )
ELSE(DIRECTSHOW_FOUND)
  # make FIND_PACKAGE friendly
  IF(NOT DIRECTSHOW_FIND_QUIETLY)
    IF(DIRECTSHOW_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR
              "DirectShow required, please specify it's location.")
    ELSE(DIRECTSHOW_FIND_REQUIRED)
      MESSAGE(STATUS "DirectShow was not found.")
    ENDIF(DIRECTSHOW_FIND_REQUIRED)
  ENDIF(NOT DIRECTSHOW_FIND_QUIETLY)
ENDIF(DIRECTSHOW_FOUND)

#
# $Log: FindDirectShow.cmake,v $
# Revision 1.4  2007/04/26 19:33:56  anton
# All files in libraries: Applied new license text, separate copyright and
# updated dates, added standard header where missing.
#
# Revision 1.3  2007/03/09 20:25:07  anton
# FindDirectShow.cmake: Added variables for Vfw32 and WinMMCVS
#
# Revision 1.2  2007/03/06 17:05:49  anton
# FindDirectShow.cmake: Added path for SDK Windows 2003 server R2 first
# as this is our prefered SDK.
#
# Revision 1.1  2007/02/23 16:53:27  anton
# Import code from Balazs for cisstStereoVision.   Tested compilation of portable
# code, not Linux nor Windows specific branches.  Also missing support for IPP.
#
#
