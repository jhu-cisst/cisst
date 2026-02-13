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

#include <cisstMultiTask/mtsComponentModelQtNodes.h>
#include <cisstMultiTask/mtsComponentViewerQt.h>

#include <cisstMultiTask/mtsManagerComponentServices.h>
#include <cisstMultiTask/mtsManagerLocal.h>

#include <QAction>
#include <QContextMenuEvent>
#include <QEvent>
#include <QFileDialog>
#include <QLabel>
#include <QMenuBar>
#include <QMenu>
#include <QToolButton>
#include <QTimer>
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
#include <set>
#include <algorithm>

CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(mtsComponentViewerQt,
                                      mtsTaskFromSignal, std::string);

mtsComponentViewerQt::mtsComponentViewerQt(
                                                       const std::string &componentName)
    : mtsTaskFromSignal(componentName) {
    m_show_component_tags.insert("Generic");
    m_show_interface_tags.insert("Generic");
    m_show_interface_tags.insert("System");
    m_show_interface_tags.insert("State table");

    this->mTags.clear();
    this->AddTag("UI");
    // Register NodeId and ConnectionId for Qt signal/slot connections
    qRegisterMetaType<QtNodes::NodeId>("NodeId");
    qRegisterMetaType<QtNodes::ConnectionId>("ConnectionId");

    mtsInterfaceRequired *required = EnableDynamicComponentManagement();
    if (required) {
        required->SetMailBoxAndArgumentQueuesSize(1024);
        ManagerComponentServices->AddComponentEventHandler(
                                                           &mtsComponentViewerQt::AddComponentHandler, this);
        ManagerComponentServices->ChangeStateEventHandler(&mtsComponentViewerQt::ChangeStateHandler,
                                                          this);
        ManagerComponentServices->AddConnectionEventHandler(
                                                            &mtsComponentViewerQt::AddConnectionHandler, this);
        ManagerComponentServices->RemoveConnectionEventHandler(&mtsComponentViewerQt::RemoveConnectionHandler,
                                                               this);
    } else {
        cmnThrow(std::runtime_error("mtsComponentViewerQt constructor: failed to "
                                    "enable dynamic component composition"));
    }

    setupUi();
}

void mtsComponentViewerQt::Configure(
                                           const std::string &CMN_UNUSED(filename)) {}

void mtsComponentViewerQt::Startup(void) {
    // Query existing components and connections before starting event monitoring
    auto processNames = ManagerComponentServices->GetNamesOfProcesses();
    
    for (const auto &processName : processNames) {
        auto descriptions = ManagerComponentServices->GetDescriptionsOfComponents(processName);
        for (const auto &desc : descriptions) {
            m_component_infos.push_back(desc);
        }
    }
    
    // Query existing connections
    auto connections = ManagerComponentServices->GetListOfConnections();
    m_connection_infos = connections;
    
    // Populate the graph with existing components and connections
    QMetaObject::invokeMethod(this, [=]() {
        UpdateGraph();
        // Auto-layout the graph after a short delay to ensure rendering is complete
        QTimer::singleShot(1000, this, &mtsComponentViewerQt::onAutoLayout);
    }, Qt::QueuedConnection);
}

void mtsComponentViewerQt::Cleanup(void) {}

void mtsComponentViewerQt::Run(void) {
    ProcessQueuedCommands();
    ProcessQueuedEvents();
}

