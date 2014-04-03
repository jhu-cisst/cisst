/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs P. Vagvolgyi
  Created on: 2011-06-15

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlQtWidgetFramerate_h
#define _svlQtWidgetFramerate_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstVector/vctDynamicVectorTypes.h>

#include <QObject>
#include <QString>

// Always include last!
#include <cisstStereoVision/svlExportQt.h>

// Forward declarations
class svlQtDialog;
class Ui_WidgetFramerate;


class CISST_EXPORT svlQtWidgetFramerate: public QObject, public cmnGenericObject
{
    Q_OBJECT
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT)

public:
    svlQtWidgetFramerate();
    svlQtWidgetFramerate(const svlQtWidgetFramerate& other);
    ~svlQtWidgetFramerate();

    static svlQtWidgetFramerate* New(bool create = true);
    void Delete();

    bool WaitForClose();

    bool Create();
    double GetFramerate();
    bool Destroy();

protected:
    svlQtDialog *Dialog;
    Ui_WidgetFramerate *UIWidget;

protected slots:
    void QSlotCustomToggled(bool checked);
    void QSlotCustomTextEdited(const QString & text);

    void QSlotCreate();
    void QSlotGetFramerate();
    void QSlotDestroy();

private:
    QString CustomText;
    double Framerate;
    bool   retSuccess;
};

CMN_DECLARE_SERVICES_INSTANTIATION(svlQtWidgetFramerate)

#endif // _svlQtWidgetFramerate_h

