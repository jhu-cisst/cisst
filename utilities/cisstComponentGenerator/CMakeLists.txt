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

# name of project and executable
project (multiTaskCodeGeneratorCodeGen)

INCLUDE_DIRECTORIES(${multiTaskCodeGeneratorCodeGen_SOURCE_DIR})

# name the main executable and specifies with source files to use
ADD_EXECUTABLE(CodeGenerator
				cisstBuilderMacros.h
				Component.h
				parser.h
				Component.cpp
				parser.cpp
				parserMain.cpp)
