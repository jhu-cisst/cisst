#
# $Id$
#

# Backward compatibility
CMAKE_MINIMUM_REQUIRED(VERSION 2.0)
PROJECT(registrationTest)

ADD_EXECUTABLE(accuracy Accuracy.cpp DataSet.cpp DataSet.h)

ADD_EXECUTABLE(distances Distances.cpp DataSet.cpp DataSet.h)

CISST_REQUIRES("accuracy" "cisstCommon;cisstVector;cisstNumerical")
CISST_REQUIRES("distances" "cisstCommon;cisstVector")
