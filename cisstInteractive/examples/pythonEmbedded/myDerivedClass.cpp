/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
 
  Author(s): Anton Deguet
  Created on: 2004-10-05

  (C) Copyright 2004-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "myDerivedClass.h"
#include <cisstVector/vctRandom.h>

CMN_IMPLEMENT_SERVICES(myDerivedClass);


myDerivedClass::myDerivedClass(void) {
    vctRandom(this->FixedSizeVectorMember, 5.0, 10.0);
    this->DynamicVectorMember.SetSize(5);
    vctRandom(this->DynamicVectorMember, 0.0, 5.0);
}
