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
       requiredInterface->AddFunction("StartCollection", Collection.StartCollection);
       requiredInterface->AddFunction("StopCollection", Collection.StopCollection);
       requiredInterface->AddFunction("StartCollectionIn", Collection.StartCollectionIn);
       requiredInterface->AddFunction("StopCollectionIn", Collection.StopCollectionIn);
       requiredInterface->AddFunction("SetWorkingDirectory", Collection.SetWorkingDirectory);
       requiredInterface->AddFunction("SetOutputToDefault", Collection.SetOutputToDefault);
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
    QObject::connect(widget, SIGNAL(StartCollectionIn(double)),
                     this, SLOT(StartCollectionInQSlot(double)));
    QObject::connect(widget, SIGNAL(StopCollectionIn(double)),
                     this, SLOT(StopCollectionInQSlot(double)));
    QObject::connect(widget, SIGNAL(SetWorkingDirectory(QString)),
                     this, SLOT(SetWorkingDirectoryQSlot(QString)));
    QObject::connect(widget, SIGNAL(SetOutputToDefault()),
                     this, SLOT(SetOutputToDefaultQSlot()));
}


void mtsCollectorQComponent::StartCollectionQSlot(void)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "StartCollectionQSlot: starting data collection" << std::endl;
    Collection.StartCollection();
}


void mtsCollectorQComponent::StopCollectionQSlot(void)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "StopCollectionQSlot: stopping data collection" << std::endl;
    Collection.StopCollection();
}


void mtsCollectorQComponent::StartCollectionInQSlot(double delay)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "StartCollectionInQSlot: starting data collection in " << delay << "s" << std::endl;
    Collection.StartCollectionIn(mtsDouble(delay));
}


void mtsCollectorQComponent::StopCollectionInQSlot(double delay)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "StopCollectionInQSlot: stopping data collection in " << delay << "s" << std::endl;
    Collection.StopCollectionIn(mtsDouble(delay));
}


void mtsCollectorQComponent::SetWorkingDirectoryQSlot(QString directoryQt)
{
    mtsStdString directory(directoryQt.toStdString());
    Collection.SetWorkingDirectory(directory);
    Collection.SetOutputToDefault();
}


void mtsCollectorQComponent::SetOutputToDefaultQSlot(void)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "StopCollectionQSlot: set output to default" << std::endl;
    Collection.SetOutputToDefault();
}
