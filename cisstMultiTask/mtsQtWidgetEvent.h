/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Praneeth Sadda, Anton Deguet
  Created on: 2011-11-11

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _mtsQtWidgetEvent_h
#define _mtsQtWidgetEvent_h

#include <map>

class mtsQtWidgetGenericObjectRead;
class mtsQtWidgetGenericObjectWrite;

#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsFunctionVoidReturn.h>
#include <cisstMultiTask/mtsFunctionWrite.h>
#include <cisstMultiTask/mtsFunctionWriteReturn.h>
#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstMultiTask/mtsFunctionQualifiedRead.h>

#include <cisstMultiTask/mtsGenericObject.h>

#include <QWidget>
#include <QTime>

class QLabel;
class QVBoxLayout;

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

/* ! A widget that wraps a single mtsCommand, allowing the user to execute the command and view its output. */
class CISST_EXPORT mtsQtWidgetEvent: public QWidget, public cmnGenericObject
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

 private:
    QLabel * Info;

 protected:
    QVBoxLayout * MainLayout;
    void UpdateInfo(void);
    unsigned long long int Counter;

 public:
    mtsQtWidgetEvent(void);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsQtWidgetEvent)


class CISST_EXPORT mtsQtWidgetEventVoid: public mtsQtWidgetEvent
{
    Q_OBJECT;
 public:
    explicit mtsQtWidgetEventVoid(void);
    void EventHandler(void);
};


class CISST_EXPORT mtsQtWidgetEventWrite: public mtsQtWidgetEvent
{
    Q_OBJECT;
    QWidget * PayloadWidget; 
    const mtsCommandWriteBase * Command;
 public:
    explicit mtsQtWidgetEventWrite(void);
    void EventHandler(const mtsGenericObject & payload);
    void SetCommand(const mtsCommandWriteBase * command);
    void CreateArgumentsWidgets(void);
};


#endif // _mtsQtWidgetEvent_h
