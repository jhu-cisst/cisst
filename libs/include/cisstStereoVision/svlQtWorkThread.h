/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s):  Balazs P. Vagvolgyi
  Created on: 2011-06-03

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlQtWorkThread_h
#define _svlQtWorkThread_h

#include <QtGui>

// Always include last!
#include <cisstStereoVision/svlExportQt.h>


#define START_QT_ENVIRONMENT \
    int ac=1; char av[]="";\
    QApplication app(ac,reinterpret_cast<char**>(&av));\
    app.setQuitOnLastWindowClosed(false);\
    svlQtWorkThread work_thread;app.exec();}\
    void svlQtWorkThread::run(){{

#define STOP_QT_ENVIRONMENT \
    }QApplication::instance()->exit(0);


class CISST_EXPORT svlQtWorkThread : public QThread
{
    Q_OBJECT

public:
    svlQtWorkThread();
    virtual ~svlQtWorkThread();

protected:
    void run();
};

#endif // _svlQtWorkThread_h

