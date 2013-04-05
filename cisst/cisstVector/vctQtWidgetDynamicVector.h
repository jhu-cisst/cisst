/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2011-12-08

  (C) Copyright 2011-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

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
template class CISST_EXPORT vctQtWidgetDynamicVectorReadInterger<bool>;
#endif // CISST_WINDOWS

// -- for doubles
class CISST_EXPORT vctQtWidgetDynamicVectorDoubleWrite: public QTableWidget
{
 public:
    vctQtWidgetDynamicVectorDoubleWrite(void);
    virtual bool SetValue(const vctDynamicVector<double> & value);
    virtual bool GetValue(vctDynamicVector<double> & placeHolder) const;
};


// -- for ints
class CISST_EXPORT vctQtWidgetDynamicVectorIntWrite: public QTableWidget
{
 public:
    vctQtWidgetDynamicVectorIntWrite(void);
    virtual bool SetValue(const vctDynamicVector<int> & value);
    virtual bool GetValue(vctDynamicVector<int> & placeHolder) const;
};


// -- for bools
class CISST_EXPORT vctQtWidgetDynamicVectorBoolWrite: public QTableWidget
{
 public:
    vctQtWidgetDynamicVectorBoolWrite(void);
    virtual bool SetValue(const vctDynamicVector<bool> & value);
    virtual bool GetValue(vctDynamicVector<bool> & placeHolder) const;
};

#endif // _vctQtWidgetDynamicVector_h
