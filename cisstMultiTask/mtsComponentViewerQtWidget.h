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

#ifndef _mtsComponentViewerQtWidget_h
#define _mtsComponentViewerQtWidget_h

#include <cisstConfig.h>
#if !CISST_HAS_QTNODES
#error "mtsComponentViewerQtWidget.h requires CISST_HAS_QTNODES"
#endif

#include <cisstMultiTask/mtsTaskFromSignal.h>

#include <cisstMultiTask/mtsManagerComponentServices.h>
#include <cisstMultiTask/mtsParameterTypes.h>

#include <QTimer>
#include <QWidget>

class QToolBar;

namespace QtNodes {
    class NodeDelegateModelRegistry;
    class DataFlowGraphModel;
    class DataFlowGraphicsScene;
    class GraphicsView;
} // namespace QtNodes

// Always include last
#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsComponentViewerQtWidget : public QWidget,
                                                public mtsTaskFromSignal {
    Q_OBJECT
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION_ONEARG, CMN_LOG_ALLOW_DEFAULT);

 public:
    mtsComponentViewerQtWidget(
                               const std::string &componentName = "ComponentViewer");
    ~mtsComponentViewerQtWidget() = default;

    void Configure(const std::string &filename = "") override;
    void Startup(void) override;
    void Cleanup(void) override;
    void Run(void) override;

 protected slots:
    void onExportDOT(void);
    void onAutoLayout(void);
    void onContextMenuRequested(const QPoint &pos);

    void
        AddComponentHandler(const mtsDescriptionComponent &component_description);
    void
        AddConnectionHandler(const mtsDescriptionConnection &connection_description);

 protected:
    QToolBar *ToolBar;

    void setupUi(void);

    // Qt Nodes objects
    std::shared_ptr<QtNodes::NodeDelegateModelRegistry> Registry;
    QtNodes::DataFlowGraphModel *GraphModel;
    QtNodes::DataFlowGraphicsScene *Scene;
    QtNodes::GraphicsView *View;

    // Keep track of components and their node IDs
    std::vector<std::string> m_components;

    // Keep NodeId as an unsigned int in the public header to avoid pulling
    // QtNodes types into all translation units; the implementation will use
    // QtNodes::NodeId where needed.
    std::map<std::string, unsigned int> NodeIds;

    // Track interface to port index mapping
    std::map<std::string, std::map<std::string, unsigned int>>
        m_provided_interface_to_port;
    std::map<std::string, std::map<std::string, unsigned int>>
        m_required_interface_to_port;

    // Cache for rebuilding graph
    std::vector<mtsDescriptionComponent> m_component_infos;
    std::vector<mtsDescriptionConnection> m_connection_infos;

    bool m_showSystemInterfaces;
    void UpdateGraph(void);

 protected slots:
    void onToggleSystemInterfaces(bool checked);

 private:
    // no copy constructor
    mtsComponentViewerQtWidget(const mtsComponentViewerQtWidget &other) = delete;
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentViewerQtWidget);

#endif // _mtsComponentViewerQtWidget_h
