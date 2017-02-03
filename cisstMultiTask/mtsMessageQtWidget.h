/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2013-05-17

  (C) Copyright 2013-2017 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsMessageQtWidget_h
#define _mtsMessageQtWidget_h

#include <cisstMultiTask/mtsComponent.h>

#include <QTextEdit>

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

class CISST_EXPORT mtsMessageQtWidget: public QTextEdit
{
    Q_OBJECT;

public:
    mtsMessageQtWidget(void);
    inline virtual ~mtsMessageQtWidget() {}

    void SetInterfaceRequired(mtsInterfaceRequired * interfaceRequired);
    
signals:
    void SignalAppendMessage(QString);
    void SignalSetColor(QColor);

private slots:
    void SlotTextChanged(void);

protected:

    void setupUi(void);

    void ErrorEventHandler(const mtsMessage & message);
    void WarningEventHandler(const mtsMessage & message);
    void StatusEventHandler(const mtsMessage & message);
};

#endif // _mtsMessageQtWidget_h
