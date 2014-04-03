/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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

#ifndef _mtsQtScaleAndSignalTool_h
#define _mtsQtScaleAndSignalTool_h

#include <QWidget>
#include <QTreeWidget>

#include <cisstVector/vctPlot2DOpenGLQtWidget.h>

#include <cisstMultiTask/mtsManagerGlobal.h>

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

class CISST_EXPORT mtsQtScaleAndSignalTool: public QWidget
{
    Q_OBJECT;

public:
    mtsQtScaleAndSignalTool(mtsManagerGlobal * managerGlobal, vctPlot2DOpenGLQtWidget * visualizer, QWidget* parent = 0);

public slots:
    //void AddScale(/*Scale*/);
    //void AddSignal(/*Signal*/);
    //void RemoveScale(/*Scale*/);
    //void RemoveSignal(/*Signal*/);
    //void MoveSignal(/*Scale*/);

private:
    vctPlot2DOpenGLQtWidget * Visualizer;
};

class mtsQtScaleEditor: public QTreeWidget
{
    Q_OBJECT;

public:
    mtsQtScaleEditor(vctPlot2DOpenGLQtWidget * visualizer, QWidget* parent = 0);

    friend class mtsQtScaleAndSignalTool;

private:
    vctPlot2DOpenGLQtWidget * Visualizer;
    int ScaleNameCounter;
    QString CurrentScale;

    void BuildTree(vctPlot2DOpenGLQtWidget * visualizer);

private slots:
    void NewScale();
    void DeleteScale();
    void RenameScale();
    void ShowContextMenu(const QPoint & point);
};

#endif
