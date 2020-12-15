/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:   2009-01-26

  (C) Copyright 2006-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


%module cisstParameterTypesPython


%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%include "std_pair.i"
%include "std_streambuf.i"
%include "std_iostream.i"

%include "swigrun.i"

%import "cisstConfig.h"

%import "cisstCommon/cisstCommon.i"
%import "cisstVector/cisstVector.i"
%import "cisstMultiTask/cisstMultiTask.i"

%init %{
    import_array() // numpy initialization
%}

%header %{
#include <cisstParameterTypes/prmPython.h>
%}

// Generate parameter documentation for IRE
%feature("autodoc", "1");

#define CISST_EXPORT
#define CISST_DEPRECATED

// Import file with macros and typedefs
%import "cisstMultiTask/mtsMacros.h"

// Wrap base type
%include "cisstParameterTypes/prmMotionBase.h"

// Wrap all others 
%include "cisstParameterTypes/prmEventButton.h"

%include "cisstParameterTypes/prmPositionCartesianGet.h"
%include "cisstParameterTypes/prmPositionCartesianSet.h"
%include "cisstParameterTypes/prmVelocityCartesianGet.h"
%include "cisstParameterTypes/prmVelocityCartesianSet.h"
%include "cisstParameterTypes/prmForceCartesianGet.h"
%include "cisstParameterTypes/prmForceCartesianSet.h"

%include "cisstParameterTypes/prmPositionJointGet.h"
%include "cisstParameterTypes/prmPositionJointSet.h"
%include "cisstParameterTypes/prmVelocityJointGet.h"
%include "cisstParameterTypes/prmVelocityJointSet.h"

%include "cisstParameterTypes/prmTransformationBase.h"
%include "cisstParameterTypes/prmTransformationManager.h"
%include "cisstParameterTypes/prmTransformationDynamic.h"
%include "cisstParameterTypes/prmTransformationFixed.h"

%include "cisstParameterTypes/prmRobotState.h"
