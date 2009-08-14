/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Anton Deguet
  Created on: 2009-08-13

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnLogger.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstMultiTask/mtsTaskManager.h>

int main(int CMN_UNUSED(argc), char ** CMN_UNUSED(argv))
{
    // log configuration, add a log per thread
    osaThreadedLogFile threadedLog("multiTaskNetworkManager");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_RUN_WARNING);
    // specify a higher, more verbose log level for these classes
    cmnClassRegister::SetLoD("mtsTaskManager", CMN_LOG_LOD_RUN_WARNING);

    // Get the TaskManager instance and set operation mode
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    taskManager->SetTaskManagerType(mtsTaskManager::TASK_MANAGER_SERVER);

    while (1) {
        osaSleep(10 * cmn_ms);
    }

    return 0;
}
