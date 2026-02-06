/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab:
 */

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
#include <cisstMultiTask/mtsComponentViewerQtWidget.h>

#include <cisstMultiTask/mtsManagerComponentServices.h>

#include <QAction>
#include <QFileDialog>
#include <QLabel>
#include <QMenuBar>
#include <QToolBar>
#include <QVBoxLayout>

#include <QtNodes/ConnectionStyle>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeDelegateModelRegistry>

#define GVC_EXPORTS 1
#include <cgraph.h>
#include <gvc.h>

CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(mtsComponentViewerQtWidget,
                                      mtsTaskFromSignal, std::string);

mtsComponentViewerQtWidget::mtsComponentViewerQtWidget(
    const std::string &componentName)
    : mtsTaskFromSignal(componentName) {
  // Register NodeId and ConnectionId for Qt signal/slot connections
  qRegisterMetaType<QtNodes::NodeId>("NodeId");
  qRegisterMetaType<QtNodes::ConnectionId>("ConnectionId");

  mtsInterfaceRequired *required = EnableDynamicComponentManagement();
  if (required) {
    ManagerComponentServices->AddComponentEventHandler(
        &mtsComponentViewerQtWidget::AddComponentHandler, this);
    // ManagerComponentServices->ChangeStateEventHandler(&mtsComponentViewerQtWidget::ChangeStateHandler,
    // this);
    ManagerComponentServices->AddConnectionEventHandler(
        &mtsComponentViewerQtWidget::AddConnectionHandler, this);
    // ManagerComponentServices->RemoveConnectionEventHandler(&mtsComponentViewerQtWidget::RemoveConnectionHandler,
    // this);
  } else {
    cmnThrow(std::runtime_error("mtsComponentViewer constructor: failed to "
                                "enable dynamic component composition"));
  }

  setupUi();
}

void mtsComponentViewerQtWidget::Configure(
    const std::string &CMN_UNUSED(filename)) {}

void mtsComponentViewerQtWidget::Startup(void) {
  for (const auto &processName :
       ManagerComponentServices->GetNamesOfProcesses()) {
    std::cerr << "Process: " << processName << std::endl;
    for (const auto &componentName :
         ManagerComponentServices->GetNamesOfComponents(processName)) {
      std::cerr << "  Component: " << componentName << std::endl;
    }
  }
}

void mtsComponentViewerQtWidget::Cleanup(void) {}

void mtsComponentViewerQtWidget::Run(void) {
  ProcessQueuedCommands();
  ProcessQueuedEvents();
}

void mtsComponentViewerQtWidget::setupUi(void) {
  Registry = std::make_shared<QtNodes::NodeDelegateModelRegistry>();
  GraphModel = new QtNodes::DataFlowGraphModel(Registry);
  Scene = new QtNodes::DataFlowGraphicsScene(*GraphModel);
  View = new QtNodes::GraphicsView(Scene);

  QVBoxLayout *layout = new QVBoxLayout();
  this->setLayout(layout);
  layout->setContentsMargins(0, 0, 0, 0);

  layout->addWidget(View);

  View->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(View, &QWidget::customContextMenuRequested, this,
          &mtsComponentViewerQtWidget::onContextMenuRequested);

  QtNodes::ConnectionStyle::setConnectionStyle(
      R"(
  {
    "ConnectionStyle": {
      "UseDataDefinedColors": true,
      "LineWidth": 2.0
    }
  }
  )");

  show();
}

void mtsComponentViewerQtWidget::onContextMenuRequested(const QPoint &pos) {
  QMenu contextMenu(this);
  QAction *actionAutoLayout = contextMenu.addAction(tr("Auto-Layout"));
  QAction *actionExportDOT = contextMenu.addAction(tr("Export as DOT..."));

  connect(actionAutoLayout, &QAction::triggered, this,
          &mtsComponentViewerQtWidget::onAutoLayout);
  connect(actionExportDOT, &QAction::triggered, this,
          &mtsComponentViewerQtWidget::onExportDOT);

  contextMenu.exec(View->mapToGlobal(pos));
}

