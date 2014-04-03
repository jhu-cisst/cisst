/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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

// Always include last
#include <cisstMultiTask/mtsExport.h>

MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(vctDoubleQuat, mtsDoubleQuat);
typedef mtsDoubleQuat mtsQuat;
MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(vctFloatQuat, mtsFloatQuat);

MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(vctDoubleQuatRot3, mtsDoubleQuatRot3);
typedef mtsDoubleQuatRot3 mtsQuatRot3;
MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(vctFloatQuatRot3, mtsFloatQuatRot3);

MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(vctDoubleAxAnRot3, mtsDoubleAxAnRot3);
typedef mtsDoubleAxAnRot3 mtsAxAnRot3;
MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(vctFloatAxAnRot3, mtsFloatAxAnRot3);

MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(vctDoubleRodRot3, mtsDoubleRodRot3);
typedef mtsDoubleRodRot3 mtsRodRot3;
MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(vctFloatRodRot3, mtsFloatRodRot3);

MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(vctDoubleMatRot3, mtsDoubleMatRot3);
typedef mtsDoubleMatRot3 mtsMatRot3;
typedef mtsDoubleMatRot3 mtsDoubleRot3;
typedef mtsDoubleMatRot3 mtsRot3;
MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(vctFloatMatRot3, mtsFloatMatRot3);

MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(vctDoubleQuatFrm3, mtsDoubleQuatFrm3);
typedef mtsDoubleQuatFrm3 mtsQuatFrm3;
MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(vctFloatQuatFrm3, mtsFloatQuatFrm3);

MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(vctDoubleMatFrm3, mtsDoubleMatFrm3);
typedef mtsDoubleMatFrm3 mtsMatFrm3;
typedef mtsDoubleMatFrm3 mtsDoubleFrm3;
typedef mtsDoubleMatFrm3 mtsFrm3;
MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(vctFloatMatFrm3, mtsFloatMatFrm3);

MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(vctDoubleFrm4x4, mtsDoubleFrm4x4);
typedef mtsDoubleFrm4x4 mtsFrm4x4;
MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(vctFloatFrm4x4, mtsFloatFrm4x4);

MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(vctDoubleAnRot2, mtsDoubleAnRot2);
typedef mtsDoubleAnRot2 mtsAnRot2;
MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(vctFloatAnRot2, mtsFloatAnRot2);

MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(vctDoubleMatRot2, mtsDoubleMatRot2);
typedef mtsDoubleAnRot2 mtsAnRot2;
MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(vctFloatMatRot2, mtsFloatMatRot2);

#endif  // _mtsTransformationTypes_h

