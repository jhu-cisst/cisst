/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsStealthlinkControllerComponent.h 2835 2011-08-11 15:36:45Z wliu25 $

  Author(s): Peter Kazanzides, Anton Deguet
  Created on: 2006

  (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef MTSSTEALTHLINKCONTROLLERCOMPONENT_H
#define MTSSTEALTHLINKCONTROLLERCOMPONENT_H

#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

// Always include last
#include <cisstStealthlink/cisstStealthlinkExport.h>

class CISST_EXPORT mtsStealthlinkControllerComponent : public mtsComponent
{

    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION_ONEARG, CMN_LOG_ALLOW_DEFAULT);

public:


   mtsFunctionRead GetTool;
   mtsFunctionRead GetFrame;

   mtsFunctionRead GetMarkerCartesian;
   mtsFunctionRead GetPositionCartesian;

   mtsStealthlinkControllerComponent(const std::string &name);
   ~mtsStealthlinkControllerComponent() {};

};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsStealthlinkControllerComponent);

#endif // MTSSTEALTHLINKCONTROLLERCOMPONENT_H
