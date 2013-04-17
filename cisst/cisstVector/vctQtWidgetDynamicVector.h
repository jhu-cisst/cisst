/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2011-12-08

  (C) Copyright 2011-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _vctQtWidgetDynamicVector_h
#define _vctQtWidgetDynamicVector_h

class QWidget;
class QTableWidget;
class QTableWidgetItem;
#include <QObject>

#include <cisstVector/vctDynamicVector.h>

// Always include last
#include <cisstVector/vctExportQt.h>

class CISST_EXPORT vctQtWidgetDynamicVectorReadBase
{
protected:
    QTableWidget * Table;
public:
    vctQtWidgetDynamicVectorReadBase(void);
    QWidget * GetWidget(void);
};

template <class _elementType>
class CISST_EXPORT vctQtWidgetDynamicVectorReadFloating: public vctQtWidgetDynamicVectorReadBase
{
    typedef _elementType value_type;
    int Precision;
    char Format;
public:
    vctQtWidgetDynamicVectorReadFloating(void);
    virtual bool SetValue(const vctDynamicVector<value_type> & value);
    void SetPrecision(const int precision);
    void SetFormat(const char format);
};

template <class _elementType>
class CISST_EXPORT vctQtWidgetDynamicVectorReadInteger: public vctQtWidgetDynamicVectorReadBase
{
    typedef _elementType value_type;
    int Base;
public:
    vctQtWidgetDynamicVectorReadInteger(void);
    virtual bool SetValue(const vctDynamicVector<value_type> & value);
    void SetBase(const int base);
};

typedef vctQtWidgetDynamicVectorReadFloating<double> vctQtWidgetDynamicVectorDoubleRead;
typedef vctQtWidgetDynamicVectorReadFloating<float> vctQtWidgetDynamicVectorFloatRead;
typedef vctQtWidgetDynamicVectorReadInteger<int> vctQtWidgetDynamicVectorIntRead;
typedef vctQtWidgetDynamicVectorReadInteger<unsigned int> vctQtWidgetDynamicVectorUIntRead;
typedef vctQtWidgetDynamicVectorReadInteger<bool> vctQtWidgetDynamicVectorBoolRead;

#if ((CISST_OS == CISST_WINDOWS) && (CISST_COMPILER != CISST_GCC))
template class CISST_EXPORT vctQtWidgetDynamicVectorReadFloating<double>;
template class CISST_EXPORT vctQtWidgetDynamicVectorReadFloating<float>;
template class CISST_EXPORT vctQtWidgetDynamicVectorReadInteger<int>;
template class CISST_EXPORT vctQtWidgetDynamicVectorReadInteger<unsigned int>;
template class CISST_EXPORT vctQtWidgetDynamicVectorReadInteger<bool>;
#endif // CISST_WINDOWS


class CISST_EXPORT vctQtWidgetDynamicVectorWriteBase: public QObject
{
    Q_OBJECT;
    enum {SLIDER_RESOLUTION = 1000};
public:
    typedef enum {TEXT_WIDGET, SPINBOX_WIDGET, SLIDER_WIDGET} DisplayModeType;
    vctQtWidgetDynamicVectorWriteBase(const DisplayModeType displayMode);
    void SetDisplayMode(const DisplayModeType displayMode);
    QWidget * GetWidget(void);
signals:
    void valueChanged(void);
protected:
    QTableWidget * Table;
    DisplayModeType DisplayMode;
protected slots:
    void SliderValueChangedSlot(int value);
    void DoubleSpinBoxValueChangedSlot(double value);
    void SpinBoxValueChangedSlot(int value);
    void ItemChangedSlot(QTableWidgetItem * item);
};

template <class _elementType>
class CISST_EXPORT vctQtWidgetDynamicVectorWriteFloating: public vctQtWidgetDynamicVectorWriteBase
{
    typedef _elementType value_type;
    int Precision;
    char Format;
    value_type Minimum, Maximum, Step;
public:
    vctQtWidgetDynamicVectorWriteFloating(const DisplayModeType displayMode = TEXT_WIDGET);
    virtual bool SetValue(const vctDynamicVector<value_type> & value, bool blockSignals = true);
    virtual bool GetValue(vctDynamicVector<value_type> & placeHolder) const;
    void SetPrecision(const int precision);
    void SetFormat(const char format);
    void SetRange(const value_type minimum, const value_type maximum);
    void SetStep(const value_type step);
};

template <class _elementType>
class CISST_EXPORT vctQtWidgetDynamicVectorWriteInteger: public vctQtWidgetDynamicVectorWriteBase
{
    typedef _elementType value_type;
    int Base;
    value_type Minimum, Maximum, Step;
public:
    vctQtWidgetDynamicVectorWriteInteger(const DisplayModeType displayMode = TEXT_WIDGET);
    virtual bool SetValue(const vctDynamicVector<value_type> & value, bool blockSignals = true);
    virtual bool GetValue(vctDynamicVector<value_type> & placeHolder) const;
    void SetBase(const int base);
    void SetRange(const value_type minimum, const value_type maximum);
    void SetStep(const value_type step);
};

typedef vctQtWidgetDynamicVectorWriteFloating<double> vctQtWidgetDynamicVectorDoubleWrite;
typedef vctQtWidgetDynamicVectorWriteFloating<float> vctQtWidgetDynamicVectorFloatWrite;
typedef vctQtWidgetDynamicVectorWriteInteger<int> vctQtWidgetDynamicVectorIntWrite;
typedef vctQtWidgetDynamicVectorWriteInteger<unsigned int> vctQtWidgetDynamicVectorUIntWrite;

#if ((CISST_OS == CISST_WINDOWS) && (CISST_COMPILER != CISST_GCC))
template class CISST_EXPORT vctQtWidgetDynamicVectorWriteFloating<double>;
template class CISST_EXPORT vctQtWidgetDynamicVectorWriteFloating<float>;
template class CISST_EXPORT vctQtWidgetDynamicVectorWriteInteger<int>;
template class CISST_EXPORT vctQtWidgetDynamicVectorWriteInteger<unsigned int>;
#endif // CISST_WINDOWS


// -- for bools
#include <QTableWidget>
class CISST_EXPORT vctQtWidgetDynamicVectorBoolWrite: public QTableWidget
{
 public:
    vctQtWidgetDynamicVectorBoolWrite(void);
    virtual bool SetValue(const vctDynamicVector<bool> & value);
    virtual bool GetValue(vctDynamicVector<bool> & placeHolder) const;
};

#endif // _vctQtWidgetDynamicVector_h
