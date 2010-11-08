#
# $Id$
#
# (C) Copyright 2003-2010 Johns Hopkins University (JHU), All Rights
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

# all source files
set (SOURCE_FILES
     imgUCharRGB.cpp
     )

# all header files
set (HEADER_FILES
     imgExport.h
     imgUCharRGB.h
     )

cisst_add_library (LIBRARY cisstImage
                   DEPENDENCIES cisstCommon cisstVector
                   SOURCE_FILES ${SOURCE_FILES}
                   HEADER_FILES ${HEADER_FILES}
                   )
