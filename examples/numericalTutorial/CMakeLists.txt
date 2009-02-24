#
# $Id$
#
# (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
# Reserved.
# 
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

ADD_EXECUTABLE(numericalTutorial
               main.cpp
               ExampleSVDUserOutputWorkspace.h
               ExampleSVDImplicitWorkspace.h
               ExampleSVDWorkspaceSize.h
               ExampleSVDDynamicData.h
               ExampleSVDUpdateMatrixS.h
               ExampleSVDFixedSize.h
               ExampleSVDFixedSizeData.h
               ExampleInverse.h
               ExamplePInverse.h
               ExampleLUDynamicData.h
               ExampleIsOrthonormal.h
               ExampleGaussJordanInverse.h
)

CISST_REQUIRES("numericalTutorial" "cisstCommon;cisstVector;cisstNumerical")
VARIABLE_REQUIRES(BUILD_EXAMPLES_numericalTutorial REQUIREMENT_MET CISST_HAS_CISSTNETLIB)

IF(CISST_HAS_CISSTNETLIB)
  TARGET_LINK_LIBRARIES(numericalTutorial
                        ${CISSTNETLIB_LIBRARIES}
                        ${CISSTNETLIB_F95_LIBRARIES}
                        ${CISSTNETLIB_GCC_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT})
ENDIF(CISST_HAS_CISSTNETLIB)
