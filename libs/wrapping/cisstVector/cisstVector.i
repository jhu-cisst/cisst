/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cisstVector.i,v 1.8 2008/03/13 20:44:21 anton Exp $

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

%import "cisstCommon/cisstCommon.i"

%header %{
    // Put header files here
    #include "cisstVector/cisstVector.i.h"
%}

%ignore *::operator[]; // We define __setitem__ and __getitem__
%ignore *::operator=; // Just to avoid warnings

// We don't use iterators in Python, so far.
%ignore *::begin;
%ignore *::end;
%ignore *::rbegin;
%ignore *::rend;
%ignore *::at;

// Generate parameter documentation for IRE
%feature("autodoc", "1");

// General approach for Python print
%rename(__str__) ToString;
%ignore *::ToStream;

// The traits are used everywhere
%include "cisstVector/vctContainerTraits.h"

// Define some basic flags
%include "cisstVector/vctForwardDeclarations.h" 

// Include per type of container
%include "cisstVector/vctDynamicVector.i"
%include "cisstVector/vctFixedSizeVector.i"

%include "cisstVector/vctDynamicMatrix.i"
%include "cisstVector/vctFixedSizeMatrix.i"

// %include "cisstVector/vctQuaternion.i"
// %include "cisstVector/vctQuaternionRotation3.i"
// %include "cisstVector/vctMatrixRotation3.i"
// %include "cisstVector/vctAxisAngleRotation3.i"
// %include "cisstVector/vctFrame.i"

