#
# $Id$
#
# (C) Copyright 2003-2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

cmake_minimum_required (VERSION 2.8)

# create a list of libraries needed for this project
set (CISST_LIBRARIES_REQUIRED cisstCommon cisstVector cisstNumerical)

# find cisst and make sure the required libraries have been compiled
find_package (cisst REQUIRED ${CISST_LIBRARIES_REQUIRED})

if (cisst_FOUND)

  # load cisst configuration
  include (${CISST_USE_FILE})

  # make sure cisstNumerical has been compiled with cisstNetlib
  if (NOT CISST_HAS_CISSTNETLIB)
    message (SEND_ERROR "cisstVector example needs cisstNetlib, make sure cisstNumerical is compiled with cisstNetlib")
  endif (NOT CISST_HAS_CISSTNETLIB)

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
  # link against cisst libraries (and dependencies)
  cisst_target_link_libraries (vectorTutorial ${CISST_LIBRARIES_REQUIRED})

endif (cisst_FOUND)