void mtsComponentViewerQtWidget::onExportDOT(void) {
  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Export as DOT"), "", tr("DOT Files (*.dot);;All Files (*)"));
  if (fileName.isEmpty())
    return;

  GVC_t *gvc = gvContext();
  Agraph_t *g = agopen((char *)"G", Agdirected, nullptr);

  // Map nodes
  std::map<QtNodes::NodeId, Agnode_t *> graphvizNodes;
  for (const auto &component : m_components) {
    QtNodes::NodeId id = NodeIds[component];
    Agnode_t *v = agnode(g, (char *)component.c_str(), 1);
    graphvizNodes[id] = v;
  }

  // Map edges
  for (const auto &component : m_components) {
    QtNodes::NodeId id = NodeIds[component];
    const auto &portMap = m_provided_interface_to_port[component];
    for (const auto &portPair : portMap) {
      QtNodes::PortIndex outPortIndex = portPair.second;
      for (const auto &connId :
           GraphModel->connections(id, QtNodes::PortType::Out, outPortIndex)) {
        Agnode_t *u = graphvizNodes[connId.outNodeId];
        Agnode_t *v = graphvizNodes[connId.inNodeId];
        agedge(g, u, v, nullptr, 1);
      }
    }
  }

  FILE *fp = fopen(fileName.toStdString().c_str(), "w");
  if (fp) {
    agwrite(g, fp);
    fclose(fp);
  }

  agclose(g);
  gvFreeContext(gvc);
}

void mtsComponentViewerQtWidget::onAutoLayout(void) {
  GVC_t *gvc = gvContext();
  Agraph_t *g = agopen((char *)"G", Agdirected, nullptr);

  // Set graph attributes for left-to-right
  agsafeset(g, (char *)"rankdir", (char *)"LR", (char *)"LR");
  agsafeset(g, (char *)"nodesep", (char *)"0.5", (char *)"0.5");
  agsafeset(g, (char *)"ranksep", (char *)"1.0", (char *)"1.0");

  std::map<QtNodes::NodeId, Agnode_t *> graphvizNodes;
  for (const auto &component : m_components) {
    QtNodes::NodeId id = NodeIds[component];
    Agnode_t *v = agnode(g, (char *)component.c_str(), 1);
    // Rough size estimate for Graphviz
    agsafeset(v, (char *)"width", (char *)"2.0", (char *)"");
    agsafeset(v, (char *)"height", (char *)"1.0", (char *)"");
    agsafeset(v, (char *)"fixedsize", (char *)"true", (char *)"");
    graphvizNodes[id] = v;
  }

  for (const auto &component : m_components) {
    QtNodes::NodeId id = NodeIds[component];
    const auto &portMap = m_provided_interface_to_port[component];
    for (const auto &portPair : portMap) {
      QtNodes::PortIndex outPortIndex = portPair.second;
      for (const auto &connId :
           GraphModel->connections(id, QtNodes::PortType::Out, outPortIndex)) {
        Agnode_t *u = graphvizNodes[connId.outNodeId];
        Agnode_t *v = graphvizNodes[connId.inNodeId];
        agedge(g, u, v, nullptr, 1);
      }
    }
  }

  // Run layout
  gvLayout(gvc, g, "dot");

  // Apply positions
  for (const auto &component : m_components) {
    QtNodes::NodeId id = NodeIds[component];
    Agnode_t *v = graphvizNodes[id];
    // Graphviz uses points (1/72 inch), typical screen is 96 DPI
    double x = ND_coord(v).x;
    double y = ND_coord(v).y;
    GraphModel->setNodeData(id, QtNodes::NodeRole::Position, QPointF(x, -y));
  }

  gvFreeLayout(gvc, g);
  agclose(g);
  gvFreeContext(gvc);
}

