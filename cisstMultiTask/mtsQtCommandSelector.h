/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Praneeth Sadda
  Created on: 2012-05-14

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _mtsQtCommandSelector_h
#define _mtsQtCommandSelector_h

#include <QTreeWidget>

#include <cisstMultiTask/mtsManagerGlobal.h>

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

class mtsQtCommandSelector : public QTreeWidget {
    Q_OBJECT;  

public:
    mtsQtCommandSelector(mtsManagerGlobal* globalComponentManager, QWidget* parent = 0);

private slots:
    void CurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void ItemActivated(QTreeWidgetItem* item, int column);

public slots:
    void Refresh(void);

signals:
    void CommandSelected(QString command);

private:
    void BuildTree(mtsManagerGlobal* globalComponentManager);

    mtsManagerGlobal* GlobalManager;
};

#endif
