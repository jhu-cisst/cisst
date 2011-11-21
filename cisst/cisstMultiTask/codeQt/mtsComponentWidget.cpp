/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Praneeth Sadda, Anton Deguet
  Created on: 2011-11-11

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstMultiTask/mtsComponentWidget.h>
#include <cisstMultiTask/mtsInterfaceRequiredWidget.h>

#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

#include <QVBoxLayout>

mtsComponentWidget::mtsComponentWidget(const mtsComponent * component, bool makeWindow):
    QWidget(),
    Component(component),
    ExecutionComponent(new mtsComponent("QtTesterComponent"))
{
    QLayout* layout = new QVBoxLayout();
    TabWidget = new QTabWidget();
    TabWidget->setMovable(true);
    layout->addWidget(TabWidget);
    setLayout(layout);
    UpdateUI(*component);
}


void mtsComponentWidget::UpdateUI(const mtsComponent & component)
{
    //TODO: swap layout
    //TODO: execution component? This will have to be refreshed as well (Probably new/delete)
    typedef std::vector<std::string> ContainerType;
    ContainerType interfaceNames = component.GetNamesOfInterfacesProvided();

    ContainerType::const_iterator i = interfaceNames.begin();
    const ContainerType::const_iterator end = interfaceNames.end();
    for ( ; i < end; ++i)
    {
        ExecutionComponent->AddInterfaceRequired(*i);
        mtsInterfaceRequired * requiredInterface = ExecutionComponent->GetInterfaceRequired(*i);
        TabWidget->addTab(new mtsInterfaceRequiredWidget(Component->GetInterfaceProvided(*i), requiredInterface), i->c_str());
    }
}
