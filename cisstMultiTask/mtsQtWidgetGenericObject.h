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

#ifndef _mtsQtWidgetGenericObject_h
#define _mtsQtWidgetGenericObject_h

class QHBoxLayout;
#include <QWidget>

#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstMultiTask/mtsQtWidgetFactory.h>

// Always include last
#include <cisstMultiTask/mtsExportQt.h>


class mtsQtWidgetGenericObjectRead: public QWidget, public cmnGenericObject
{
    Q_OBJECT;

protected:
    QHBoxLayout * Layout;

public:
    mtsQtWidgetGenericObjectRead(void);
    virtual bool SetValue(const mtsGenericObject & value) = 0;
};


class mtsQtWidgetGenericObjectWrite: public mtsQtWidgetGenericObjectRead
{
    Q_OBJECT;

public:
    mtsQtWidgetGenericObjectWrite(void);
    virtual bool GetValue(mtsGenericObject & placeHolder) const = 0;
};


#define MTS_QT_REGISTER_WIDGET_READ_CREATOR(type, widgetType)           \
    mtsQtWidgetGenericObjectRead * mtsQtWidget##type##ReadCreateFunction(void) { \
        return new widgetType;                                          \
    }                                                                   \
    static const mtsQtWidgetFactory::WidgetReadCreator dummy##widgetType(&typeid(type), \
                                                                         mtsQtWidget##type##ReadCreateFunction);

#define MTS_QT_REGISTER_WIDGET_WRITE_CREATOR(type, widgetType)          \
    mtsQtWidgetGenericObjectWrite * mtsQtWidget##type##WriteCreateFunction(void) { \
        return new widgetType;                                          \
    }                                                                   \
    static const mtsQtWidgetFactory::WidgetWriteCreator dummy##widgetType(&typeid(type), \
                                                                          mtsQtWidget##type##WriteCreateFunction);


#define MTS_QT_IMPLEMENT_SERVICES_AND_REGISTER_CREATORS(type, creatorRead, creatorWrite) \
    CMN_IMPLEMENT_SERVICES(creatorRead);                                \
    CMN_IMPLEMENT_SERVICES(creatorWrite);                               \
    MTS_QT_REGISTER_WIDGET_READ_CREATOR(type, creatorRead);             \
    MTS_QT_REGISTER_WIDGET_WRITE_CREATOR(type, creatorWrite);

#endif // _mtsQtWidgetGenericObject_h
