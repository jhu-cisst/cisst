/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Gorkem Sevinc, Anton Deguet
  Created on:	2009-08-27

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/
#ifndef _prmCollaborativeControlForce_h
#define _prmCollaborativeControlForce_h

#include <cisstMultiTask/mtsTaskFromCallback.h>
#include <cisstMultiTask/mtsMacros.h>
#include <cisstRobot/robCollaborativeControlForce.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>

#error "This class is deprecated, do not use"

MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(robCollaborativeControlForce::ParameterType, prmCollaborativeControlForce);

#endif
