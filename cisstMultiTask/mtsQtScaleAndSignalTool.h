/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Praneeth Sadda
  Created on:	2012-05-24

  (C) Copyright 2012-2026 Johns Hopkins University (JHU), All Rights Reserved.

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

#include <eigenqt/Plot2DWidget.h>

#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsManagerLocal.h>

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

class CISST_EXPORT mtsQtScaleAndSignalTool: public QWidget
{
    Q_OBJECT;

public:
    mtsQtScaleAndSignalTool(mtsManagerLocal * componentManager, EigenQt::Plot2DWidget * visualizer, QWidget* parent = 0);

public slots:
    //void AddScale(/*Scale*/);
    //void AddSignal(/*Signal*/);
    //void RemoveScale(/*Scale*/);
    //void RemoveSignal(/*Signal*/);
    //void MoveSignal(/*Scale*/);

private:
    EigenQt::Plot2DWidget * Visualizer;
};

class mtsQtScaleEditor: public QTreeWidget
{
    Q_OBJECT;

public:
    mtsQtScaleEditor(EigenQt::Plot2DWidget * visualizer, QWidget* parent = 0);

    friend class mtsQtScaleAndSignalTool;

private:
    EigenQt::Plot2DWidget * Visualizer;
    int ScaleNameCounter;
    QString CurrentScale;

    void BuildTree(EigenQt::Plot2DWidget * visualizer);

private slots:
    void NewScale();
    void DeleteScale();
    void RenameScale();
    void ShowContextMenu(const QPoint & point);
};

#endif
