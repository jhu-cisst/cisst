/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:	2009-11-08

  (C) Copyright 2009-2024 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Header files from cisstParameterTypes required to compile the SWIG generated Python wrappers
 */
#pragma once

#ifndef _prmPython_h
#define _prmPython_h

#include <cisstCommon/cmnPython.h>
#include <cisstVector/vctPython.h>
#include <cisstMultiTask/mtsPython.h>

#include <cisstParameterTypes/prmEventButton.h>

#include <cisstParameterTypes/prmPositionCartesianGet.h>
#include <cisstParameterTypes/prmPositionCartesianSet.h>
#include <cisstParameterTypes/prmVelocityCartesianGet.h>
#include <cisstParameterTypes/prmVelocityCartesianSet.h>
#include <cisstParameterTypes/prmForceCartesianGet.h>
#include <cisstParameterTypes/prmForceCartesianSet.h>

#include <cisstParameterTypes/prmPositionJointGet.h>
#include <cisstParameterTypes/prmPositionJointSet.h>
#include <cisstParameterTypes/prmVelocityJointGet.h>
#include <cisstParameterTypes/prmVelocityJointSet.h>

#include <cisstParameterTypes/prmRobotState.h>

#include <cisstParameterTypes/prmStateJoint.h>
#include <cisstParameterTypes/prmConfigurationJoint.h>
#include <cisstParameterTypes/prmOperatingState.h>
#include <cisstParameterTypes/prmActuatorState.h>

#include <cisstParameterTypes/prmImageFrame.h>
#include <cisstParameterTypes/prmCameraInfo.h>
#include <cisstParameterTypes/prmDepthMap.h>

#endif // _prmPython_h
