/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cisstStealthlink.i 2772 2011-07-22 17:40:07Z wliu25 $

  Author(s):	Anton Deguet
  Created on:   2009-01-26

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


%module cisstStealthlinkPython


%include "std_list.i"
%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%include "std_pair.i"
%include "std_streambuf.i"
%include "std_iostream.i"

%include "swigrun.i"

%import "cisstConfig.h"
%import "cisstDevices/devConfig.h"

%import "cisstCommon/cisstCommon.i"
%import "cisstVector/cisstVector.i"
%import "cisstMultiTask/cisstMultiTask.i"
%import "cisstOSAbstraction/cisstOSAbstraction.i"
%import "cisstParameterTypes/cisstParameterTypes.i"

%init %{
    import_array() // numpy initialization
%}

%header %{
#include <cisstStealthlink/cisstStealthlinkPython.h>
%}

// Generate parameter documentation for IRE
%feature("autodoc", "1");

%rename(__str__) ToString;
%ignore *::ToStream;
%ignore operator<<;
%ignore operator=;

%ignore *::operator[]; // We define __setitem__ and __getitem__

#define CISST_EXPORT
#define CISST_DEPRECATED

#if CISST_HAS_XML
    %include "cisstStealthlink/mtsStealthlinkTypes.h"
    %include "cisstStealthlink/mtsStealthlink.h"
#endif

%include "cisstStealthlink/mtsStealthlinkControllerComponent.h"


