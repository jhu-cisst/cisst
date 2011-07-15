#
# $Id$
#
# (C) Copyright 2003-2010 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

if (BUILD_LIBS_cisstNumerical AND CISST_HAS_CISSTNETLIB)

  add_executable (vectorTutorial
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

  cisst_target_link_libraries (vectorTutorial cisstCommon cisstVector cisstNumerical)

else (BUILD_LIBS_cisstNumerical AND CISST_HAS_CISSTNETLIB)
  message (WARNING " vectorTutorial examples will not be compiled since they require BUILD_LIBS_cisstNumerical and CISST_HAS_CISSTNETLIB")
endif (BUILD_LIBS_cisstNumerical AND CISST_HAS_CISSTNETLIB)