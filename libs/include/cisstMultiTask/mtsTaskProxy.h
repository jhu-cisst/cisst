/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsDeviceProxy.h 291 2009-04-28 01:49:13Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2009-05-06

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines a periodic task.
*/

#ifndef _mtsDeviceProxy_h
#define _mtsDeviceProxy_h

#include <cisstMultiTask/mtsDevice.h>

class mtsDeviceProxy : public mtsDevice {

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);

public:
    mtsDeviceProxy(const std::string & taskName, double period);
    ~mtsDeviceProxy() {};

    void Configure() {}
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsDeviceProxy);

#endif // _mtsDeviceProxy_h

