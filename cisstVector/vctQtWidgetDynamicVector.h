/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2011-12-08

  (C) Copyright 2011-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _vctQtWidgetDynamicVector_h
#define _vctQtWidgetDynamicVector_h

#include <QTableWidget>

#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctForwardDeclarationsQt.h>

// Always include last
#include <cisstVector/vctExportQt.h>

class CISST_EXPORT vctQtWidgetDynamicVectorReadBase: public QTableWidget
{
    Q_OBJECT;
public:
    vctQtWidgetDynamicVectorReadBase(void);
};

template <class _elementType>
class CISST_EXPORT vctQtWidgetDynamicVectorReadFloating: public vctQtWidgetDynamicVectorReadBase
{
    typedef _elementType value_type;
    int Precision;
    char Format;
public:
    vctQtWidgetDynamicVectorReadFloating(void);
    virtual bool SetValue(const vctDynamicConstVectorRef<value_type> & value);
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
    virtual bool SetValue(const vctDynamicConstVectorRef<value_type> & value);
    void SetBase(const int base);
};

#if (CISST_OS == CISST_WINDOWS) && defined(CISST_COMPILER_IS_MSVC)
#pragma warning ( disable : 4661 )
template class CISST_EXPORT vctQtWidgetDynamicVectorReadFloating<double>;
template class CISST_EXPORT vctQtWidgetDynamicVectorReadFloating<float>;
template class CISST_EXPORT vctQtWidgetDynamicVectorReadInteger<int>;
template class CISST_EXPORT vctQtWidgetDynamicVectorReadInteger<unsigned int>;
template class CISST_EXPORT vctQtWidgetDynamicVectorReadInteger<bool>;
#endif // CISST_WINDOWS


class CISST_EXPORT vctQtWidgetDynamicVectorWriteBase: public QTableWidget
{
    Q_OBJECT;
public:
    typedef enum {TEXT_WIDGET, SPINBOX_WIDGET, SLIDER_WIDGET} DisplayModeType;
    vctQtWidgetDynamicVectorWriteBase(const DisplayModeType displayMode);
signals:
    void valueChanged(void);
protected:
    enum {SLIDER_RESOLUTION = 1000};
    DisplayModeType DisplayMode;
protected slots:
    void SliderValueChangedSlot(int value);
    void DoubleSpinBoxEditingFinishedSlot(void);
    void SpinBoxEditingFinishedSlot(void);
    void ItemChangedSlot(QTableWidgetItem * item);
};

template <class _elementType>
class CISST_EXPORT vctQtWidgetDynamicVectorWriteFloating: public vctQtWidgetDynamicVectorWriteBase
{
    typedef _elementType value_type;
    int Precision;
    char Format;
    value_type Minimum, Maximum, Step;
    vctDynamicVector<value_type> Minimums, Maximums;
public:
    vctQtWidgetDynamicVectorWriteFloating(const DisplayModeType displayMode = TEXT_WIDGET);
    virtual bool SetValue(const vctDynamicVector<value_type> & value, bool blockSignals = true);
    virtual bool GetValue(vctDynamicVector<value_type> & placeHolder) const;
    void SetPrecision(const int precision);
    void SetFormat(const char format);
    void SetRange(const value_type minimum, const value_type maximum);
    void SetRange(const vctDynamicVector<value_type> & minimums, const vctDynamicVector<value_type> & maximums);
    void SetStep(const value_type step);
protected:
    void UpdateWidgetPrecision(void);
    void UpdateWidgetRange(void);
    value_type GetMinimum(const size_t index) const;
    value_type GetMaximum(const size_t index) const;
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

#if (CISST_OS == CISST_WINDOWS) && defined(CISST_COMPILER_IS_MSVC)
#pragma warning ( disable : 4661 )
template class CISST_EXPORT vctQtWidgetDynamicVectorWriteFloating<double>;
template class CISST_EXPORT vctQtWidgetDynamicVectorWriteFloating<float>;
template class CISST_EXPORT vctQtWidgetDynamicVectorWriteInteger<int>;
template class CISST_EXPORT vctQtWidgetDynamicVectorWriteInteger<unsigned int>;
#endif // CISST_WINDOWS


// -- for bools
class CISST_EXPORT vctQtWidgetDynamicVectorBoolWrite: public QTableWidget
{
    Q_OBJECT;
 public:
    vctQtWidgetDynamicVectorBoolWrite(void);
    virtual bool SetValue(const vctDynamicVector<bool> & value, bool blockSignals = true);
    virtual bool GetValue(vctDynamicVector<bool> & placeHolder) const;
signals:
    bool valueChanged(void);
protected slots:
    void ValueChangedSlot(bool value);
};

#endif // _vctQtWidgetDynamicVector_h
