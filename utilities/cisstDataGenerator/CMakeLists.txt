#
# $Id: CMakeLists.txt 213 2009-04-01 15:12:08Z adeguet1 $
#
# (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
# Reserved.
# 
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

# name the main executable and specifies with source files to use
add_executable (cisstDataGenerator
                cisstDataGenerator.cpp
                cdgData.h
                cdgData.cpp
                cdgDataMember.h
                cdgDataMember.cpp
                cdgTypedef.h
                cdgTypedef.cpp
		)

cisst_target_link_libraries (cisstDataGenerator cisstCommon)
