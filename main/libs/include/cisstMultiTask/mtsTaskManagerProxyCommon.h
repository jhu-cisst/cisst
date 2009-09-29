/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsTaskManagerProxyCommon.h 142 2009-03-11 23:02:34Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2009-03-17

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsTaskManagerProxyCommon_h
#define _mtsTaskManagerProxyCommon_h

#include <cisstMultiTask/mtsProxyBaseCommon.h>
#include <cisstMultiTask/mtsTaskManager.h>

class CISST_EXPORT mtsTaskManagerProxyCommon : public mtsProxyBaseCommon<mtsTaskManager> {
    
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);

public:
    mtsTaskManagerProxyCommon();
    ~mtsTaskManagerProxyCommon();
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsTaskManagerProxyCommon)

#endif // _mtsTaskManagerProxyCommon_h
