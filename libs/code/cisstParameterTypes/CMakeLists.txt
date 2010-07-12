# CMakeLists for prmParameterTypes
#
# (C) Copyright 2003-2008 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# set the library name
SET(LIBRARY cisstParameterTypes)

# set dependencies
SET(DEPENDENCIES
    cisstCommon
    cisstVector
    cisstOSAbstraction
    cisstMultiTask
    cisstRobot
    )

# all source files
SET(SOURCE_FILES
    prmEventButton.cpp
    prmMotionBase.cpp

    prmPositionJointGet.cpp
    prmPositionJointSet.cpp
    prmVelocityJointGet.cpp
    prmVelocityJointSet.cpp

    prmPositionCartesianGet.cpp
    prmPositionCartesianSet.cpp
    prmVelocityCartesianGet.cpp
    prmVelocityCartesianSet.cpp
    prmForceCartesianGet.cpp
    prmForceCartesianSet.cpp

    prmTransformationBase.cpp
    prmTransformationFixed.cpp
    prmTransformationDynamic.cpp
    prmTransformationManager.cpp

    prmMaskedVector.cpp
    prmActuatorState.cpp
    prmRobotState.cpp
    prmActuatorParameters.cpp
    prmGainParameters.cpp

    prmCollaborativeControlForce.cpp
    )

# all header files
SET(HEADER_FILES
    prmEventButton.h
    prmExport.h
    prmForwardDeclarations.h
    prmMotionBase.h

    prmPositionJointGet.h
    prmPositionJointSet.h
    prmVelocityJointGet.h
    prmVelocityJointSet.h

    prmPositionCartesianGet.h
    prmPositionCartesianSet.h
    prmVelocityCartesianGet.h
    prmVelocityCartesianSet.h
    prmForceCartesianGet.h
    prmForceCartesianSet.h

    prmTransformationBase.h
    prmTransformationFixed.h
    prmTransformationDynamic.h
    prmTransformationManager.h

    prmMaskedVector.h
    prmActuatorState.h
    prmRobotState.h
    prmActuatorParameters.h
    prmGainParameters.h

    prmCollaborativeControlForce.h
    )

CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)

# Python wrappers
if (CISST_HAS_SWIG_PYTHON)
  cisst_add_swig_module(MODULE cisstParameterTypes
                        INTERFACE_DIRECTORY ../../include/cisstParameterTypes
			CISST_LIBRARIES cisstParameterTypes
                                        cisstMultiTask
					cisstOSAbstraction
					cisstVector
					cisstCommon)
endif (CISST_HAS_SWIG_PYTHON)
