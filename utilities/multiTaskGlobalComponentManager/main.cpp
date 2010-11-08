/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Min Yang Jung
  Created on: 2010-01-20

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
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
#include <cisstMultiTask/mtsManagerGlobal.h>
#include <cisstMultiTask/mtsManagerLocal.h>

int main(int CMN_UNUSED(argc), char ** CMN_UNUSED(argv))
{
    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);
    // add a log per thread
    //osaThreadedLogFile threadedLog("GlobalComponentManager");
    //cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);
    // specify a higher, more verbose log level for these classes
    cmnClassRegister::SetLoD("mtsManagerGlobal", CMN_LOG_LOD_VERY_VERBOSE);

    // Create and start global component manager
    mtsManagerGlobal globalComponentManager;
    if (!globalComponentManager.StartServer()) {
        CMN_LOG_INIT_ERROR << "Failed to start global component manager." << std::endl;
        return 1;
    }
    CMN_LOG_INIT_VERBOSE << "Global component manager started..." << std::endl;

    // Get local component manager instance
    mtsManagerLocal * localManager;
    try {
        localManager = mtsManagerLocal::GetInstance(globalComponentManager);
    } catch (...) {
        CMN_LOG_INIT_ERROR << "Failed to initialize local component manager" << std::endl;
        return 1;
    }

    // create the tasks, i.e. find the commands
    localManager->CreateAll();
    // start the periodic Run
    localManager->StartAll();

    while (1) {
        osaSleep(100 * cmn_ms);
    }

    return 0;
}
