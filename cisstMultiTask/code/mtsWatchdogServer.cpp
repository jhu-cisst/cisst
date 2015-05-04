/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
 Author(s):  Marcin Balicki
 Created on: 2014

 (C) Copyright 2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsWatchdogServer.h>
#include <cisstMultiTask/mtsStateTable.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

// required to implement the class services, see cisstCommon
CMN_IMPLEMENT_SERVICES(mtsWatchdogServer);

void mtsWatchdogServer::AddToProvidedInterface(mtsInterfaceProvided &provInt, mtsStateTable &stateTable) {


     stateTable.AddData(WatchdogState, "WatchdogState");

     provInt.AddCommandReadState(stateTable,    WatchdogState,    "GetWatchdogState");
     provInt.AddCommandWriteState(stateTable,   WatchdogState,   "SetWatchdogState");
     provInt.AddCommandVoid(&mtsWatchdogServer::Reset,          this,   "WatchdogReset");

}
bool mtsWatchdogServer::CheckAndUpdate(void) {

    if (!IsOK)
        return false;
    //check if variable on the this end is false - i.e. it has been update by the client
    //if it is OK reset watchdogtimer and set the state to True so that client sees that we are alive

    if (WatchdogState == false) {
        Reset();
        return true;
    }
    //else check if time has exceeded timeout, if not return true, else return false;
    else if (StopWatch.GetElapsedTime() > Timeout) {
            IsOK = false;
            return false;
    }

    return true;
}

void mtsWatchdogServer::Reset(void) {
    WatchdogState = true;
    StopWatch.Reset();
    StopWatch.Start();
    IsOK = true;
}

void mtsWatchdogServer::Start(void) {
    Reset();
}

