/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsVector.cpp,v 1.3 2008/08/21 04:05:19 pkaz Exp $

  Author(s):	Anton Deguet
  Created on:   2008-02-05

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstMultiTask/mtsVector.h>

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDoubleVec);
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsFloatVec);
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsLongVec);
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsULongVec);
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsIntVec);
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUIntVec);
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsShortVec);
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUShortVec);
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsCharVec);
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUCharVec);
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsBoolVec);

#ifdef CISST_GETVECTOR
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtscmnDoubleVec);
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDoubleVecVec);
#endif

