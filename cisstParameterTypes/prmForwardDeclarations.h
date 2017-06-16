/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Rajesh Kumar, Anton Deguet
  Created on:   2008-03-03

  (C) Copyright 2007-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Forward declarations for cisstParameterTypes
*/


#ifndef _prmForwardDeclarations_h
#define _prmForwardDeclarations_h

// transformation manager related classes
class prmTransformationBase;
class prmTransformationFixed;
class prmTransformationDynamic;
class prmTransformationManager;

// motion enums
/*! Parameters type for command execution/completion behavior specification */
typedef enum Blocking { 
	NO_WAIT,      /*! non blocking */
	WAIT_START,   /*! blocking until start, non preemptive */
	WAIT_FINISH   /*! blocking type  */
} prmBlocking;

#include <cisstCommon/cmnDataFunctionsEnumMacros.h>
CMN_DATA_SPECIALIZATION_FOR_ENUM(Blocking, int);
#if CISST_HAS_JSON
CMN_DECLARE_DATA_FUNCTIONS_JSON_FOR_ENUM(Blocking);
#endif

// robot API types
class prmPositionCartesianGet;
class prmPositionCartesianSet;
class prmVelocityCartesianGet;
class prmVelocityCartesianSet;
class prmForceCartesianGet;
class prmForceCartesianSet;

class prmPositionJointGet;
class prmPositionJointSet;
class prmVelocityJointGet;
class prmVelocityJointSet;

class prmEventButton;

#endif // _prmForwardDeclarations_h

