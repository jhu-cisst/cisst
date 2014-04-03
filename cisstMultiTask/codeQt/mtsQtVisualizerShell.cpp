/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Praneeth Sadda
  Created on:	2012-05-14

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <QHBoxLayout>
#include <QSplitter>

#include <cisstMultiTask/mtsQtScaleAndSignalTool.h>
#include <cisstMultiTask/mtsQtVisualizerShell.h>

mtsQtVisualizerShell::mtsQtVisualizerShell(mtsManagerGlobal * managerGlobal, vctPlot2DOpenGLQtWidget * visualizer, QWidget * parent)
    : QWidget(parent), Visualizer(visualizer)
{
    QLayout * layout = new QHBoxLayout();
    setLayout(layout);

    QSplitter * splitter = new QSplitter();
    layout->addWidget(splitter);

    if(Visualizer == 0) Visualizer = new vctPlot2DOpenGLQtWidget();

    mtsQtScaleAndSignalTool * scaleAndSignalTool = new mtsQtScaleAndSignalTool(managerGlobal, Visualizer);
    scaleAndSignalTool->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    splitter->addWidget(scaleAndSignalTool);

    Visualizer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    splitter->addWidget(Visualizer);
    //connect(CommandSelector, SIGNAL(CommandSelected(QString)), this, SLOT(SelectCommand));
}
