#
# $Id$
#
# (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---
if (CISST_BUILD_LIBS_cisstNumerical AND CISST_HAS_CISSTNETLIB)

  cisst_set_directories (cisstCommon cisstVector cisstNumerical)
  add_executable (numericalTutorial
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
  cisst_target_link_libraries(numericalTutorial cisstCommon cisstVector cisstNumerical)

else (CISST_BUILD_LIBS_cisstNumerical AND CISST_HAS_CISSTNETLIB)
  message (WARNING " numericalTutorial examples will not be compiled since they require CISST_BUILD_LIBS_cisstNumerical and CISST_HAS_CISSTNETLIB")
endif (CISST_BUILD_LIBS_cisstNumerical AND CISST_HAS_CISSTNETLIB)