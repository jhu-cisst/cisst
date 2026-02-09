/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2025-10-28

  (C) Copyright 2025-2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <QLabel>
#include <cisstMultiTask/mtsComponentModelQtNodes.h>
#include <iostream>

mtsComponentModelQtNodes::mtsComponentModelQtNodes(const std::string &name)
    : m_name(name) {}

QString mtsComponentModelQtNodes::caption(void) const {
    return QString::fromStdString(m_name);
}

QString mtsComponentModelQtNodes::name(void) const {
    return QString::fromStdString(m_name);
}

unsigned int
mtsComponentModelQtNodes::nPorts(QtNodes::PortType portType) const {
    switch (portType) {
    case QtNodes::PortType::In:
        return m_interfaces_required.size();
    case QtNodes::PortType::Out:
        return m_interfaces_provided.size();
    default:
        return 0;
    }
}

QtNodes::NodeDataType
mtsComponentModelQtNodes::dataType(QtNodes::PortType portType,
                                   QtNodes::PortIndex portIndex) const {
    QtNodes::NodeDataType type;
    type.id = "cisst";
    switch (portType) {
    case QtNodes::PortType::In: {
        if (portIndex < m_interfaces_required.size()) {
            type.name = QString("[R] %1").arg(
                                              QString::fromStdString(m_interfaces_required[portIndex]));
        }
        break;
    }
    case QtNodes::PortType::Out: {
        if (portIndex < m_interfaces_provided.size()) {
            type.name = QString("[P] %1").arg(
                                              QString::fromStdString(m_interfaces_provided[portIndex]));
        }
        break;
    }
    default:
        break;
    }
    return type;
}

QString
mtsComponentModelQtNodes::portCaption(QtNodes::PortType portType,
                                      QtNodes::PortIndex portIndex) const {
    switch (portType) {
    case QtNodes::PortType::In:
        if (portIndex < m_interfaces_required.size()) {
            return QString::fromStdString(m_interfaces_required[portIndex]);
        }
        break;
    case QtNodes::PortType::Out:
        if (portIndex < m_interfaces_provided.size()) {
            return QString::fromStdString(m_interfaces_provided[portIndex]);
        }
        break;
    default:
        break;
    }
    return QString();
}

bool mtsComponentModelQtNodes::portCaptionVisible(QtNodes::PortType,
                                                  QtNodes::PortIndex) const {
    return true;
}

void mtsComponentModelQtNodes::setInData(std::shared_ptr<QtNodes::NodeData>,
                                         QtNodes::PortIndex) {
    // Connections are managed by cisst, we don't need to do anything here
}

std::shared_ptr<QtNodes::NodeData>
mtsComponentModelQtNodes::outData(QtNodes::PortIndex) {
    // cisst manages the data flow, we don't need to do anything here
    return nullptr;
}

QWidget *mtsComponentModelQtNodes::embeddedWidget(void) {
    if (!m_widget) {
        auto label = new QLabel(QString::fromStdString(m_name));
        label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
        m_widget = label;
    }
    return m_widget;
}

bool mtsComponentModelQtNodes::AddInterfaceProvided(const std::string &name) {
    m_interfaces_provided.push_back(name);
    return true;
}

bool mtsComponentModelQtNodes::AddInterfaceRequired(const std::string &name) {
    m_interfaces_required.push_back(name);
    return true;
}
