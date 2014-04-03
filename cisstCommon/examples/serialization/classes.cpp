/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2007-04-08

  (C) Copyright 2007-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include "classes.h"
#include <cisstCommon/cmnSerializer.h>
#include <cisstCommon/cmnDeSerializer.h>

CMN_IMPLEMENT_SERVICES(classA);

void classA::SerializeRaw(std::ostream & outputStream) const {
    cmnSerializeRaw(outputStream, this->Value);
}

void classA::DeSerializeRaw(std::istream & inputStream) {
    cmnDeSerializeRaw(inputStream, this->Value);
}



CMN_IMPLEMENT_SERVICES(classB);

void classB::SerializeRaw(std::ostream & outputStream) const {
    cmnSerializeRaw(outputStream, this->Value);
}

void classB::DeSerializeRaw(std::istream & inputStream) {
    cmnDeSerializeRaw(inputStream, this->Value);
}

