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

#ifndef _mtsArgumentWidgets_h
#define _mtsArgumentWidgets_h

#include <cisstMultiTask/mtsArgumentWidget.h>

#include <QObject>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QComboBox>

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

class FocusForwarder : public QObject {
private:
    QWidget** focused;
public:
    FocusForwarder(QWidget** focusReceiver);
    bool eventFilter(QObject *obj, QEvent *event);
};


class CISST_EXPORT IntInputWidget : public ArgumentWidget {

    Q_OBJECT

private:
    QSpinBox* spinBox;
    mutable mtsInt data;

public slots:
    virtual void SetValue(const mtsGenericObject& value);

public:
    IntInputWidget();
    virtual const mtsGenericObject& GetValue() const;
    virtual mtsGenericObject& GetValueRef();
};


class CISST_EXPORT BoolInputWidget : public ArgumentWidget {

    Q_OBJECT

private:
    QComboBox* comboBox;
    mutable mtsBool data;

public slots:
    virtual void SetValue(const mtsGenericObject& value);

public:
    BoolInputWidget();
    virtual const mtsGenericObject& GetValue() const;
    virtual mtsGenericObject& GetValueRef();
};

class CISST_EXPORT DoubleInputWidget : public ArgumentWidget {

    Q_OBJECT

private:
    QDoubleSpinBox* spinBox;
    mutable mtsDouble data;

public slots:
    virtual void SetValue(const mtsGenericObject& value);

public:
    DoubleInputWidget();
    virtual const mtsGenericObject& GetValue() const;
    virtual mtsGenericObject& GetValueRef();
};

class CISST_EXPORT StdStringInputWidget : public ArgumentWidget {

    Q_OBJECT

private:
    QLineEdit* lineEdit;
    mutable mtsStdString data;

public slots:
    virtual void SetValue(const mtsGenericObject& value);

public:
    StdStringInputWidget();
    virtual const mtsGenericObject& GetValue() const;
    virtual mtsGenericObject& GetValueRef();
};

class CISST_EXPORT OutputWidget : public ArgumentWidget {

    Q_OBJECT

protected:
    QLabel* label;

public:
    OutputWidget();
};

class CISST_EXPORT IntOutputWidget : public OutputWidget {

    Q_OBJECT

private:
    mutable mtsInt data;

public slots:
    virtual void SetValue(const mtsGenericObject& value);

public:
    virtual const mtsGenericObject& GetValue() const;
    virtual mtsGenericObject& GetValueRef();
};

class CISST_EXPORT BoolOutputWidget : public OutputWidget {

    Q_OBJECT

private:
    mutable mtsBool data;

public slots:
    virtual void SetValue(const mtsGenericObject& value);

public:
    virtual const mtsGenericObject& GetValue() const;
    virtual mtsGenericObject& GetValueRef();
};

class CISST_EXPORT DoubleOutputWidget : public OutputWidget {

    Q_OBJECT

private:
    mutable mtsDouble data;

public slots:
    virtual void SetValue(const mtsGenericObject& value);

public:
    virtual const mtsGenericObject& GetValue() const;
    virtual mtsGenericObject& GetValueRef();
};

class CISST_EXPORT StdStringOutputWidget : public OutputWidget {

    Q_OBJECT

private:
    mutable mtsStdString data;

public slots:
    virtual void SetValue(const mtsGenericObject& value);

public:
    virtual const mtsGenericObject& GetValue() const;
    virtual mtsGenericObject& GetValueRef();
};

#endif // _mtsArgumentsWidgets_h
