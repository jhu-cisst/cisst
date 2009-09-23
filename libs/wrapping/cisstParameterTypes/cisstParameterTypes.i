/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cisstParameterTypes.i,v 1.4 2009/01/07 05:04:36 pkaz Exp $

  Author(s):	Anton Deguet
  Created on:   2009-01-26

  (C) Copyright 2006-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

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

%header %{
    // Put header files here
    #include "cisstParameterTypes/cisstParameterTypes.i.h"
%}

// Generate parameter documentation for IRE
%feature("autodoc", "1");

%rename(__str__) ToString;
%ignore *::ToStream;
%ignore operator<<;

%ignore *::operator[]; // We define __setitem__ and __getitem__

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
%include "cisstParameterTypes/prmCollaborativeControlForce.h"

%include "cisstParameterTypes/prmPositionJointGet.h"
%include "cisstParameterTypes/prmPositionJointSet.h"
%include "cisstParameterTypes/prmVelocityJointGet.h"
%include "cisstParameterTypes/prmVelocityJointSet.h"
