/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Praneeth Sadda
  Created on: 2012-02-09

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _vctQtWidgetDynamicMatrix_h
#define _vctQtWidgetDynamicMatrix_h

#include <QTableWidget>

#include <cisstVector/vctDynamicMatrix.h>

// Always include last
#include <cisstVector/vctExportQt.h>


// -- for doubles
class CISST_EXPORT vctQtWidgetDynamicMatrixDoubleRead: public QTableWidget
{
    Q_OBJECT;
 public:
    vctQtWidgetDynamicMatrixDoubleRead(void);
    virtual bool SetValue(const vctDynamicMatrix<double> & value);
};

class CISST_EXPORT vctQtWidgetDynamicMatrixDoubleWrite: public QTableWidget
{
    Q_OBJECT;
 public:
    vctQtWidgetDynamicMatrixDoubleWrite(void);
    virtual bool SetValue(const vctDynamicMatrix<double> & value);
    virtual bool GetValue(vctDynamicMatrix<double> & placeHolder) const;
};


// -- for ints
class CISST_EXPORT vctQtWidgetDynamicMatrixIntRead: public QTableWidget
{
    Q_OBJECT;
 public:
    vctQtWidgetDynamicMatrixIntRead(void);
    virtual bool SetValue(const vctDynamicMatrix<int> & value);
};

class CISST_EXPORT vctQtWidgetDynamicMatrixIntWrite: public QTableWidget
{
    Q_OBJECT;
 public:
    vctQtWidgetDynamicMatrixIntWrite(void);
    virtual bool SetValue(const vctDynamicMatrix<int> & value);
    virtual bool GetValue(vctDynamicMatrix<int> & placeHolder) const;
};


// -- for bools
class CISST_EXPORT vctQtWidgetDynamicMatrixBoolRead: public QTableWidget
{
    Q_OBJECT;
 public:
    vctQtWidgetDynamicMatrixBoolRead(void);
    virtual bool SetValue(const vctDynamicMatrix<bool> & value);
};

class CISST_EXPORT vctQtWidgetDynamicMatrixBoolWrite: public QTableWidget
{
    Q_OBJECT;
 public:
    vctQtWidgetDynamicMatrixBoolWrite(void);
    virtual bool SetValue(const vctDynamicMatrix<bool> & value);
    virtual bool GetValue(vctDynamicMatrix<bool> & placeHolder) const;
};

#endif // _vctQtWidgetDynamicMatrix_h
