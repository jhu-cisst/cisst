#
# $Id$
#
# (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
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

  add_executable (accuracy Accuracy.cpp DataSet.cpp DataSet.h)
  cisst_target_link_libraries (accuracy cisstCommon cisstVector cisstNumerical)

  add_executable (distances Distances.cpp DataSet.cpp DataSet.h)
  cisst_target_link_libraries (distances cisstCommon cisstVector)

else (BUILD_LIBS_cisstNumerical AND CISST_HAS_CISSTNETLIB)
  message (WARNING " registrationTest examples will not be compiled since they require BUILD_LIBS_cisstNumerical and CISST_HAS_CISSTNETLIB")
endif (BUILD_LIBS_cisstNumerical AND CISST_HAS_CISSTNETLIB)