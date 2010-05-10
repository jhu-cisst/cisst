/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctPlot2DQWidget.h 1238 2010-02-27 03:16:01Z auneri1 $

  Author(s):  Anton Deguet
  Created on: 2010-05-05

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _vctPlot2DQWidget_h
#define _vctPlot2DQWidget_h

#include <QGLWidget>
#include <vctPlot2DGLBase.h>

class vctPlot2DQWidget: public QGLWidget, public vctPlot2DGLBase
{
    Q_OBJECT;

public:
    vctPlot2DQWidget(QWidget * parent);
    ~vctPlot2DQWidget(void) {};

protected:

    /*! Methods required for Qt */
    //@{
    virtual void initializeGL(void);
    virtual void resizeGL(int w, int h);
    virtual void paintGL(void);
    //@}
};

#endif  // _vctPlot2DQWidget_h
