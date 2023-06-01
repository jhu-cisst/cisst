/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2010-10-20

  (C) Copyright 2010-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstParameterTypes/prmActuatorParameters.h>
CMN_IMPLEMENT_SERVICES(prmActuatorParameters);

#include <cisstParameterTypes/prmActuatorState.h>
CMN_IMPLEMENT_SERVICES(prmActuatorState);

#include <cisstParameterTypes/prmEventButton.h>
CMN_IMPLEMENT_SERVICES(prmEventButton);

#include <cisstParameterTypes/prmForceCartesianGet.h>
CMN_IMPLEMENT_SERVICES(prmForceCartesianGet);

#include <cisstParameterTypes/prmForceCartesianSet.h>
CMN_IMPLEMENT_SERVICES(prmForceCartesianSet);

#include <cisstParameterTypes/prmForceTorqueJointSet.h>
CMN_IMPLEMENT_SERVICES(prmForceTorqueJointSet);

#include "cisstParameterTypes/prmGainParameters.h"
CMN_IMPLEMENT_SERVICES(prmGainParameters);

#include <cisstParameterTypes/prmMaskedVector.h>
CMN_IMPLEMENT_SERVICES_TEMPLATED(prmMaskedDoubleVec);
CMN_IMPLEMENT_SERVICES_TEMPLATED(prmMaskedFloatVec);
CMN_IMPLEMENT_SERVICES_TEMPLATED(prmMaskedLongVec);
CMN_IMPLEMENT_SERVICES_TEMPLATED(prmMaskedULongVec);
CMN_IMPLEMENT_SERVICES_TEMPLATED(prmMaskedIntVec);
CMN_IMPLEMENT_SERVICES_TEMPLATED(prmMaskedUIntVec);
CMN_IMPLEMENT_SERVICES_TEMPLATED(prmMaskedShortVec);
CMN_IMPLEMENT_SERVICES_TEMPLATED(prmMaskedUShortVec);
CMN_IMPLEMENT_SERVICES_TEMPLATED(prmMaskedCharVec);
CMN_IMPLEMENT_SERVICES_TEMPLATED(prmMaskedUCharVec);
CMN_IMPLEMENT_SERVICES_TEMPLATED(prmMaskedBoolVec);

#include <cisstParameterTypes/prmPositionCartesianGet.h>
CMN_IMPLEMENT_SERVICES(prmPositionCartesianGet);

#include <cisstParameterTypes/prmPositionCartesianArrayGet.h>
CMN_IMPLEMENT_SERVICES(prmPositionCartesianArrayGet);

#include <cisstParameterTypes/prmPositionCartesianSet.h>
CMN_IMPLEMENT_SERVICES(prmPositionCartesianSet);

#include <cisstParameterTypes/prmPositionJointGet.h>
CMN_IMPLEMENT_SERVICES(prmPositionJointGet);

#include <cisstParameterTypes/prmPositionJointSet.h>
CMN_IMPLEMENT_SERVICES(prmPositionJointSet);

#include <cisstParameterTypes/prmRobotState.h>
CMN_IMPLEMENT_SERVICES(prmRobotState);

#include <cisstParameterTypes/prmTransformationBase.h>
CMN_IMPLEMENT_SERVICES(prmTransformationBase);

#include <cisstParameterTypes/prmTransformationDynamic.h>
CMN_IMPLEMENT_SERVICES(prmTransformationDynamic);

#include <cisstParameterTypes/prmTransformationFixed.h>
CMN_IMPLEMENT_SERVICES(prmTransformationFixed);

#include <cisstParameterTypes/prmTransformationManager.h>
CMN_IMPLEMENT_SERVICES(prmTransformationManager);

#include <cisstParameterTypes/prmVelocityCartesianGet.h>
CMN_IMPLEMENT_SERVICES(prmVelocityCartesianGet);

#include <cisstParameterTypes/prmVelocityCartesianSet.h>
CMN_IMPLEMENT_SERVICES(prmVelocityCartesianSet);

#include <cisstParameterTypes/prmVelocityJointGet.h>
CMN_IMPLEMENT_SERVICES(prmVelocityJointGet);

#include <cisstParameterTypes/prmVelocityJointSet.h>
CMN_IMPLEMENT_SERVICES(prmVelocityJointSet);

#include <cisstParameterTypes/prmCartesianImpedance.h>
CMN_IMPLEMENT_SERVICES(prmCartesianImpedance);

#include <cisstParameterTypes/prmJointType.h>
CMN_IMPLEMENT_SERVICES_TEMPLATED(prmJointTypeProxy);

#include <cisstParameterTypes/prmIMUSensors.h>
CMN_IMPLEMENT_SERVICES(prmIMUSensors);

#include <cisstParameterTypes/prmConfigurationJoint.h>
CMN_IMPLEMENT_SERVICES(prmConfigurationJoint);

#include <cisstParameterTypes/prmStateJoint.h>
CMN_IMPLEMENT_SERVICES(prmStateJoint);

#include <cisstParameterTypes/prmInputData.h>
CMN_IMPLEMENT_SERVICES(prmInputData);

#include <cisstParameterTypes/prmOperatingState.h>
CMN_IMPLEMENT_SERVICES(prmOperatingState);

#include <cisstParameterTypes/prmForwardKinematicsRequest.h>
CMN_IMPLEMENT_SERVICES(prmForwardKinematicsRequest);

#include <cisstParameterTypes/prmForwardKinematicsResponse.h>
CMN_IMPLEMENT_SERVICES(prmForwardKinematicsResponse);

#include <cisstParameterTypes/prmInverseKinematicsRequest.h>
CMN_IMPLEMENT_SERVICES(prmInverseKinematicsRequest);

#include <cisstParameterTypes/prmInverseKinematicsResponse.h>
CMN_IMPLEMENT_SERVICES(prmInverseKinematicsResponse);

#if CISST_HAS_JSON
CMN_IMPLEMENT_DATA_FUNCTIONS_JSON_FOR_ENUM(prmBlocking, int);
#endif
