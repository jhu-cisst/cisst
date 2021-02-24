/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2010-05-05

  (C) Copyright 2010-2021 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _vctPlot2DOpenGLQtWidget_h
#define _vctPlot2DOpenGLQtWidget_h

#include <cisstVector/vctForwardDeclarationsQt.h>
#include <cisstVector/vctPlot2DOpenGL.h>

// Always include last
#include <cisstVector/vctExportQt.h>

class CISST_EXPORT vctPlot2DOpenGLQtWidget: public vctQtOpenGLBaseWidget, public vctPlot2DOpenGL
{
    Q_OBJECT;

public:
    vctPlot2DOpenGLQtWidget(QWidget * parent = 0);
    ~vctPlot2DOpenGLQtWidget(void) {};

    inline void SetDisplayYRangeScale(vctPlot2DBase::Scale * scale) {
        DisplayYRangeScale = scale;
    }

protected:

    /*! Methods required for Qt */
    //@{
    virtual void initializeGL(void);
    virtual void resizeGL(int w, int h);
    virtual void paintGL(void);
    virtual void keyPressEvent(QKeyEvent * event);
    //@}

public slots:
    void ShowContextMenu(const QPoint & pos);
    void FreezeSlot(bool);
    void FitXSlot(void);
    void FitYSlot(void);
    void SetContinuousFitXSlot(bool);
    void SetContinuousFitYSlot(bool);
    void SetContinuousExpandYSlot(bool);
    void SetContinuousExpandYResetSlot(void);

 protected:
    vctPlot2DBase::Scale * DisplayYRangeScale;
};

#endif  // _vctPlot2DOpenGLQtWidget_h
