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

PROJECT(registrationTest)

ADD_EXECUTABLE(accuracy Accuracy.cpp DataSet.cpp DataSet.h)

ADD_EXECUTABLE(distances Distances.cpp DataSet.cpp DataSet.h)

CISST_REQUIRES("accuracy" "cisstCommon;cisstVector;cisstNumerical")
CISST_REQUIRES("distances" "cisstCommon;cisstVector")
