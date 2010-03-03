/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2010-02-26

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsRequiredInterface.h>

#include "mtsCollectorQComponent.h"

CMN_IMPLEMENT_SERVICES(mtsCollectorQComponent);


mtsCollectorQComponent::mtsCollectorQComponent(const std::string & taskName) :
    mtsDevice(taskName)
{
    // create the cisstMultiTask interface with commands and events
    mtsRequiredInterface * requiredInterface = AddRequiredInterface("DataCollection");
    if (requiredInterface) {
       requiredInterface->AddFunction("StartCollection", Collection.Start);
       requiredInterface->AddFunction("StopCollection", Collection.Stop);
    }
}


mtsCollectorQComponent::~mtsCollectorQComponent(void)
{
    CMN_LOG_CLASS_INIT_DEBUG << "~mtsCollectorQComponent: destructor has been called" << std::endl;
}


void mtsCollectorQComponent::ConnectToWidget(QWidget * widget)
{
    QObject::connect(widget, SIGNAL(StartCollection()),
                     this, SLOT(StartCollectionQSlot()));
    QObject::connect(widget, SIGNAL(StopCollection()),
                     this, SLOT(StopCollectionQSlot()));
}


void mtsCollectorQComponent::StartCollectionQSlot(void)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "StartCollectionQSlot: starting data collection" << std::endl;
    Collection.Start();

}


void mtsCollectorQComponent::StopCollectionQSlot(void)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "StopCollectionQSlot: stopping data collection" << std::endl;
    Collection.Stop();
}