void mtsComponentViewerQtWidget::AddComponentHandler(
    const mtsDescriptionComponent &component_description) {
  std::cerr << "Handler: added component: " << component_description
            << std::endl;

  const auto component_name = component_description.ComponentName;

  // Query component interfaces
  std::vector<std::string> interfacesRequired, interfacesProvided;
  ManagerComponentServices->GetNamesOfInterfaces(
      component_description.ProcessName, component_name, interfacesRequired,
      interfacesProvided);
  std::cerr << "  Interfaces Required: " << interfacesRequired.size()
            << ", Provided: " << interfacesProvided.size() << std::endl;

  // Use invokeMethod to ensure GUI updates happen in the GUI thread
  QMetaObject::invokeMethod(
      this,
      [=]() {
        // Register a node model for this component with its interfaces
        Registry->registerModel<mtsComponentModelQtNodes>(
            [component_name, interfacesRequired, interfacesProvided]() {
              auto model =
                  std::make_unique<mtsComponentModelQtNodes>(component_name);
              for (const auto &interfaceName : interfacesRequired) {
                model->AddInterfaceRequired(interfaceName);
              }
              for (const auto &interfaceName : interfacesProvided) {
                model->AddInterfaceProvided(interfaceName);
              }
              return model;
            });

        // Add node to graph
        QtNodes::NodeId nodeId =
            GraphModel->addNode(QString::fromStdString(component_name));
        std::cerr << "  Added node to graph: " << component_name
                  << " ID: " << nodeId << std::endl;

        // Store port mappings
        for (unsigned int i = 0; i < interfacesProvided.size(); ++i) {
          m_provided_interface_to_port[component_name][interfacesProvided[i]] =
              i;
        }
        for (unsigned int i = 0; i < interfacesRequired.size(); ++i) {
          m_required_interface_to_port[component_name][interfacesRequired[i]] =
              i;
        }

        m_components.push_back(component_name);
        NodeIds[component_name] = nodeId;
      },
      Qt::QueuedConnection);
}

void mtsComponentViewerQtWidget::AddConnectionHandler(
    const mtsDescriptionConnection &connection_description) {
  std::cerr << "Handler: added connection: " << connection_description
            << std::endl;

  // Use invokeMethod to ensure GUI updates happen in the GUI thread
  // and that we access the maps (NodeIds, port maps) from the correct thread
  QMetaObject::invokeMethod(
      this,
      [=]() {
        const std::string client_name =
            connection_description.Client.ComponentName;
        const std::string client_interface =
            connection_description.Client.InterfaceName;
        const std::string server_name =
            connection_description.Server.ComponentName;
        const std::string server_interface =
            connection_description.Server.InterfaceName;

        auto client_it = NodeIds.find(client_name);
        auto server_it = NodeIds.find(server_name);

        if (client_it != NodeIds.end() && server_it != NodeIds.end()) {
          auto client_port_map_it =
              m_required_interface_to_port.find(client_name);
          auto server_port_map_it =
              m_provided_interface_to_port.find(server_name);

          if (client_port_map_it != m_required_interface_to_port.end() &&
              server_port_map_it != m_provided_interface_to_port.end()) {
            auto client_port_it =
                client_port_map_it->second.find(client_interface);
            auto server_port_it =
                server_port_map_it->second.find(server_interface);

            if (client_port_it != client_port_map_it->second.end() &&
                server_port_it != server_port_map_it->second.end()) {
              QtNodes::NodeId client_node_id = client_it->second;
              QtNodes::NodeId server_node_id = server_it->second;
              QtNodes::PortIndex client_port_index = client_port_it->second;
              QtNodes::PortIndex server_port_index = server_port_it->second;

              std::cerr << "  Adding connection to graph: " << server_name
                        << ":" << server_interface << " (Port "
                        << server_port_index << ") -> " << client_name << ":"
                        << client_interface << " (Port " << client_port_index
                        << ")" << std::endl;

              // In QtNodes, connections go from Output to Input
              // Server (Provided) is Out, Client (Required) is In
              GraphModel->addConnection({server_node_id, server_port_index,
                                         client_node_id, client_port_index});
            }
          }
        }
      },
      Qt::QueuedConnection);
}
