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
#if CISST_HAS_QTNODES
#include <cisstMultiTask/mtsComponentViewerQt.h>
#endif
#include <cisstMultiTask/mtsCollectorFactoryQtWidget.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstCommon/cmnQt.h>
#include <cisstCommon/cmnPath.h>
#include <cisstCommon/cmnLoggerQtWidget.h>
#include <QApplication>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <iostream>
#include <string>

mtsCommandLineOptionsQt::mtsCommandLineOptionsQt(const std::string & options):
    mtsCommandLineOptions(options)
{
    if (options.find('D') != std::string::npos) {
        this->AddOptionNoValue("D", "dark-mode",
                               "replaces the default Qt palette with darker colors");
    }
    if (options.find('S') != std::string::npos) {
        this->AddOptionOneValue("S", "qt-style",
                                "Qt style, use this option with a random name to see available styles",
                                cmnCommandLineOptions::OPTIONAL_OPTION, &QtStyle);
    }
#if CISST_HAS_QTNODES
    if (options.find('M') != std::string::npos) {
        this->AddOptionNoValue("M", "component-viewer",
                               "start the component viewer");
    }
#endif
    if (options.find('L') != std::string::npos) {
        this->AddOptionNoValue("L", "logger",
                               "show the logger widget to dynamically change log levels for all classes");
    }
}

void mtsCommandLineOptionsQt::Apply(void)
{
    mtsCommandLineOptions::Apply();
    cmnQt::QApplicationExitsOnCtrlC();
    if (this->IsSet("dark-mode")) {
        cmnQt::SetDarkMode();
    }
    if (this->IsSet("qt-style")) {
        std::string errorMessage = cmnQt::SetStyle(QtStyle);
        if (errorMessage != "") {
            std::cerr << errorMessage << std::endl;
        }
    }

#if CISST_HAS_QTNODES
    bool showViewer = this->IsSet("component-viewer");
#else
    bool showViewer = false;
#endif
    bool showCollection = !CollectionConfig.empty();
    bool showLogger = this->IsSet("logger");

    int showCount = (showViewer ? 1 : 0) + (showCollection ? 1 : 0) + (showLogger ? 1 : 0);

    if (showCount > 0) {
        mtsManagerLocal * componentManager = mtsManagerLocal::GetInstance();

        QString qAppName = qApp->applicationName();
        if (qAppName.isEmpty()) {
            qAppName = QFileInfo(qApp->applicationFilePath()).baseName();
        }
        std::string appName = qAppName.toStdString();

        QWidget * systemWindow = nullptr;
        QTabWidget * tabWidget = nullptr;

        if (showCount > 1) {
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

#if CISST_HAS_QTNODES
        if (showViewer) {
            mtsComponentViewerQt * viewer = new mtsComponentViewerQt("ComponentViewer");
            componentManager->AddComponent(viewer);
            if (tabWidget) {
                tabWidget->addTab(viewer, "Graph");
            } else {
                systemWindow->layout()->addWidget(viewer);
            }
        }
#endif

        if (showLogger) {
            cmnLoggerQtWidget * loggerWidget = new cmnLoggerQtWidget(nullptr);
            if (tabWidget) {
                tabWidget->addTab(loggerWidget->GetWidget(), "Logger");
            } else {
                systemWindow->layout()->addWidget(loggerWidget->GetWidget());
            }
        }

        systemWindow->show();
    }
}