void mtsComponentViewerQt::setupUi(void) {
    Registry = std::make_shared<QtNodes::NodeDelegateModelRegistry>();
    GraphModel = new QtNodes::DataFlowGraphModel(Registry);
    Scene = new QtNodes::DataFlowGraphicsScene(*GraphModel);
    View = new QtNodes::GraphicsView(Scene);

    QVBoxLayout *layout = new QVBoxLayout();
    this->setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);

    // Create toolbar
    ToolBar = new QToolBar(this);
    QAction *actionAutoLayout = ToolBar->addAction(tr("Auto-Layout"));
    QAction *actionExportDOT = ToolBar->addAction(tr("Export as DOT..."));
    ToolBar->addSeparator();

    QMenu *filtersMenu = new QMenu(tr("Filters"), this);
    QToolButton *filtersButton = new QToolButton(this);
    filtersButton->setMenu(filtersMenu);
    filtersButton->setPopupMode(QToolButton::InstantPopup);
    filtersButton->setText(tr("Filters"));
    ToolBar->addWidget(filtersButton);

    // Components section
    filtersMenu->addSection(tr("Components"));
    const std::set<std::string> & componentTags = mtsManagerLocal::GetInstance()->GetValidComponentTags();
    for (auto & tag : componentTags) {
        QAction *action = filtersMenu->addAction(QString::fromStdString(tag));
        action->setCheckable(true);
        action->setChecked(m_show_component_tags.count(tag));
        action->setData(QString::fromStdString(tag));
        connect(action, &QAction::triggered, this, &mtsComponentViewerQt::onFilterComponents);
    }

    // Interfaces section
    filtersMenu->addSection(tr("Interfaces"));
    const std::set<std::string> & interfaceTags = mtsManagerLocal::GetInstance()->GetValidInterfaceTags();
    for (auto & tag : interfaceTags) {
        QAction *action = filtersMenu->addAction(QString::fromStdString(tag));
        action->setCheckable(true);
        action->setChecked(m_show_interface_tags.count(tag));
        action->setData(QString::fromStdString(tag));
        connect(action, &QAction::triggered, this, &mtsComponentViewerQt::onFilterInterfaces);
    }

    connect(actionAutoLayout, &QAction::triggered, this,
            &mtsComponentViewerQt::onAutoLayout);
    connect(actionExportDOT, &QAction::triggered, this,
            &mtsComponentViewerQt::onExportDOT);

    layout->addWidget(ToolBar);
    layout->addWidget(View);

    View->setContextMenuPolicy(Qt::NoContextMenu);

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

void mtsComponentViewerQt::onContextMenuRequested(const QPoint &pos) {
    QMenu contextMenu(this);
    QAction *actionAutoLayout = contextMenu.addAction(tr("Auto-Layout"));
    QAction *actionExportDOT = contextMenu.addAction(tr("Export as DOT..."));

    connect(actionAutoLayout, &QAction::triggered, this,
            &mtsComponentViewerQt::onAutoLayout);
    connect(actionExportDOT, &QAction::triggered, this,
            &mtsComponentViewerQt::onExportDOT);

    contextMenu.exec(View->mapToGlobal(pos));
}

void mtsComponentViewerQt::onExportDOT(void) {
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
        std::cerr << "To convert the dot file to PDF, use: dot -Tpdf "
                  << fileName.toStdString() << " -o output.pdf" << std::endl;
    }

    agclose(g);
    gvFreeContext(gvc);
}

void mtsComponentViewerQt::onFilterComponents(bool checked) {
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        std::string tag = action->data().toString().toStdString();
        if (checked) {
            m_show_component_tags.insert(tag);
        } else {
            m_show_component_tags.erase(tag);
        }
        UpdateGraph();
    }
}

void mtsComponentViewerQt::onFilterInterfaces(bool checked) {
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        std::string tag = action->data().toString().toStdString();
        if (checked) {
            m_show_interface_tags.insert(tag);
        } else {
            m_show_interface_tags.erase(tag);
        }
        UpdateGraph();
    }
}

