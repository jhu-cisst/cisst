/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2026-02-20

  (C) Copyright 2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsCommandLineOptionsQt.h>
#include <cisstMultiTask/mtsComponentViewerQt.h>
#include <cisstMultiTask/mtsCollectorFactoryQtWidget.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstCommon/cmnQt.h>
#include <cisstCommon/cmnPath.h>
#include <QApplication>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <iostream>
#include <string>

mtsCommandLineOptionsQt::mtsCommandLineOptionsQt(void):
    mtsCommandLineOptions()
{
    this->AddOptionNoValue("D", "dark-mode",
                           "replaces the default Qt palette with darker colors");
    this->AddOptionOneValue("S", "qt-style",
                            "Qt style, use this option with a random name to see available styles",
                            cmnCommandLineOptions::OPTIONAL_OPTION, &QtStyle);
    this->AddOptionNoValue("M", "component-viewer",
                           "start the component viewer");
}

void mtsCommandLineOptionsQt::Apply(void)
{
    mtsCommandLineOptions::Apply();
    if (this->IsSet("dark-mode")) {
        cmnQt::SetDarkMode();
    }
    if (this->IsSet("qt-style")) {
        std::string errorMessage = cmnQt::SetStyle(QtStyle);
        if (errorMessage != "") {
            std::cerr << errorMessage << std::endl;
        }
    }

    bool showViewer = this->IsSet("component-viewer");
    bool showCollection = !CollectionConfig.empty();

    if (showViewer || showCollection) {
        mtsManagerLocal * componentManager = mtsManagerLocal::GetInstance();

        QString qAppName = qApp->applicationName();
        if (qAppName.isEmpty()) {
            qAppName = QFileInfo(qApp->applicationFilePath()).baseName();
        }
        std::string appName = qAppName.toStdString();

        QWidget * systemWindow = nullptr;
        QTabWidget * tabWidget = nullptr;

        if (showViewer && showCollection) {
            tabWidget = new QTabWidget();
            systemWindow = tabWidget;
        } else {
            systemWindow = new QWidget();
            new QVBoxLayout(systemWindow);
            systemWindow->layout()->setContentsMargins(0, 0, 0, 0);
        }
        systemWindow->setWindowTitle(QString::fromStdString(appName + " System"));

        if (showCollection) {
            mtsCollectorFactoryQtWidget * collectorWidget = new mtsCollectorFactoryQtWidget("collectors-GUI");
            componentManager->AddComponent(collectorWidget);
            componentManager->Connect(collectorWidget->GetName(), "Collector", "collectors", "Control");

            QWidget * collectorWrapper = new QWidget();
            QHBoxLayout * collectorLayout = new QHBoxLayout(collectorWrapper);
            collectorLayout->addWidget(collectorWidget);
            collectorLayout->addStretch();

            if (tabWidget) {
                tabWidget->addTab(collectorWrapper, "Collection");
            } else {
                systemWindow->layout()->addWidget(collectorWrapper);
            }
        }

        if (showViewer) {
            mtsComponentViewerQt * viewer = new mtsComponentViewerQt("ComponentViewer");
            componentManager->AddComponent(viewer);
            if (tabWidget) {
                tabWidget->addTab(viewer, "Graph");
            } else {
                systemWindow->layout()->addWidget(viewer);
            }
        }
        systemWindow->show();
    }
}
