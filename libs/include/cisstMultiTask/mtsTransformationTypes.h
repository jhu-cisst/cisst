/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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


/*!
  \file
  \brief Typedef for different transformations
*/


#ifndef _mtsTransformationTypes_h
#define _mtsTransformationTypes_h

// see vctTransformationTypes for naming conventions
#include <cisstVector/vctTransformationTypes.h>
#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstMultiTask/mtsMacros.h>

MTS_PROXY_CLASS_DECLARATION_FROM(vctDoubleQuat, mtsDoubleQuat);
typedef mtsDoubleQuat mtsQuat;
MTS_PROXY_CLASS_DECLARATION_FROM(vctFloatQuat, mtsFloatQuat);

MTS_PROXY_CLASS_DECLARATION_FROM(vctDoubleQuatRot3, mtsDoubleQuatRot3);
typedef mtsDoubleQuatRot3 mtsQuatRot3;
MTS_PROXY_CLASS_DECLARATION_FROM(vctFloatQuatRot3, mtsFloatQuatRot3);

MTS_PROXY_CLASS_DECLARATION_FROM(vctDoubleAxAnRot3, mtsDoubleAxAnRot3);
typedef mtsDoubleAxAnRot3 mtsAxAnRot3;
MTS_PROXY_CLASS_DECLARATION_FROM(vctFloatAxAnRot3, mtsFloatAxAnRot3);

MTS_PROXY_CLASS_DECLARATION_FROM(vctDoubleRodRot3, mtsDoubleRodRot3);
typedef mtsDoubleRodRot3 mtsRodRot3;
MTS_PROXY_CLASS_DECLARATION_FROM(vctFloatRodRot3, mtsFloatRodRot3);

MTS_PROXY_CLASS_DECLARATION_FROM(vctDoubleMatRot3, mtsDoubleMatRot3);
typedef mtsDoubleMatRot3 mtsMatRot3;
typedef mtsDoubleMatRot3 mtsDoubleRot3;
typedef mtsDoubleMatRot3 mtsRot3;
MTS_PROXY_CLASS_DECLARATION_FROM(vctFloatMatRot3, mtsFloatMatRot3);

MTS_PROXY_CLASS_DECLARATION_FROM(vctDoubleQuatFrm3, mtsDoubleQuatFrm3);
typedef mtsDoubleQuatFrm3 mtsQuatFrm3;
MTS_PROXY_CLASS_DECLARATION_FROM(vctFloatQuatFrm3, mtsFloatQuatFrm3);

MTS_PROXY_CLASS_DECLARATION_FROM(vctDoubleMatFrm3, mtsDoubleMatFrm3);
typedef mtsDoubleMatFrm3 mtsMatFrm3;
typedef mtsDoubleMatFrm3 mtsDoubleFrm3;
typedef mtsDoubleMatFrm3 mtsFrm3;
MTS_PROXY_CLASS_DECLARATION_FROM(vctFloatMatFrm3, mtsFloatMatFrm3);

MTS_PROXY_CLASS_DECLARATION_FROM(vctDoubleFrm4x4, mtsDoubleFrm4x4);
typedef mtsDoubleFrm4x4 mtsFrm4x4;
MTS_PROXY_CLASS_DECLARATION_FROM(vctFloatFrm4x4, mtsFloatFrm4x4);

MTS_PROXY_CLASS_DECLARATION_FROM(vctDoubleAnRot2, mtsDoubleAnRot2);
typedef mtsDoubleAnRot2 mtsAnRot2;
MTS_PROXY_CLASS_DECLARATION_FROM(vctFloatAnRot2, mtsFloatAnRot2);

MTS_PROXY_CLASS_DECLARATION_FROM(vctDoubleMatRot2, mtsDoubleMatRot2);
typedef mtsDoubleAnRot2 mtsAnRot2;
MTS_PROXY_CLASS_DECLARATION_FROM(vctFloatMatRot2, mtsFloatMatRot2);

#endif  // _mtsTransformationTypes_h

