#
# (C) Copyright 2003-2023 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (cisstParameterTypes)

# create data type using the data generator
cisst_data_generator (cisstParameterTypes
                      "${cisst_BINARY_DIR}/include" # where to save the file
                      "cisstParameterTypes/"        # sub directory for include
                      ../prmActuatorJointCoupling.cdg
                      ../prmPositionCartesianGet.cdg
                      ../prmPositionCartesianArrayGet.cdg
                      ../prmPositionCartesianSet.cdg
                      ../prmVelocityCartesianGet.cdg
                      ../prmPositionJointGet.cdg
                      ../prmPositionJointSet.cdg
                      ../prmForceTorqueJointSet.cdg
                      ../prmCartesianImpedance.cdg
                      ../prmIMUSensors.cdg
                      ../prmStateJoint.cdg
                      ../prmConfigurationJoint.cdg
                      ../prmInputData.cdg
                      ../prmKeyValue.cdg
                      ../prmOperatingState.cdg
                      ../prmForwardKinematicsRequest.cdg
                      ../prmForwardKinematicsResponse.cdg
                      ../prmInverseKinematicsRequest.cdg
                      ../prmInverseKinematicsResponse.cdg
                      )

# to compile cisst generated code, need to find header file
include_directories (${CMAKE_CURRENT_BINARY_DIR})


# all source files
set (SOURCE_FILES
     prmEventButton.cpp
     prmMotionBase.cpp
     prmJointType.cpp

     prmVelocityJointGet.cpp
     prmVelocityJointSet.cpp

     prmVelocityCartesianSet.cpp
     prmForceCartesianGet.cpp
     prmForceCartesianSet.cpp

     prmTransformationBase.cpp
     prmTransformationFixed.cpp
     prmTransformationDynamic.cpp
     prmTransformationManager.cpp

     prmActuatorState.cpp
     prmRobotState.cpp
     prmActuatorParameters.cpp

     prmInputDataConverter.cpp
     prmOperatingState.cpp

     prmClassServices.cpp
     )

# all header files
set (HEADER_FILES
     prmEventButton.h
     prmExport.h
     prmForwardDeclarations.h
     prmMotionBase.h
     prmJointType.h

     prmVelocityJointGet.h
     prmVelocityJointSet.h

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

     prmInputDataConverter.h
     )

cisst_add_library (LIBRARY cisstParameterTypes
                   FOLDER cisstParameterTypes
                   DEPENDENCIES cisstCommon cisstVector cisstOSAbstraction cisstMultiTask
                   SOURCE_FILES ${SOURCE_FILES}
                   HEADER_FILES ${HEADER_FILES}
                   ADDITIONAL_SOURCE_FILES ${cisstParameterTypes_CISST_DG_SRCS}
                   ADDITIONAL_HEADER_FILES_RELATIVE ${cisstParameterTypes_CISST_DG_HDRS}
                   )

install (FILES ${cisstParameterTypes_CISST_DG_HDRS_FULLPATH}
         DESTINATION "include/cisstParameterTypes")
