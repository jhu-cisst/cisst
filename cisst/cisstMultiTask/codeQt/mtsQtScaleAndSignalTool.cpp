/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):	Praneeth Sadda
  Created on:	2012-05-24

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <QVBoxLayout>
#include <QSplitter>
#include <QTreeWidget>

#include <cisstMultiTask/mtsQtCommandSelector.h>
#include <cisstMultiTask/mtsQtScaleAndSignalTool.h>

mtsQtScaleAndSignalTool::mtsQtScaleAndSignalTool(mtsManagerGlobal * globalManager, vctPlot2DOpenGLQtWidget * visualizer, QWidget * parent)
    : QWidget(parent), Visualizer(visualizer)
{
    QLayout * layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    QSplitter * splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);
    layout->addWidget(splitter);

    splitter->addWidget(new QTreeWidget());
    splitter->addWidget(new mtsQtCommandSelector(globalManager));
}
