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

#ifndef _mtsQtWidgetGenericObjectProxy_h
#define _mtsQtWidgetGenericObjectProxy_h

#include <cisstMultiTask/mtsQtWidgetGenericObject.h>

class QLabel;
class QSpinBox;
class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QTableWidget;

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

// -- mtsInt
class CISST_EXPORT mtsQtWidgetIntRead: public mtsQtWidgetGenericObjectRead
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
 private:
    QLabel * Label;
 public:
    mtsQtWidgetIntRead(void);
    virtual bool SetValue(const mtsGenericObject & value);
};
CMN_DECLARE_SERVICES_INSTANTIATION(mtsQtWidgetIntRead);

class CISST_EXPORT mtsQtWidgetIntWrite: public mtsQtWidgetGenericObjectWrite
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
 private:
    QSpinBox * SpinBox;
 public:
    mtsQtWidgetIntWrite(void);
    virtual bool SetValue(const mtsGenericObject & value);
    virtual bool GetValue(mtsGenericObject & placeHolder) const;
};
CMN_DECLARE_SERVICES_INSTANTIATION(mtsQtWidgetIntWrite);


// -- mtsBool
class CISST_EXPORT mtsQtWidgetBoolRead: public mtsQtWidgetGenericObjectRead
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
 private:
    QLabel * Label;
 public:
    mtsQtWidgetBoolRead(void);
    virtual bool SetValue(const mtsGenericObject & value);
};
CMN_DECLARE_SERVICES_INSTANTIATION(mtsQtWidgetBoolRead);

class CISST_EXPORT mtsQtWidgetBoolWrite: public mtsQtWidgetGenericObjectWrite
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
 private:
    QComboBox * ComboBox;
 public:
    mtsQtWidgetBoolWrite(void);
    virtual bool SetValue(const mtsGenericObject & value);
    virtual bool GetValue(mtsGenericObject & placeHolder) const;
};
CMN_DECLARE_SERVICES_INSTANTIATION(mtsQtWidgetBoolWrite);


// -- mtsDouble
class CISST_EXPORT mtsQtWidgetDoubleRead: public mtsQtWidgetGenericObjectRead
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
 private:
    QLabel * Label;
 public:
    mtsQtWidgetDoubleRead(void);
    virtual bool SetValue(const mtsGenericObject & value);
};
CMN_DECLARE_SERVICES_INSTANTIATION(mtsQtWidgetDoubleRead);

class CISST_EXPORT mtsQtWidgetDoubleWrite: public mtsQtWidgetGenericObjectWrite
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
 private:
    QDoubleSpinBox * DoubleSpinBox;
 public:
    mtsQtWidgetDoubleWrite(void);
    virtual bool SetValue(const mtsGenericObject & value);
    virtual bool GetValue(mtsGenericObject & placeHolder) const;
};
CMN_DECLARE_SERVICES_INSTANTIATION(mtsQtWidgetDoubleWrite);


// -- mtsStdString
class CISST_EXPORT mtsQtWidgetStdStringRead: public mtsQtWidgetGenericObjectRead
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
 private:
    QLabel * Label;
 public:
    mtsQtWidgetStdStringRead(void);
    virtual bool SetValue(const mtsGenericObject & value);
};
CMN_DECLARE_SERVICES_INSTANTIATION(mtsQtWidgetStdStringRead);

class CISST_EXPORT mtsQtWidgetStdStringWrite: public mtsQtWidgetGenericObjectWrite
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
 private:
    QLineEdit * LineEdit;
 public:
    mtsQtWidgetStdStringWrite(void);
    virtual bool SetValue(const mtsGenericObject & value);
    virtual bool GetValue(mtsGenericObject & placeHolder) const;
};
CMN_DECLARE_SERVICES_INSTANTIATION(mtsQtWidgetStdStringWrite);


// -- mtsStateIndex
class CISST_EXPORT mtsQtWidgetStateIndexRead: public mtsQtWidgetGenericObjectRead
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
 private:
    QLabel * Label;
 public:
    mtsQtWidgetStateIndexRead(void);
    virtual bool SetValue(const mtsGenericObject & value);
};
CMN_DECLARE_SERVICES_INSTANTIATION(mtsQtWidgetStateIndexRead);

class CISST_EXPORT mtsQtWidgetStateIndexWrite: public mtsQtWidgetGenericObjectWrite
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
 private:
    QTableWidget * TableWidget;
    QSpinBox * IndexSpinBox;
    QDoubleSpinBox * TicksSpinBox;
    QDoubleSpinBox * LengthSpinBox;
    static QWidget * MakeSpinBox();
    static QWidget * MakeDoubleSpinBox();
 public:
    mtsQtWidgetStateIndexWrite(void);
    virtual bool SetValue(const mtsGenericObject & value);
    virtual bool GetValue(mtsGenericObject & placeHolder) const;
};
CMN_DECLARE_SERVICES_INSTANTIATION(mtsQtWidgetStateIndexWrite);

#endif // _mtsQtWidgetGenericObject_h