void mtsComponentViewerQt::onAutoLayout(void) {
    GVC_t *gvc = gvContext();
    Agraph_t *g = agopen((char *)"G", Agdirected, nullptr);

    // Set graph attributes for left-to-right
    agsafeset(g, (char *)"rankdir", (char *)"LR", (char *)"LR");
    agsafeset(g, (char *)"nodesep", (char *)"1.0", (char *)"1.0");
    agsafeset(g, (char *)"ranksep", (char *)"1.5", (char *)"1.5");

    std::map<QtNodes::NodeId, Agnode_t *> graphvizNodes;
    for (const auto &component : m_components) {
        QtNodes::NodeId id = NodeIds[component];
        Agnode_t *v = agnode(g, (char *)component.c_str(), 1);

        // Get actual size from scene
        if (Scene) {
            QSize size = Scene->nodeGeometry().size(id);
            // Graphviz uses inches, assume 72 DPI, but let's be generous
            double width = size.width() / 72.0;
            double height = size.height() / 72.0;
            char wStr[32], hStr[32];
            sprintf(wStr, "%.2f", width);
            sprintf(hStr, "%.2f", height);
            agsafeset(v, (char *)"width", wStr, (char *)"");
            agsafeset(v, (char *)"height", hStr, (char *)"");
            agsafeset(v, (char *)"fixedsize", (char *)"true", (char *)"");
        }

        // Pass current position to Graphviz
        QPointF pos =
            GraphModel->nodeData(id, QtNodes::NodeRole::Position).value<QPointF>();
        if (!pos.isNull()) {
            char posStr[64];
            // Note: Graphviz Y is inverted relative to QtNodes (QtNodes Y+ down,
            // Graphviz Y+ up usually, but we inverted it on set) On set: QPointF(x,
            // -y). So read back: x=x, y=-y.
            sprintf(posStr, "%.2f,%.2f", pos.x(), -pos.y());
            agsafeset(v, (char *)"pos", posStr, (char *)"");
        }

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

    // Center view
    if (Scene) {
        View->centerOn(Scene->itemsBoundingRect().center());
    }
}

void mtsComponentViewerQt::AddComponentHandler(
                                                     const mtsDescriptionComponent &component_description) {
    // Use invokeMethod to ensure GUI updates happen in the GUI thread
    QMetaObject::invokeMethod(
                              this,
                              [=]() {
                                  // Check if component already exists to prevent duplicates
                                  auto it = std::find_if(m_component_infos.begin(), m_component_infos.end(),
                                      [&](const mtsDescriptionComponent &desc) {
                                          return desc.ProcessName == component_description.ProcessName &&
                                                 desc.ComponentName == component_description.ComponentName;
                                      });
                                  
                                  // Only add if not found
                                  if (it == m_component_infos.end()) {
                                      m_component_infos.push_back(component_description);
                                      UpdateGraph();
                                  }
                              },
                              Qt::QueuedConnection);
}

void mtsComponentViewerQt::AddConnectionHandler(
                                                      const mtsDescriptionConnection &connection_description) {
    // Use invokeMethod to ensure GUI updates happen in the GUI thread
    QMetaObject::invokeMethod(
                              this,
                              [=]() {
                                  m_connection_infos.push_back(connection_description);
                                  UpdateGraph();
                              },
                              Qt::QueuedConnection);
}

void mtsComponentViewerQt::RemoveConnectionHandler(
                                                         const mtsDescriptionConnection &connection_description) {
    // Use invokeMethod to ensure GUI updates happen in the GUI thread
    QMetaObject::invokeMethod(
                              this,
                              [=]() {
                                  // Locate and remove connection from m_connection_infos
                                  // Since we don't have a unique ID for connection descriptions, we might need to filter
                                  // For simplicity, we can rebuild the list or just remove matching entries.
                                  // Better yet, just remove from m_connection_infos if we can match it.
                                  // But UpdateGraph rebuilds everything from m_connection_infos, so we must remove it from there.

                                  auto it = std::remove_if(m_connection_infos.begin(), m_connection_infos.end(),
                                                           [&](const mtsDescriptionConnection &c) {
                                                               return (c.ConnectionID == connection_description.ConnectionID &&
                                                                       c.Client.ComponentName == connection_description.Client.ComponentName &&
                                                                       c.Client.InterfaceName == connection_description.Client.InterfaceName &&
                                                                       c.Server.ComponentName == connection_description.Server.ComponentName &&
                                                                       c.Server.InterfaceName == connection_description.Server.InterfaceName);
                                                           });
                                  if (it != m_connection_infos.end()) {
                                      m_connection_infos.erase(it, m_connection_infos.end());
                                      UpdateGraph();
                                  }
                              },
                              Qt::QueuedConnection);
}

void mtsComponentViewerQt::ChangeStateHandler(const mtsComponentStateChange &state_change) {
    QMetaObject::invokeMethod(
                              this,
                              [=]() {
                                  std::string name = state_change.ComponentName;
                                  auto it = NodeIds.find(name);
                                  if (it != NodeIds.end()) {
                                      QtNodes::NodeId id = it->second;
                                      auto cisstNode = GraphModel->delegateModel<mtsComponentModelQtNodes>(id);
                                      if (cisstNode) {
                                          cisstNode->SetState(mtsComponentState::EnumToString(state_change.NewState.State()));
                                      }
                                  }
                              },
                              Qt::QueuedConnection);
}

void mtsComponentViewerQt::UpdateGraph(void) {
    if (!GraphModel)
        return;

    // Save positions of existing nodes
    std::map<std::string, QPointF> savedPositions;
    for (const auto &pair : NodeIds) {
        if (GraphModel->nodeExists(pair.second)) {
            savedPositions[pair.first] =
                GraphModel->nodeData(pair.second, QtNodes::NodeRole::Position)
                .value<QPointF>();
        }
    }

    // Clear Model
    // Clear Graph and Scene
    if (Scene) {
        Scene->clearScene();
    } else {
        auto allIds = GraphModel->allNodeIds();
        for (auto id : allIds) {
            GraphModel->deleteNode(id);
        }
    }
    NodeIds.clear();
    m_components.clear();
    m_provided_interface_to_port.clear();
    m_required_interface_to_port.clear();

    // Identify connected components
    std::set<std::string> connectedComponents;
    for (const auto &conn : m_connection_infos) {
        connectedComponents.insert(conn.Client.ComponentName);
        connectedComponents.insert(conn.Server.ComponentName);
    }

    // Re-add components
    for (const auto &desc : m_component_infos) {
        std::string name = desc.ComponentName;

        bool showComponent = false;
        if (desc.Tags.empty()) {
            // Components without tags are shown if Generic is checked
            showComponent = m_show_component_tags.count("Generic");
        } else {
            for (auto & tag : desc.Tags) {
                if (m_show_component_tags.count(tag)) {
                    showComponent = true;
                    break;
                }
            }
        }
        if (!showComponent) {
            continue;
        }

        // Fetch interfaces and filter them
        std::vector<mtsDescriptionInterfaceFullName> interfacesRequiredFull, interfacesProvidedFull;
        ManagerComponentServices->GetDescriptionsOfInterfaces(desc.ProcessName, name, interfacesRequiredFull, interfacesProvidedFull);

        std::vector<std::string> interfacesRequired, interfacesProvided;
        for (const auto &intfc : interfacesRequiredFull) {
            bool showIntfc = false;
            if (intfc.Tags.empty()) {
                showIntfc = m_show_interface_tags.count("Generic");
            } else {
                for (auto & tag : intfc.Tags) {
                    if (m_show_interface_tags.count(tag)) {
                        showIntfc = true;
                        break;
                    }
                }
            }
            if (showIntfc) {
                interfacesRequired.push_back(intfc.InterfaceName);
            }
        }
        for (const auto &intfc : interfacesProvidedFull) {
            bool showIntfc = false;
            if (intfc.Tags.empty()) {
                showIntfc = m_show_interface_tags.count("Generic");
            } else {
                for (auto & tag : intfc.Tags) {
                    if (m_show_interface_tags.count(tag)) {
                        showIntfc = true;
                        break;
                    }
                }
            }
            if (showIntfc) {
                interfacesProvided.push_back(intfc.InterfaceName);
            }
        }

        auto state = ManagerComponentServices->ComponentGetState(desc);
        std::string className = desc.ClassName;

        Registry->registerModel<mtsComponentModelQtNodes>(
            [name, className, state, interfacesRequired, interfacesProvided]() {
                auto model = std::make_unique<mtsComponentModelQtNodes>(name);
                model->SetClassName(className);
                model->SetState(mtsComponentState::EnumToString(state.State()));
                for (const auto &interfaceName : interfacesRequired) {
                    model->AddInterfaceRequired(interfaceName);
                }
                for (const auto &interfaceName : interfacesProvided) {
                    model->AddInterfaceProvided(interfaceName);
                }
                return model;
            });

        QtNodes::NodeId nodeId = GraphModel->addNode(QString::fromStdString(name));
        NodeIds[name] = nodeId;
        m_components.push_back(name);

        for (unsigned int i = 0; i < interfacesProvided.size(); ++i) {
            m_provided_interface_to_port[name][interfacesProvided[i]] = i;
        }
        for (unsigned int i = 0; i < interfacesRequired.size(); ++i) {
            m_required_interface_to_port[name][interfacesRequired[i]] = i;
        }

        // Restore position
        if (savedPositions.count(name)) {
            GraphModel->setNodeData(nodeId, QtNodes::NodeRole::Position,
                                    savedPositions[name]);
        }
    }

    // Re-add connections
    for (const auto &conn : m_connection_infos) {
        const std::string client_name = conn.Client.ComponentName;
        const std::string client_interface = conn.Client.InterfaceName;
        const std::string server_name = conn.Server.ComponentName;
        const std::string server_interface = conn.Server.InterfaceName;

        auto client_it = NodeIds.find(client_name);
        auto server_it = NodeIds.find(server_name);

        if (client_it != NodeIds.end() && server_it != NodeIds.end()) {
            // Both nodes exist (passed filter)
            auto client_port_map_it = m_required_interface_to_port.find(client_name);
            auto server_port_map_it = m_provided_interface_to_port.find(server_name);
            if (client_port_map_it != m_required_interface_to_port.end() &&
                server_port_map_it != m_provided_interface_to_port.end()) {
                auto client_port_it = client_port_map_it->second.find(client_interface);
                auto server_port_it = server_port_map_it->second.find(server_interface);
                if (client_port_it != client_port_map_it->second.end() &&
                    server_port_it != server_port_map_it->second.end()) {
                    QtNodes::ConnectionId cId;
                    cId.outNodeId = server_it->second;
                    cId.outPortIndex = server_port_it->second;
                    cId.inNodeId = client_it->second;
                    cId.inPortIndex = client_port_it->second;
                    GraphModel->addConnection(cId);
                }
            }
        }
    }
}
