/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2009-11-08

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Header files from cisstMultiTask required to compile the SWIG generated Python wrappers
 */
#pragma once

#ifndef _mtsPython_h
#define _mtsPython_h

#include <cisstCommon/cmnPython.h>
#include <cisstVector/vctPython.h>
#include <cisstOSAbstraction/osaPython.h>

#include <cisstMultiTask/mtsCommandBase.h>
#include <cisstMultiTask/mtsCommandVoid.h>
#include <cisstMultiTask/mtsCommandVoidReturn.h>
#include <cisstMultiTask/mtsCommandRead.h>
#include <cisstMultiTask/mtsCommandWriteBase.h>
#include <cisstMultiTask/mtsCommandWriteReturn.h>
#include <cisstMultiTask/mtsEventReceiver.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsFunctionBase.h>
#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsFunctionVoidReturn.h>
#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstMultiTask/mtsFunctionWrite.h>
#include <cisstMultiTask/mtsFunctionWriteReturn.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsTask.h>
#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsTaskFromSignal.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsStateIndex.h>
#include <cisstMultiTask/mtsIntervalStatistics.h>
#include <cisstMultiTask/mtsCollectorBase.h>
#include <cisstMultiTask/mtsCollectorState.h>
#include <cisstMultiTask/mtsParameterTypes.h>
#include <cisstMultiTask/mtsManagerComponentServices.h>
#include <cisstMultiTask/mtsTransformationTypes.h>

#include <cisstMultiTask/mtsVector.h>
#include <cisstMultiTask/mtsMatrix.h>

// For IRE, because EnableDynamicComponentManagement is protected
// This could be instead be added to mtsComponent.h
// (also see mtsComponentWithManagement in cisstMultiTask.i)
class mtsComponentWithManagement : public mtsComponent
{
public:
    mtsComponentWithManagement(const std::string &name)
        : mtsComponent(name) { EnableDynamicComponentManagement(); }
    ~mtsComponentWithManagement() {}
};

#endif // _mtsPython_h
