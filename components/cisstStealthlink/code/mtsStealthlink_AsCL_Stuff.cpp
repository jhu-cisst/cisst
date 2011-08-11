/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Peter Kazanzides, Anton Deguet
  Created on: 2006

  (C) Copyright 2007-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <iostream>
#include <cisstOSAbstraction/osaStopwatch.h>
#include "mtsStealthlink_AsCL_Stuff.h"

//------------------------------------------------------------------------------
mtsStealthlink_AsCL_IO_Watch::mtsStealthlink_AsCL_IO_Watch(void) : Callback(0)
{
}


mtsStealthlink_AsCL_IO_Watch::~mtsStealthlink_AsCL_IO_Watch() {
    RemoveWatch();
}


int mtsStealthlink_AsCL_IO_Watch::AddWatch(int sock, void * func, void * client_ptr)
{
    if (Callback)
        CMN_LOG_INIT_WARNING << "Stealthlink AddWatch: callback was already specified" << std::endl;
    else
        CMN_LOG_INIT_VERBOSE << "Stealthlink AddWatch called for socket fd = " << sock << std::endl;

    fd = sock;
    Callback = (Watch_Callback)func;
    ClientPtr = client_ptr;
    return 1;
}


void mtsStealthlink_AsCL_IO_Watch::RemoveWatch(void)
{
    CMN_LOG_INIT_VERBOSE << "Stealthlink RemoveWatch called" << std::endl;
    Callback = 0;
}

void mtsStealthlink_AsCL_IO_Watch::CheckWatch(void)
{
    if (Callback) {
        fd_set readfds;
        FD_ZERO( &readfds);
        FD_SET( fd, &readfds);
        timeval timeout = { 0L , 0L };
        int ret = select( fd+1, &readfds, NULL, NULL, &timeout);
        if (ret < 0) {
            CMN_LOG_RUN_ERROR << "Stealthlink CheckWatch: select failed, rc = " << ret << std::endl;
            Callback = 0;
        }
        else if ((ret > 0) && FD_ISSET(fd, &readfds))
            Callback(ClientPtr);
    }
}


//------------------------------------------------------------------------------
//   NOTE: the NoGui_AsCL_Timeout class from Medtronic has no implementation
//         (it was commented out), so probably this class is no longer needed.

mtsStealthlink_AsCL_Timeout::mtsStealthlink_AsCL_Timeout(void) : Callback(0)
{
    StealthlinkTimer = new osaStopwatch;
    StealthlinkTimer->Reset();
}


mtsStealthlink_AsCL_Timeout::~mtsStealthlink_AsCL_Timeout(void)
{
    RemoveTimeout();
    delete StealthlinkTimer;
}

// Units for tmo_val are not known.  FLTK implementation by PK assumed milliseconds.
// NoGUI implementation from Medtronic has commented out the implementation, but
// the commented-out code seems to assume microseconds.

int mtsStealthlink_AsCL_Timeout::AddTimeout(int tmo_val, void * func, void * obj)
{
    if (Callback)
        CMN_LOG_INIT_WARNING << "Stealthlink AddTimeout: callback was already specified" << std::endl;
#if 0
    else
        CMN_LOG_INIT_VERBOSE << "Stealthlink AddTimeout called for timeout = " << tmo_val << std::endl;
#endif

    Timeout = tmo_val / 1000.0;
    Callback = (Timeout_Callback) func;
    DataObj = obj;

    // Start timer  
    StealthlinkTimer->Reset();
    StealthlinkTimer->Start();

    return 1;
}


void mtsStealthlink_AsCL_Timeout::RemoveTimeout(void)
{
    CMN_LOG_INIT_VERBOSE << "Stealthlink RemoveTimeout called" << std::endl;
    Callback = 0;
    StealthlinkTimer->Reset();
}

void mtsStealthlink_AsCL_Timeout::CheckTimeout(void)
{
    // if callback function returns non-zero (true), reschedule timeout.
    if (Callback && (StealthlinkTimer->GetElapsedTime() >= Timeout)) {
        StealthlinkTimer->Reset();
        int rc = Callback(DataObj);
#if 0
        CMN_LOG_RUN_VERBOSE << "Stealthlink Timeout callback returned " << rc << std::endl;
#endif
        if (rc)  // reschedule
            StealthlinkTimer->Start();
        else
            Callback = 0;
    }
}


//------------------------------------------------------------------------------
mtsStealthlink_AsCL_Utils::mtsStealthlink_AsCL_Utils(void) : curWatch(0), curTimeout(0)
{}


mtsStealthlink_AsCL_Utils::~mtsStealthlink_AsCL_Utils(void)
{}


AsCL_IO_Watch * mtsStealthlink_AsCL_Utils::new_IO_Watch(void)
{
    if (curWatch)
        CMN_LOG_INIT_WARNING << "Stealthlink IO_Watch already allocated" << std::endl;

    curWatch = new mtsStealthlink_AsCL_IO_Watch;
    return curWatch;
}


AsCL_Timeout * mtsStealthlink_AsCL_Utils::new_Timeout(void)
{
    if (curTimeout)
        CMN_LOG_INIT_WARNING << "Stealthlink Timeout already allocated" << std::endl;

    curTimeout = new mtsStealthlink_AsCL_Timeout;
    return curTimeout;
}

void mtsStealthlink_AsCL_Utils::CheckCallbacks(void)
{
    if (curWatch)
        curWatch->CheckWatch();
    if (curTimeout)
        curTimeout->CheckTimeout();
}
