/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2010-11-06

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsProxyConfig_h
#define _mtsProxyConfig_h

#include <cisstCommon/cmnUnits.h>

#define IMPROVE_ICE_THREADING 1 // MJ TEMP

namespace mtsProxyConfig
{
    // After the GCM issues a new connection id, a LCM should inform the GCM
    // that the connection is successfully established within this period time.
    // Otherwise, the GCM will invalidate the connection and send connection 
    // failure message to LCMs that are involved in the connection.
    // (See mtsManagerGlobal.h)
    const double ConnectConfirmTimeOut = 15.0 * cmn_s;

    // Connection refresh period (heart-beat period) for connections between the
    // GCM and LCM
    const double RefreshPeriodForManagers = 1.0 * cmn_s;

    // Connection refresh period (heart-beat period) for connections between 
    // a provided interface and a required interface
    const double RefreshPeriodForInterfaces = 1.0 * cmn_s;

    // Connection check period to see if manager client proxies connected (of 
    // type mtsManagerProxyClient) are valid
    const double CheckPeriodForManagerConnections = 1.5 * RefreshPeriodForManagers;

    // Connection check period to see if interface client proxies connected (of 
    // type mtsComponentInterfaceProxyClient) are valid
    const double CheckPeriodForInterfaceConnections = 1.5 * RefreshPeriodForInterfaces;

    // Globally disable connection monitoring thread.  
    // MJ: Use this option at your own risk because this is the bare minimum 
    // to guarantee "healthy" connections.  Also note that setting this flag 
    // true will disables the entire disconnection detection mechanisms as well.
    const bool DisableConnectionMonitoring = false;
};

#endif // _mtsProxyConfig_h
