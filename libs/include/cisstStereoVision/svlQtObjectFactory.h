/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

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

#ifndef _svlQtObjectFactory_h
#define _svlQtObjectFactory_h

#include <cisstOSAbstraction/osaThread.h>
#include <QApplication>
#include <QObject>

// Always include last!
#include <cisstStereoVision/svlExportQt.h>


// Forward declaration
void* __app_funct();

//    int ac=1;
//    char av[]="";
//    QApplication app(ac,reinterpret_cast<char**>(&av));

#define START_QT_ENVIRONMENT \
    QApplication app(argc,argv);\
    app.setQuitOnLastWindowClosed(false);\
    svlQtObjectFactory::Init();\
    osaThread thread;\
    thread.Create(__app_funct);\
    app.exec();\
    return 0;\
    }void* __app_funct(){{

#define STOP_QT_ENVIRONMENT \
    }QApplication::instance()->exit(0);


// Forward declarations
class cmnGenericObject;
class osaCriticalSection;

class CISST_EXPORT svlQtObjectFactory : QObject
{
    Q_OBJECT

public:
    static void Init();
    static cmnGenericObject* Create(const std::string & classname);
    static void Delete(cmnGenericObject* obj);

private slots:
    void QSlotCreate();
    void QSlotDelete();

private:
    static svlQtObjectFactory* GetInstance();
    svlQtObjectFactory();
    ~svlQtObjectFactory();

    osaCriticalSection* CS;

    std::string ClassName;
    cmnGenericObject* Object;
};

#endif // _svlQtObjectFactory_h

