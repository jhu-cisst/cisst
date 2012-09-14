/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsEventPublisher.cpp 3303 2012-01-02 16:33:23Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2012-09-12

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#include <cisstMultiTask/mtsEventPublisher.h>

//CMN_IMPLEMENT_SERVICES(mtsEventPublisher);

mtsEventPublisher::mtsEventPublisher(): SF::EventPublisherBase()
{
}

bool mtsEventPublisher::PublishEvent(const std::string & eventDescriptionJSON)
{
    if (!EventPublisher.IsValid()) {
        //CMN_LOG_CLASS_RUN_ERROR << "PublishEvent: Failed to publish due to invalid event publisher.  JSON [ " << eventDescriptionJSON << " ]" << std::endl;
        return false;
    }

    std::cout << EventPublisher(eventDescriptionJSON) << std::endl;
    std::cout << eventDescriptionJSON << std::endl;

    return true;
}
