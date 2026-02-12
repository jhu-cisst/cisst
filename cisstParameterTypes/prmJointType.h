/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:	2008-04-08

  (C) Copyright 2024 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief For backward compatibility, cmnJointType should be used instead
*/


#ifndef _prmJointType_h
#define _prmJointType_h

#include <cisstCommon/cmnJointType.h>

typedef cmnJointType prmJointType;

#define PRM_JOINT_UNDEFINED CMN_JOINT_UNDEFINED
#define PRM_JOINT_PRISMATIC CMN_JOINT_PRISMATIC
#define PRM_JOINT_REVOLUTE  CMN_JOINT_REVOLUTE
#define PRM_JOINT_INACTIVE  CMN_JOINT_INACTIVE

#define prmJointTypeToFactor cmnJointTypeToFactor

#endif  // _prmJointType_h
