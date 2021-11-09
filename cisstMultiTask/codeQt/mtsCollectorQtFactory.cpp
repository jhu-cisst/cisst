/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2014-03-03

  (C) Copyright 2014-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstMultiTask/mtsCollectorQtFactory.h>

#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsCollectorFactory.h>
#include <cisstMultiTask/mtsCollectorQtComponent.h>

mtsCollectorQtFactory::mtsCollectorQtFactory(const std::string & componentName):
    mtsTaskFromSignal(componentName)
{

}

void mtsCollectorQtFactory::Run(void)
{
    ProcessQueuedCommands();
    ProcessQueuedEvents();
}

void mtsCollectorQtFactory::Cleanup(void)
{

}

void mtsCollectorQtFactory::SetFactory(const std::string & collectorFactory)
{
    // check if we already have some collectors
    if (!mQtComponents.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "SetFactory: it seems that \"" << this->GetName()
                                 << "\" has already been associated to a collector factory." << std::endl;
        return;
    }

    // get access to component manager
    mtsComponentManager * manager = mtsComponentManager::GetInstance();

    // check that this name points to a collector factory
    mtsComponent * genericComponent = manager->GetComponent(collectorFactory);
    if (!genericComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "SetFactory: can't find generic component with name \""
                                 << collectorFactory << "\"" << std::endl;
        return;
    }
    mtsCollectorFactory * factoryComponent = dynamic_cast<mtsCollectorFactory *>(genericComponent);
    if (!factoryComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "SetFactory: found generic component with name \""
                                 << collectorFactory << "\" but it doesn't seem to be of type \"mtsCollectorFactory\"." << std::endl;
        return;
    }
    // for each collector, create a QtComponent
    typedef std::list<std::string> NameList;
    NameList collectors;
    factoryComponent->GetCollectorsNames(collectors);
    const NameList::const_iterator end = collectors.end();
    NameList::const_iterator iter;
    for (iter = collectors.begin();
         iter != end;
         ++iter) {
        const std::string qtComponentName = *iter + "::QtComponent";
        if (manager->GetComponent(qtComponentName)) {
            CMN_LOG_CLASS_INIT_ERROR << "SetFactory: found an existing component with the name \"" << qtComponentName
                                     << "\", unable to create and add mtsCollectorQtComponent." << std::endl;
        } else {
            mtsCollectorQtComponent * qtComponent = new mtsCollectorQtComponent(qtComponentName);
            manager->AddComponent(qtComponent);
            mQtComponents.insert(CollectorQtComponent(*iter, qtComponentName));
            CMN_LOG_CLASS_INIT_DEBUG << "SetFactory: created mtsCollectorQtComponent \""
                                     << qtComponentName << "\"." << std::endl;
        }
    }
}

void mtsCollectorQtFactory::Connect(void) const
{
    mtsComponentManager * manager = mtsComponentManager::GetInstance();
    CollectorQtComponents::const_iterator iter;
    const CollectorQtComponents::const_iterator end = mQtComponents.end();
    for (iter = mQtComponents.begin();
         iter != end;
         ++iter) {
        const std::string collectorName = (*iter).first;
        const std::string qtComponentName = (*iter).second;
        manager->Connect(qtComponentName, "DataCollection",
                         collectorName, "Control");
    }
}

void mtsCollectorQtFactory::ConnectToWidget(QWidget * widget) const
{
    mtsComponentManager * manager = mtsComponentManager::GetInstance();
    CollectorQtComponents::const_iterator iter;
    const CollectorQtComponents::const_iterator end = mQtComponents.end();
    for (iter = mQtComponents.begin();
         iter != end;
         ++iter) {
        const std::string qtComponentName = (*iter).second;
        mtsCollectorQtComponent * qtComponent =
            dynamic_cast<mtsCollectorQtComponent *>(manager->GetComponent(qtComponentName));
        if (qtComponent) {
            qtComponent->ConnectToWidget(widget);
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectToWidget: unable to find component \"" << qtComponentName
                                     << "\", can't connect to Qt widget." << std::endl;
        }
    }
}
