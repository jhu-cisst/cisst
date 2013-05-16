/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2004-03-29

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


%module cisstVectorPython

%include <std_string.i>
%include <std_except.i>
%include <std_vector.i>

%import "cisstCommon/cisstCommon.i"

%header %{
#include <cisstVector/vctPython.h>
%}

%ignore *::operator[]; // We define __setitem__ and __getitem__
%ignore *::operator=; // Just to avoid warnings

// Generate parameter documentation for IRE
%feature("autodoc", "1");

// The traits are used everywhere
%include "cisstVector/vctContainerTraits.h"

%include "cisstVector/vctDynamicVectorTypemaps.i"
%include "cisstVector/vctDynamicMatrixTypemaps.i"
%include "cisstVector/vctDynamicNArrayTypemaps.i"

%include "cisstVector/vctFixedSizeVectorTypemaps.i"
%include "cisstVector/vctFixedSizeMatrixTypemaps.i"

%include "cisstVector/vctFrame.i"
