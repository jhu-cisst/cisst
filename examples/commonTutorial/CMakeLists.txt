#
# $Id: CMakeLists.txt,v 1.2 2008/08/21 21:27:46 anton Exp $
#
# (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
# Reserved.
# 
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

ADD_EXECUTABLE(commonTutorial
               main.cpp
               portability.cpp
               units.cpp)

CISST_REQUIRES("commonTutorial" "cisstCommon")
