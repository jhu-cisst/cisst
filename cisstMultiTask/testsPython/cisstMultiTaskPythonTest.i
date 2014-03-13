/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2010-02-20

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


%module cisstMultiTaskPythonTestPython

// Following needed to define ptrdiff_t
%include "std_common.i"

%import "cisstMultiTask/cisstMultiTask.i"


%header %{
#include <cisstMultiTask/mtsPython.h>
#include "mtsPeriodicTaskTest.h"
%}

%include "mtsPeriodicTaskTest.h"
