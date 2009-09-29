#
# $Id: CMakeLists.txt 75 2009-02-24 16:47:20Z adeguet1 $
#
# (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# CMakeLists for cisstImage
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

# set the library name
SET(LIBRARY cisstImage)

# set dependencies
SET(DEPENDENCIES
    cisstCommon
    cisstVector)

# all source files
SET(SOURCE_FILES
    imgUCharRGB.cpp
    )

# all header files
SET(HEADER_FILES
    imgExport.h
    imgUCharRGB.h
    )


CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)
