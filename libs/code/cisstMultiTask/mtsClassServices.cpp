/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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

#include <cisstMultiTask/mtsCollectorEvent.h>
CMN_IMPLEMENT_SERVICES(mtsCollectorEvent)

#include <cisstMultiTask/mtsCollectorState.h>
 CMN_IMPLEMENT_SERVICES(mtsCollectorState)

#include <cisstMultiTask/mtsComponent.h>
CMN_IMPLEMENT_SERVICES(mtsComponent)

#include <cisstMultiTask/mtsInterfaceProvidedOrOutput.h>
CMN_IMPLEMENT_SERVICES(mtsInterfaceProvidedOrOutput)

#include <cisstMultiTask/mtsInterfaceProvided.h>
CMN_IMPLEMENT_SERVICES(mtsInterfaceProvided)

#include <cisstMultiTask/mtsInterfaceRequired.h>
CMN_IMPLEMENT_SERVICES(mtsInterfaceRequired)

#include <cisstMultiTask/mtsStateIndex.h>
CMN_IMPLEMENT_SERVICES(mtsStateIndex)

#include <cisstMultiTask/mtsStateTable.h>
CMN_IMPLEMENT_SERVICES(mtsStateTable)
CMN_IMPLEMENT_SERVICES(mtsStateTableIndexRange)

#include <cisstMultiTask/mtsTask.h>
CMN_IMPLEMENT_SERVICES(mtsTask)

#include <cisstMultiTask/mtsTaskContinuous.h>
CMN_IMPLEMENT_SERVICES(mtsTaskContinuous)

#include <cisstMultiTask/mtsTaskFromCallback.h>
CMN_IMPLEMENT_SERVICES(mtsTaskFromCallback)
CMN_IMPLEMENT_SERVICES(mtsTaskFromCallbackAdapter)

#include <cisstMultiTask/mtsTaskFromSignal.h>
CMN_IMPLEMENT_SERVICES(mtsTaskFromSignal)

#include <cisstMultiTask/mtsTaskPeriodic.h>
CMN_IMPLEMENT_SERVICES(mtsTaskPeriodic)

#include <cisstMultiTask/mtsFixedSizeVectorTypes.h>
#define MTS_FIXED_SIZE_VECTOR_IMPLEMENT(ElementType)   \
CMN_IMPLEMENT_SERVICES_TEMPLATED(mts##ElementType##1); \
CMN_IMPLEMENT_SERVICES_TEMPLATED(mts##ElementType##2); \
CMN_IMPLEMENT_SERVICES_TEMPLATED(mts##ElementType##3); \
CMN_IMPLEMENT_SERVICES_TEMPLATED(mts##ElementType##4); \
CMN_IMPLEMENT_SERVICES_TEMPLATED(mts##ElementType##5); \
CMN_IMPLEMENT_SERVICES_TEMPLATED(mts##ElementType##6)
MTS_FIXED_SIZE_VECTOR_IMPLEMENT(Double)
MTS_FIXED_SIZE_VECTOR_IMPLEMENT(Float)
MTS_FIXED_SIZE_VECTOR_IMPLEMENT(Int)
MTS_FIXED_SIZE_VECTOR_IMPLEMENT(UInt)
MTS_FIXED_SIZE_VECTOR_IMPLEMENT(Char)
MTS_FIXED_SIZE_VECTOR_IMPLEMENT(UChar)
MTS_FIXED_SIZE_VECTOR_IMPLEMENT(Bool)

#include <cisstMultiTask/mtsGenericObjectProxy.h>
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDouble)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsLong)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsULong)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsInt)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUInt)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsShort)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUShort)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsChar)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUChar)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsBool)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsStdString)

#include <cisstMultiTask/mtsTransformationTypes.h>
CMN_IMPLEMENT_SERVICES(mtsDoubleQuat)
CMN_IMPLEMENT_SERVICES(mtsFloatQuat)
CMN_IMPLEMENT_SERVICES(mtsDoubleQuatRot3)
CMN_IMPLEMENT_SERVICES(mtsFloatQuatRot3)
CMN_IMPLEMENT_SERVICES(mtsDoubleAxAnRot3)
CMN_IMPLEMENT_SERVICES(mtsFloatAxAnRot3)
CMN_IMPLEMENT_SERVICES(mtsDoubleRodRot3)
CMN_IMPLEMENT_SERVICES(mtsFloatRodRot3)
CMN_IMPLEMENT_SERVICES(mtsDoubleMatRot3)
CMN_IMPLEMENT_SERVICES(mtsFloatMatRot3)
CMN_IMPLEMENT_SERVICES(mtsDoubleQuatFrm3)
CMN_IMPLEMENT_SERVICES(mtsFloatQuatFrm3)
CMN_IMPLEMENT_SERVICES(mtsDoubleMatFrm3)
CMN_IMPLEMENT_SERVICES(mtsFloatMatFrm3)
CMN_IMPLEMENT_SERVICES(mtsDoubleFrm4x4)
CMN_IMPLEMENT_SERVICES(mtsFloatFrm4x4)
CMN_IMPLEMENT_SERVICES(mtsDoubleAnRot2)
CMN_IMPLEMENT_SERVICES(mtsFloatAnRot2)
CMN_IMPLEMENT_SERVICES(mtsDoubleMatRot2)
CMN_IMPLEMENT_SERVICES(mtsFloatMatRot2)

#include <cisstMultiTask/mtsVector.h>
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDoubleVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsFloatVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsLongVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsULongVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsIntVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUIntVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsShortVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUShortVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsCharVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUCharVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsBoolVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsStdStringVec)

#include <cisstMultiTask/mtsMatrix.h>
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDoubleMat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsFloatMat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsLongMat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsULongMat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsIntMat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUIntMat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsShortMat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUShortMat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsCharMat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUCharMat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsBoolMat)

#include <cisstMultiTask/mtsHistory.h>
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDoubleHistory)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDoubleVecHistory)

#include <cisstMultiTask/mtsManagerGlobal.h>
CMN_IMPLEMENT_SERVICES(mtsManagerGlobal)
CMN_IMPLEMENT_SERVICES(mtsManagerGlobalInterface)

#include <cisstMultiTask/mtsManagerLocal.h>
CMN_IMPLEMENT_SERVICES(mtsManagerLocal)
CMN_IMPLEMENT_SERVICES(mtsManagerLocalInterface)


/* ICE dependent classes */
#include <cisstMultiTask/mtsConfig.h>
#if CISST_MTS_HAS_ICE

#include <cisstMultiTask/mtsComponentInterfaceProxyClient.h>
CMN_IMPLEMENT_SERVICES(mtsComponentInterfaceProxyClient)

#include <cisstMultiTask/mtsComponentInterfaceProxyServer.h>
CMN_IMPLEMENT_SERVICES(mtsComponentInterfaceProxyServer)

#include <cisstMultiTask/mtsComponentProxy.h>
CMN_IMPLEMENT_SERVICES(mtsComponentProxy)

#include <cisstMultiTask/mtsManagerProxyClient.h>
CMN_IMPLEMENT_SERVICES(mtsManagerProxyClient)

#include <cisstMultiTask/mtsManagerProxyServer.h>
CMN_IMPLEMENT_SERVICES(mtsManagerProxyServer)

#endif // CISST_MTS_HAS_ICE
