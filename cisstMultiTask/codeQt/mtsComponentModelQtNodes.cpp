/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2025-10-28

  (C) Copyright 2025 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsComponentModelQtNodes.h>
#include <QLabel>

mtsComponentModelQtNodes::mtsComponentModelQtNodes(mtsComponent * component):
    Component(component)
{
}

QString mtsComponentModelQtNodes::caption() const
{
    return QString::fromStdString(Component->GetName());
}

QString mtsComponentModelQtNodes::name() const
{
    return QString::fromStdString(Component->GetName());
}

unsigned int mtsComponentModelQtNodes::nPorts(QtNodes::PortType portType) const
{
    switch (portType) {
        case QtNodes::PortType::In:
            return Component->GetNamesOfInterfacesRequired().size();
        case QtNodes::PortType::Out:
            return Component->GetNamesOfInterfacesProvided().size();
        default:
            return 0;
    }
}

QtNodes::NodeDataType mtsComponentModelQtNodes::dataType(QtNodes::PortType portType,
                                                         QtNodes::PortIndex portIndex) const
{
    QtNodes::NodeDataType type;
    switch (portType) {
        case QtNodes::PortType::In: {
            auto names = Component->GetNamesOfInterfacesRequired();
            if (portIndex < names.size()) {
                type.id = QString::fromStdString(names[portIndex]);
                type.name = QString("Required: %1").arg(type.id);
            }
            break;
        }
        case QtNodes::PortType::Out: {
            auto names = Component->GetNamesOfInterfacesProvided();
            if (portIndex < names.size()) {
                type.id = QString::fromStdString(names[portIndex]);
                type.name = QString("Provided: %1").arg(type.id);
            }
            break;
        }
        default:
            break;
    }
    return type;
}

void mtsComponentModelQtNodes::setInData(std::shared_ptr<QtNodes::NodeData>,
                                     QtNodes::PortIndex)
{
    // Connections are managed by cisst, we don't need to do anything here
}

std::shared_ptr<QtNodes::NodeData> mtsComponentModelQtNodes::outData(QtNodes::PortIndex)
{
    // cisst manages the data flow, we don't need to do anything here
    return nullptr;
}

QWidget * mtsComponentModelQtNodes::embeddedWidget()
{
    // Create a simple label showing component state
    auto label = new QLabel(QString("State: %1")
                           .arg(QString::fromStdString(Component->GetState().HumanReadable())));
    label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    return label;
}
