/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsEventPublisher.h 3034 2011-10-09 01:53:36Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2012-09-12

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#ifndef _mtsEventPublisher_h
#define _mtsEventPublisher_h

#include "eventPublisherBase.h"

#include <cisstMultiTask/mtsFunctionWrite.h>

#include <cisstMultiTask/mtsExport.h>

class mtsMonitorComponent;

class CISST_EXPORT mtsEventPublisher: public SC::EventPublisherBase 
{
    //CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    // Event publishers that publish JSON-encoded events or faults to the
    // Safety Framework.

    /*! Event publisher for active filters */
    mtsFunctionWrite EventPublisher;
    /*! Event publisher for passive filters */
    mtsMonitorComponent * MonitorComponentInstance;

public:
    mtsEventPublisher(const SC::FilterBase::FilteringType filteringType);
    ~mtsEventPublisher() {}

    /*! Publish events to the Safety Framework */
    bool PublishEvent(const std::string & eventDescriptionJSON);

    //
    //  Active Filtering
    //
    /*! Returns event publisher for active filters */
    mtsFunctionWrite & GetEventPublisherFunction(void) { return EventPublisher; }

    //
    //  Passive Filtering
    //
    /*! Setter for passive filters */
    inline void SetMonitorComponentInstance(mtsMonitorComponent * instance) {
        MonitorComponentInstance = instance;
    }
    /*! Returns event publisher for passive filters */
    // MJ: Is there a better way to access mtsMonitorComponent::HandleFaultEvent()???
    //mtsMonitorComponent * GetEventPublisherInstance(void) const { return MonitorComponentInstance; }

    /*
    void ToStream(std::ostream & outputStream) const {}
    void SerializeRaw(std::ostream & outputStream) const {}
    void DeSerializeRaw(std::istream & inputStream) {}
    */
};

//CMN_DECLARE_SERVICES_INSTANTIATION(mtsEventPublisher);

#endif // _mtsEventPublisher_h
