/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2010-03-19

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnDeSerializer.h>
CMN_IMPLEMENT_SERVICES(cmnDeSerializer);

#include <cisstCommon/cmnGenericObjectProxy.h>
CMN_IMPLEMENT_SERVICES_TEMPLATED(cmnDouble);
CMN_IMPLEMENT_SERVICES_TEMPLATED(cmnLong);
CMN_IMPLEMENT_SERVICES_TEMPLATED(cmnULong);
CMN_IMPLEMENT_SERVICES_TEMPLATED(cmnLongLong);
CMN_IMPLEMENT_SERVICES_TEMPLATED(cmnULongLong);
CMN_IMPLEMENT_SERVICES_TEMPLATED(cmnInt);
CMN_IMPLEMENT_SERVICES_TEMPLATED(cmnUInt);
CMN_IMPLEMENT_SERVICES_TEMPLATED(cmnShort);
CMN_IMPLEMENT_SERVICES_TEMPLATED(cmnUShort);
CMN_IMPLEMENT_SERVICES_TEMPLATED(cmnChar);
CMN_IMPLEMENT_SERVICES_TEMPLATED(cmnUChar);
CMN_IMPLEMENT_SERVICES_TEMPLATED(cmnBool);

#include <cisstCommon/cmnPath.h>
CMN_IMPLEMENT_SERVICES(cmnPath);

#include <cisstCommon/cmnSerializer.h>
CMN_IMPLEMENT_SERVICES(cmnSerializer);
