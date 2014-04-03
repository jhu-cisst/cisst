/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2010-02-26

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsCollectorQtComponent.h>


mtsCollectorQtComponent::mtsCollectorQtComponent(const std::string & componentName) :
    mtsComponent(componentName)
{
    // create the cisstMultiTask interface with commands and events
    mtsInterfaceRequired * interfaceRequired = AddInterfaceRequired("DataCollection");
    if (interfaceRequired) {
       interfaceRequired->AddFunction("StartCollection", Collection.StartCollection);
       interfaceRequired->AddFunction("StopCollection", Collection.StopCollection);
       interfaceRequired->AddFunction("StartCollectionIn", Collection.StartCollectionIn);
       interfaceRequired->AddFunction("StopCollectionIn", Collection.StopCollectionIn);
       interfaceRequired->AddFunction("SetWorkingDirectory", Collection.SetWorkingDirectory);
       interfaceRequired->AddFunction("SetOutputToDefault", Collection.SetOutputToDefault);
       interfaceRequired->AddEventHandlerVoid(&mtsCollectorQtComponent::CollectionStartedHandler, this,
                                              "CollectionStarted");
       interfaceRequired->AddEventHandlerWrite(&mtsCollectorQtComponent::CollectionStoppedHandler, this,
                                               "CollectionStopped");
       interfaceRequired->AddEventHandlerWrite(&mtsCollectorQtComponent::ProgressHandler, this,
                                               "Progress");
    }
}


mtsCollectorQtComponent::~mtsCollectorQtComponent(void)
{
    CMN_LOG_CLASS_INIT_DEBUG << "~mtsCollectorQtComponent: destructor has been called" << std::endl;
}


void mtsCollectorQtComponent::CollectionStartedHandler(void)
{
    emit CollectionStartedQSignal();
}


void mtsCollectorQtComponent::CollectionStoppedHandler(const mtsUInt & count)
{
    emit CollectionStoppedQSignal(count.Data);
}


void mtsCollectorQtComponent::ProgressHandler(const mtsUInt & count)
{
    emit ProgressQSignal(count.Data);
}


void mtsCollectorQtComponent::ConnectToWidget(QWidget * widget) const
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
    QObject::connect(this, SIGNAL(CollectorAddedQSignal()),
                     widget, SLOT(CollectorAdded()));
    QObject::connect(this, SIGNAL(CollectionStartedQSignal()),
                     widget, SLOT(CollectionStarted()));
    QObject::connect(this, SIGNAL(CollectionStoppedQSignal(unsigned int)),
                     widget, SLOT(CollectionStopped(unsigned int)));
    QObject::connect(this, SIGNAL(ProgressQSignal(unsigned int)),
                     widget, SLOT(Progress(unsigned int)));
    emit CollectorAddedQSignal();

}


void mtsCollectorQtComponent::StartCollectionQSlot(void)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "StartCollectionQSlot: starting data collection" << std::endl;
    Collection.StartCollection();
}


void mtsCollectorQtComponent::StopCollectionQSlot(void)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "StopCollectionQSlot: stopping data collection" << std::endl;
    Collection.StopCollection();
}


void mtsCollectorQtComponent::StartCollectionInQSlot(double delay)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "StartCollectionInQSlot: starting data collection in " << delay << "s" << std::endl;
    Collection.StartCollectionIn(mtsDouble(delay));
}


void mtsCollectorQtComponent::StopCollectionInQSlot(double delay)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "StopCollectionInQSlot: stopping data collection in " << delay << "s" << std::endl;
    Collection.StopCollectionIn(mtsDouble(delay));
}


void mtsCollectorQtComponent::SetWorkingDirectoryQSlot(QString directoryQt)
{
    mtsStdString directory(directoryQt.toStdString());
    Collection.SetWorkingDirectory(directory);
    Collection.SetOutputToDefault();
}


void mtsCollectorQtComponent::SetOutputToDefaultQSlot(void)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "StopCollectionQSlot: set output to default" << std::endl;
    Collection.SetOutputToDefault();
}
