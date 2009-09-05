#
# $Id$
#

# Backward compatibility
CMAKE_MINIMUM_REQUIRED(VERSION 2.0)
PROJECT(registrationTest)

ADD_EXECUTABLE(Accuracy Accuracy.cpp DataSet.cpp DataSet.h)

ADD_EXECUTABLE(Distances Distances.cpp DataSet.cpp DataSet.h)

CISST_REQUIRES("Accuracy" "cisstCommon;cisstVector;cisstNumerical")
CISST_REQUIRES("Distances" "cisstCommon;cisstVector")
