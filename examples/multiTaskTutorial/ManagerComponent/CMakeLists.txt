#
# $Id: CMakeLists.txt 1295 2010-03-17 04:01:25Z adeguet1 $
#
# (C) Copyright 2005-2010 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

# name of project
PROJECT(ManagerComponent)

# name the main executable and specifies with source files to use
ADD_EXECUTABLE(ManagerComponentLocal mainControllerLocal.cpp
                                     ManagerComponentLocal.h ManagerComponentLocal.cpp
                                     CounterOddComponent.h CounterOddComponent.cpp
                                     CounterEvenComponent.h CounterEvenComponent.cpp)
CISST_REQUIRES("ManagerComponentLocal" "cisstCommon;cisstOSAbstraction;cisstMultiTask")

# compile network version if ICE is available
if (CISST_MTS_HAS_ICE)

  # name the main executable and specifies with source files to use
  ADD_EXECUTABLE(ManagerComponentController mainController.cpp
                                            ManagerComponent.h ManagerComponent.cpp)
  CISST_REQUIRES("ManagerComponentController" "cisstCommon;cisstOSAbstraction;cisstMultiTask")

  # name the main executable and specifies with source files to use
  ADD_EXECUTABLE(ManagerComponentCounter mainCounter.cpp
                                         CounterOddComponent.h CounterOddComponent.cpp
                                         CounterEvenComponent.h CounterEvenComponent.cpp)
  CISST_REQUIRES("ManagerComponentCounter" "cisstCommon;cisstOSAbstraction;cisstMultiTask")
endif (CISST_MTS_HAS_ICE)
