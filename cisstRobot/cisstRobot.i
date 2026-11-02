/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:   2016-03-21

  (C) Copyright 2016 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


%module cisstRobotPython

%include "swigrun.i"
%include "std_vector.i"

%import "cisstConfig.h"

%{
// Rely on cisstCommon.i for Numpy initialization
#define NO_IMPORT_ARRAY
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>
%}

%import "cisstCommon/cisstCommon.i"

%header %{
#include <cisstRobot/robPython.h>
%}

// Generate parameter documentation for IRE
%feature("autodoc", "1");

#define CISST_EXPORT
#define CISST_DEPRECATED

%include "cisstRobot/robLinkDynamics.h"
%include "cisstRobot/robModifiedDH.h"
