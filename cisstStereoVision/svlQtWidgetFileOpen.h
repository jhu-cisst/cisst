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

#ifndef _svlQtWidgetFileOpen_h
#define _svlQtWidgetFileOpen_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstOSAbstraction/osaThreadSignal.h>

#include <QObject>
#include <string>

// Always include last!
#include <cisstStereoVision/svlExportQt.h>

// Forward declarations
class QFileDialog;


class CISST_EXPORT svlQtWidgetFileOpen : public QObject, public cmnGenericObject
{
    Q_OBJECT
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT)

public:
    svlQtWidgetFileOpen();
    svlQtWidgetFileOpen(const svlQtWidgetFileOpen& other);
    ~svlQtWidgetFileOpen();

    static svlQtWidgetFileOpen* New();
    static svlQtWidgetFileOpen* New(const std::string & kind, const std::string & extensions, bool save = false);
    void Delete();

    bool WaitForClose();

    bool Create(const std::string & kind, const std::string & extensions, bool save = false);
    std::string GetFilePath();
    bool Destroy();

protected slots:
    void QSlotCreate();
    void QSlotDestroy();

private:
    QFileDialog* Dialog;

    std::string FilePath;

    std::string argKind;
    std::string argExtensions;
    bool        argSave;
    bool        retSuccess;

    osaThreadSignal ClosedEvent;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlQtWidgetFileOpen)

#endif // _svlQtWidgetFileOpen_h

