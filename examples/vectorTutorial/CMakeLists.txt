#
# $Id$
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

ADD_EXECUTABLE(vectorTutorial
               main.cpp
               ExampleFrame.h
               ExampleCommon.h
               ExampleReference.h
               ExampleReferenceFixedSize.h
               ExampleDynamic.h
               ExampleFixedSizeFunctions.h
               ExampleDynamicFunctions.h
	       ExampleNArray.h
               ExampleNumerical.h
               ExampleSTL.h
)

CISST_REQUIRES("vectorTutorial" "cisstCommon;cisstVector;cisstNumerical")
