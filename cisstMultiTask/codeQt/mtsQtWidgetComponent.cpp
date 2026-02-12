/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Praneeth Sadda, Anton Deguet
  Created on: 2011-11-11

  (C) Copyright 2011-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstMultiTask/mtsQtWidgetComponent.h>
#include <cisstMultiTask/mtsQtWidgetInterfaceRequired.h>

#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsManagerLocal.h>

#include <QVBoxLayout>

mtsQtWidgetComponent::mtsQtWidgetComponent(const std::string & name):
    QWidget(),
    mtsComponent(name)
{
    QLayout* layout = new QVBoxLayout();
    TabWidget = new QTabWidget();
    TabWidget->setMovable(true);
    layout->addWidget(TabWidget);
    setLayout(layout);
}


void mtsQtWidgetComponent::CreateWidgetsForComponent(const mtsComponent & component)
{
    // TODO: swap layout
    // TODO: execution component? This will have to be refreshed as well (Probably new/delete)
    typedef std::vector<std::string> ContainerType;
    ContainerType interfaceNames = component.GetNamesOfInterfacesProvided();
    ContainerType::const_iterator i;
    const ContainerType::const_iterator end = interfaceNames.end();

    CMN_LOG_CLASS_INIT_DEBUG << "CreateWidgetsForComponent: called for component \"" << component.GetName() << "\" ("
                             << interfaceNames.size() << " provided interface(s) found)" << std::endl;

    for (i = interfaceNames.begin(); i < end; ++i) {
        // skip "system" interfaces
        if (*i == "InterfaceInternalProvided") {
            CMN_LOG_CLASS_INIT_DEBUG << "CreateWidgetsForComponent: skipping interface \"InterfaceInternalProvided\"" << std::endl;
        } else {
            size_t found = i->find("StateTable", 0, 10 /* 10 chars in StateTable */);
            if (found != std::string::npos) {
                CMN_LOG_CLASS_INIT_DEBUG << "CreateWidgetsForComponent: skipping interface \"StateTable\": " << *i << std::endl;
            } else {
                CreateWidgetsForInterface(component, *i);
            }
        }
    }
}


bool mtsQtWidgetComponent::CreateWidgetsForComponent(const std::string & componentName)
{
    // this is implemented the way around, we should keep strings as long as possible instead of getting a pointer
    mtsComponent * component = mtsComponentManager::GetInstance()->GetComponent(componentName);
    if (component) {
        this->CreateWidgetsForComponent(*component);
        return true;
    }
    CMN_LOG_CLASS_INIT_WARNING << "CreateWidgetsForComponent: can't find component \"" << componentName << "\"" << std::endl;
    return false;
}


void mtsQtWidgetComponent::CreateWidgetsForInterface(const mtsComponent & component, const std::string & interfaceName)
{
    CMN_LOG_CLASS_INIT_DEBUG << "CreateWidgetsForInterface: called for component \"" << component.GetName()
                             << "\", interface \"" << interfaceName << "\"" << std::endl;
    mtsInterfaceProvided * interfaceProvided = component.GetInterfaceProvided(interfaceName);
    const std::string & interfaceFullName = interfaceProvided->GetFullName();
    mtsInterfaceRequired * requiredInterface = this->AddInterfaceRequired(interfaceFullName);
    int tabIndex = TabWidget->addTab(new mtsQtWidgetInterfaceRequired(interfaceProvided, requiredInterface),
                                     interfaceFullName.c_str());
    TabWidget->setTabToolTip(tabIndex, interfaceFullName.c_str());
}
