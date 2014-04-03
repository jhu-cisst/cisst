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

/* Qt dependent classes */
#include <cisstConfig.h>

#include <cisstMultiTask/mtsCollectorQtComponent.h>
CMN_IMPLEMENT_SERVICES(mtsCollectorQtComponent);

#include <cisstMultiTask/mtsCollectorQtFactory.h>
CMN_IMPLEMENT_SERVICES(mtsCollectorQtFactory);

#include <cisstMultiTask/mtsQtWidgetComponent.h>
CMN_IMPLEMENT_SERVICES(mtsQtWidgetComponent);

#include <cisstMultiTask/mtsQtWidgetInterfaceRequired.h>
CMN_IMPLEMENT_SERVICES(mtsQtWidgetInterfaceRequired);

#include <cisstMultiTask/mtsQtWidgetFunction.h>
CMN_IMPLEMENT_SERVICES(mtsQtWidgetFunction);

#include <cisstMultiTask/mtsQtWidgetEvent.h>
CMN_IMPLEMENT_SERVICES(mtsQtWidgetEvent);

#include <cisstMultiTask/mtsQtWidgetGenericObjectProxy.h>
MTS_QT_IMPLEMENT_SERVICES_AND_REGISTER_CREATORS(mtsInt, mtsQtWidgetIntRead, mtsQtWidgetIntWrite);
MTS_QT_IMPLEMENT_SERVICES_AND_REGISTER_CREATORS(mtsBool, mtsQtWidgetBoolRead, mtsQtWidgetBoolWrite);
MTS_QT_IMPLEMENT_SERVICES_AND_REGISTER_CREATORS(mtsDouble, mtsQtWidgetDoubleRead, mtsQtWidgetDoubleWrite);
MTS_QT_IMPLEMENT_SERVICES_AND_REGISTER_CREATORS(mtsStdString, mtsQtWidgetStdStringRead, mtsQtWidgetStdStringWrite);

#include <cisstMultiTask/mtsQtWidgetVectorTypes.h>
#include <cisstMultiTask/mtsVector.h>
MTS_QT_IMPLEMENT_SERVICES_AND_REGISTER_CREATORS(mtsDoubleVec, mtsQtWidgetDoubleVecRead, mtsQtWidgetDoubleVecWrite);
MTS_QT_IMPLEMENT_SERVICES_AND_REGISTER_CREATORS(mtsBoolVec, mtsQtWidgetBoolVecRead, mtsQtWidgetBoolVecWrite);

#include <cisstMultiTask/mtsStateIndex.h>
MTS_QT_IMPLEMENT_SERVICES_AND_REGISTER_CREATORS(mtsStateIndex, mtsQtWidgetStateIndexRead, mtsQtWidgetStateIndexWrite);
