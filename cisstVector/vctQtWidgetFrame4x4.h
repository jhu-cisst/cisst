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

#ifndef _vctQtWidgetFrame4x4_h
#define _vctQtWidgetFrame4x4_h

#include <QTableWidget>

#include <cisstVector/vctFrame4x4.h>
#include <cisstVector/vctFrameBase.h>

// Always include last
#include <cisstVector/vctExportQt.h>

// -- for doubles
class CISST_EXPORT vctQtWidgetFrame4x4DoubleRead: public QTableWidget
{
    Q_OBJECT;
 public:
    vctQtWidgetFrame4x4DoubleRead(void);
    virtual bool SetValue(const vctFrame4x4<double> & value);

    template <class _rotationType>
    bool SetValue(const vctFrameBase<_rotationType> & value) {
        return SetValue(vctFrame4x4<double>(value));
    }
};

class CISST_EXPORT vctQtWidgetFrame4x4DoubleWrite: public QTableWidget
{
    Q_OBJECT;
 public:
    vctQtWidgetFrame4x4DoubleWrite(void);
    virtual bool SetValue(const vctFrame4x4<double> & value);
    virtual bool GetValue(vctFrame4x4<double> & placeHolder) const;
};

#endif // _vctQtWidgetFrame4x4_h
