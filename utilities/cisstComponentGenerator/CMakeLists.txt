#
# $Id: CMakeLists.txt 1236 2010-06-30 09:56:14Z jkriss1 $
#
# (C) Copyright 2005-2010 Johns Hopkins University (JHU), All Rights
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
add_executable (cisstComponentGenerator
                cisstComponentGenerator.cpp
                ccgComponent.h
                ccgParser.h
                ccgComponent.cpp
                ccgParser.cpp)

cisst_target_link_libraries (cisstComponentGenerator cisstCommon)
