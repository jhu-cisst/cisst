/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsSubscriberCallback.h 3034 2011-10-09 01:53:36Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2012-08-08

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#ifndef _mtsSubscriberCallback_h
#define _mtsSubscriberCallback_h

#include "baseIce.h"
//#include "json.h"
//#include "cisstMonitor.h"

#include <cisstOSAbstraction/osaMutex.h>
#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstMultiTask/mtsExport.h>

#include <list>

class CISST_EXPORT mtsSubscriberCallback: public mtsGenericObject, public SF::SFCallback
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    typedef std::list<std::string> MessagesType;

protected:
    osaMutex QueueAccess;
    MessagesType Messages;

public:
    mtsSubscriberCallback();
    ~mtsSubscriberCallback();

    /*! Called by Ice and push new message in json format to the internal queue. */
    void Callback(const std::string & json);

    /*! Check if the internal queue has any queued entry */
    inline bool IsEmptyQueue(void) const { return Messages.empty(); }

    /*! Called by cisst plug-ins (i.e., mtsMonitorComponent and mtsSafetySupervisor) 
        which owns Subscriber objects and move all entries in the internal queue to
        the buffer that the cisst plug-ins provide. */
    void FetchMessages(MessagesType & messages);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsSubscriberCallback);

#endif // _mtsSubscriberCallback_h
