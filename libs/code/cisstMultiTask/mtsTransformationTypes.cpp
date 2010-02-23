/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctTransformationTypes.h 75 2009-02-24 16:47:20Z adeguet1 $

  Author(s):	Anton Deguet
  Created on:	2009-07-16

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstMultiTask/mtsTransformationTypes.h>

CMN_IMPLEMENT_SERVICES(mtsDoubleQuat);
CMN_IMPLEMENT_SERVICES(mtsFloatQuat);

CMN_IMPLEMENT_SERVICES(mtsDoubleQuatRot3);
CMN_IMPLEMENT_SERVICES(mtsFloatQuatRot3);

CMN_IMPLEMENT_SERVICES(mtsDoubleAxAnRot3);
CMN_IMPLEMENT_SERVICES(mtsFloatAxAnRot3);

CMN_IMPLEMENT_SERVICES(mtsDoubleRodRot3);
CMN_IMPLEMENT_SERVICES(mtsFloatRodRot3);

CMN_IMPLEMENT_SERVICES(mtsDoubleMatRot3);
CMN_IMPLEMENT_SERVICES(mtsFloatMatRot3);

CMN_IMPLEMENT_SERVICES(mtsDoubleQuatFrm3);
CMN_IMPLEMENT_SERVICES(mtsFloatQuatFrm3);

CMN_IMPLEMENT_SERVICES(mtsDoubleMatFrm3);
CMN_IMPLEMENT_SERVICES(mtsFloatMatFrm3);

CMN_IMPLEMENT_SERVICES(mtsDoubleFrm4x4);
CMN_IMPLEMENT_SERVICES(mtsFloatFrm4x4);

CMN_IMPLEMENT_SERVICES(mtsDoubleAnRot2);
CMN_IMPLEMENT_SERVICES(mtsFloatAnRot2);

CMN_IMPLEMENT_SERVICES(mtsDoubleMatRot2);
CMN_IMPLEMENT_SERVICES(mtsFloatMatRot2);

