#
# $Id: CMakeLists.txt 195 2009-03-26 20:36:16Z adeguet1 $
#
# (C) Copyright 2007-2009 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

add_library(serializationClasses STATIC classes.h classes.cpp)

add_executable(commonTutorialSerializationWrite write.cpp)
cisst_requires(commonTutorialSerializationWrite "cisstCommon")
target_link_libraries(commonTutorialSerializationWrite serializationClasses)

add_executable(commonTutorialSerializationRead read.cpp)
cisst_requires(commonTutorialSerializationRead "cisstCommon")
target_link_libraries(commonTutorialSerializationRead serializationClasses)
