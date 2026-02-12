/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:   2020-07-17

  (C) Copyright 2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


%module cisstMeshPython

%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%include "std_pair.i"
%include "std_streambuf.i"
%include "std_iostream.i"

%include "swigrun.i"

%import "cisstConfig.h"

%import "cisstCommon/cisstCommon.i"

%header %{
#include <cisstMesh/mshPython.h>
%}

// Generate parameter documentation for IRE
%feature("autodoc", "1");

#define CISST_EXPORT
#define CISST_DEPRECATED

// %include "cisstMesh/osaTimeServer.h"
// %include "cisstMesh/osaSerialPort.h"
