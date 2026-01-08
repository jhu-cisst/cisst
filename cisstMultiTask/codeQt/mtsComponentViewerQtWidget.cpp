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

#include <cisstMultiTask/mtsComponentViewerQtWidget.h>

#include <cisstMultiTask/mtsComponentModelQtNodes.h>
#include <cisstMultiTask/mtsManagerComponentServices.h>

#include <QtNodes/ConnectionStyle>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QVBoxLayout>
#include <QPointF>
#include <QVariant>

CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(mtsComponentViewerQtWidget, mtsTaskFromSignal, std::string);

mtsComponentViewerQtWidget::mtsComponentViewerQtWidget(const std::string & componentName):
    mtsTaskFromSignal(componentName)
{
    mtsInterfaceRequired * required = EnableDynamicComponentManagement();
    if (required) {
        ManagerComponentServices->AddComponentEventHandler(&mtsComponentViewerQtWidget::AddComponentHandler, this);
        // ManagerComponentServices->ChangeStateEventHandler(&mtsComponentViewerQtWidget::ChangeStateHandler, this);
        ManagerComponentServices->AddConnectionEventHandler(&mtsComponentViewerQtWidget::AddConnectionHandler, this);
        // ManagerComponentServices->RemoveConnectionEventHandler(&mtsComponentViewerQtWidget::RemoveConnectionHandler, this);
    } else {
        cmnThrow(std::runtime_error("mtsComponentViewer constructor: failed to enable dynamic component composition"));
    }

    setupUi();
}

void mtsComponentViewerQtWidget::Configure(const std::string & CMN_UNUSED(filename))
{
}

void mtsComponentViewerQtWidget::Startup(void)
{
    for (const auto & processName : ManagerComponentServices->GetNamesOfProcesses()) {
        std::cerr << "Process: " << processName << std::endl;
        for (const auto & componentName :
                 ManagerComponentServices->GetNamesOfComponents(processName)) {
            std::cerr << "  Component: " << componentName << std::endl;
        }
    }
}

void mtsComponentViewerQtWidget::Cleanup(void)
{
}

void mtsComponentViewerQtWidget::Run(void)
{
    ProcessQueuedCommands();
    ProcessQueuedEvents();
}

void mtsComponentViewerQtWidget::setupUi(void)
{
    Registry = std::make_shared<QtNodes::NodeDelegateModelRegistry>();
    GraphModel = new QtNodes::DataFlowGraphModel(Registry);
    Scene = new QtNodes::DataFlowGraphicsScene(*GraphModel);
    View = new QtNodes::GraphicsView(Scene);

    QVBoxLayout * layout = new QVBoxLayout();
    this->setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(View);

    QtNodes::ConnectionStyle::setConnectionStyle(
                                                 R"(
  {
    "ConnectionStyle": {
      "UseDataDefinedColors": true,
      "LineWidth": 2.0
    }
  }
  )");

    // Setup layout timer
    LayoutTimer = new QTimer(this);
    connect(LayoutTimer, &QTimer::timeout, this, &mtsComponentViewerQtWidget::updateLayout);
    LayoutRunning = true;
    LayoutIterations = 0;
    SetLayoutUpdateRate(1.0); // 1 Hz default
    show();
    LayoutTimer->start();
}

void mtsComponentViewerQtWidget::AddComponentHandler(const mtsDescriptionComponent & component_description)
{
    std::cerr << "Handler: added component: " << component_description << std::endl;
    // Create node model for this component
    const auto component_name = component_description.ComponentName;
    std::vector<std::string> interfacesRequired, interfacesProvided;
    ManagerComponentServices->GetNamesOfInterfaces(component_description.ProcessName, component_name,
                                                   interfacesRequired, interfacesProvided);
    std::cerr << "  Interfaces Required: " << interfacesRequired.size() << ", Provided: " << interfacesProvided.size() << std::endl;

    Registry->registerModel<mtsComponentModelQtNodes>(
                                                      [component_name, interfacesRequired, interfacesProvided]() {
                                                          auto model = std::make_unique<mtsComponentModelQtNodes>(component_name);
                                                          for (const auto & interfaceName : interfacesRequired) {
                                                              model->AddInterfaceRequired(interfaceName);
                                                          }
                                                          for (const auto & interfaceName : interfacesProvided) {
                                                              model->AddInterfaceProvided(interfaceName);
                                                          }
                                                          return model;
                                                      }
                                                      );
    
    // Add node to graph
    QtNodes::NodeId nodeId = GraphModel->addNode(QString::fromStdString(component_name));
    std::cerr << "  Added node to graph: " << component_name << " ID: " << nodeId << std::endl;

    // Add to layout engine
    Layout.AddNode(component_name);

    m_components.push_back(component_name);
    NodeIds[component_name] = nodeId;

    // Update layout
    if (LayoutRunning) {
        updateNodePositions();
    }
}

void mtsComponentViewerQtWidget::AddConnectionHandler(const mtsDescriptionConnection & connection_description)
{
    std::cerr << "Handler: added connection: " << connection_description << std::endl;
}

void mtsComponentViewerQtWidget::updateLayout(void)
{
    // Single layout step and update node positions
    Layout.Step();
    updateNodePositions();
}

void mtsComponentViewerQtWidget::SetLayoutUpdateRate(double rateHz)
{
    if (!LayoutTimer) return;
    if (rateHz <= 0.0) {
        LayoutTimer->stop();
        return;
    }
    int intervalMs = static_cast<int>(1000.0 / rateHz);
    LayoutTimer->setInterval(intervalMs);
}

void mtsComponentViewerQtWidget::updateNodePositions(void)
{
    // Move nodes in the graph model to match layout positions
    for (auto component : m_components) {
        auto it = NodeIds.find(component);
        if (it == NodeIds.end()) continue;
        unsigned int nid = it->second;
        auto pos = Layout.GetPosition(component);
        // set the NodeRole::Position role on the graph model
        std::cerr << component << " x: " << pos.x() << " y: " << pos.y() << std::endl;
        GraphModel->setNodeData(nid, QtNodes::NodeRole::Position, QVariant::fromValue(QPointF(pos.x(), pos.y())));
    }
}
