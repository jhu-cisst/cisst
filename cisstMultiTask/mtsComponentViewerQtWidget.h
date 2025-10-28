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

#ifndef _mtsComponentViewerQtWidget_h
#define _mtsComponentViewerQtWidget_h

#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsGraphLayoutQtNodes.h>

#include <QWidget>
#include <QTimer>

namespace QtNodes {
    class NodeDelegateModelRegistry;
    class DataFlowGraphModel;
    class DataFlowGraphicsScene;
    class GraphicsView;
}

// Always include last
#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsComponentViewerQtWidget: public QWidget, public mtsComponent
{
    Q_OBJECT
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION_ONEARG, CMN_LOG_ALLOW_DEFAULT);

public:
    mtsComponentViewerQtWidget(const std::string & componentName = "ComponentViewer");
    ~mtsComponentViewerQtWidget() = default;

    void Configure(const std::string & filename = "") override;
    void Startup(void) override;
    void Cleanup(void) override;

    void AddComponent(mtsComponent * component);
    void AddConnection(mtsComponent * client, const std::string & requiredInterface,
                      mtsComponent * server, const std::string & providedInterface);

    // Layout control
    void StartLayout(void);
    void StopLayout(void);
    void SetLayoutUpdateRate(double rateHz);

protected:
    void setupUi(void);
    void updateLayout(void);
    void updateNodePositions(void);

    // Qt Nodes objects
    std::shared_ptr<QtNodes::NodeDelegateModelRegistry> Registry;
    QtNodes::DataFlowGraphModel * GraphModel;
    QtNodes::DataFlowGraphicsScene * Scene;
    QtNodes::GraphicsView * View;

    // Layout objects
        mtsGraphLayoutQtNodes Layout;
    QTimer * LayoutTimer;
    bool LayoutRunning;
    int LayoutIterations;

    // Keep track of components and their node IDs
    std::vector<mtsComponent *> Components;
    // Keep NodeId as an unsigned int in the public header to avoid pulling
    // QtNodes types into all translation units; the implementation will use
    // QtNodes::NodeId where needed.
    std::map<mtsComponent *, unsigned int> NodeIds;

private:
    // no copy constructor
    mtsComponentViewerQtWidget(const mtsComponentViewerQtWidget & other) = delete;
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentViewerQtWidget);

#endif // _mtsComponentViewerQtWidget_h
