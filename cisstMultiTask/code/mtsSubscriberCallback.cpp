/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsSubscriberCallback.cpp 3303 2012-01-02 16:33:23Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2012-08-08

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#include <cisstMultiTask/mtsSubscriberCallback.h>

//#include "dict.h"

using namespace SF;
//using namespace SF::Dict;

CMN_IMPLEMENT_SERVICES(mtsSubscriberCallback);

mtsSubscriberCallback::mtsSubscriberCallback()
    : mtsGenericObject(), SFCallback()
{
}

mtsSubscriberCallback::~mtsSubscriberCallback()
{
}

void mtsSubscriberCallback::Callback(const std::string & json)
{
    CMN_LOG_CLASS_RUN_DEBUG << "mtsSubscriberCallback::Callback: " << json << std::endl;

    QueueAccess.Lock();
    {
        Messages.push_back(json); // FIFO
    }
    QueueAccess.Unlock();
}

void mtsSubscriberCallback::FetchMessages(MessagesType & messages)
{
    QueueAccess.Lock();
    {
        //messages.splice(messages.end(), Messages, Messages.begin(), Messages.end());
        messages.splice(messages.begin(), Messages, Messages.begin());
    }
    QueueAccess.Unlock();
}
