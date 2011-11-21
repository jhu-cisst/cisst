/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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

#ifndef _mtsArgumentWidget_h
#define _mtsArgumentWidget_h

#include <QWidget>

#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstMultiTask/mtsParameterTypes.h>

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

class CISST_EXPORT ArgumentWidget : public QWidget {

    Q_OBJECT;

protected:
    QLayout * Layout;
    QWidget * Focused;

public slots:
    /* ! Set the value currently stored in the widget. */
    virtual void SetValue(const mtsGenericObject& value) = 0;

public:
    ArgumentWidget(void);

    /* ! Return the value currently stored in the widget. (This is mainly intended for editor widgets.) */
    virtual const mtsGenericObject& GetValue(void) const = 0;

    /* ! Return a reference to an instance of the type handled by the widget. For example, if the widget
       is an editor for an mtsDouble, tihs function will return a reference to an mtsDouble. Any
       given instance of ArgumentWidget will return the same reference on every call to this function.
    */
    virtual mtsGenericObject& GetValueRef(void) = 0;
    virtual ~ArgumentWidget(void) { }

    static ArgumentWidget* CreateIntInputWidget(void);
    static ArgumentWidget* CreateBoolInputWidget(void);
    static ArgumentWidget* CreateDoubleInputWidget(void);
    static ArgumentWidget* CreateStdStringInputWidget(void);

    static ArgumentWidget* CreateIntOutputWidget(void);
    static ArgumentWidget* CreateBoolOutputWidget(void);
    static ArgumentWidget* CreateDoubleOutputWidget(void);
    static ArgumentWidget* CreateStdStringOutputWidget(void);
};

#endif // _mtsArgumentWidget_h
