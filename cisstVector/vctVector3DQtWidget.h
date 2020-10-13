/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2017-11-30

  (C) Copyright 2017-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _vctVector3DQtWidget_h
#define _vctVector3DQtWidget_h

// cisst include
#include <cisstVector/vctForwardDeclarations.h>
#include <cisstVector/vctForwardDeclarationsQt.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctTransformationTypes.h>

// Always include last
#include <cisstVector/vctExportQt.h>


/*!
  Widget to visualize a vector using 3 axes in OpenGL
*/
class CISST_EXPORT vctVector3DQtWidget: public vctQtOpenGLBaseWidget
{
    Q_OBJECT;

public:
    vctVector3DQtWidget(QWidget * parent = 0);
    inline ~vctVector3DQtWidget(void) {};

    void SetValue(const vct3 & value);
    void SetAutoResize(const bool autoResize);
    inline const double & MaxNorm(void) const {
        return mMaxNorm;
    }
    inline float & AxisLength(void) {
        return mAxisLength;
    }

protected:
    void ResetOrientation(void);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void keyPressEvent(QKeyEvent * event);
    void initializeGL(void);
    void paintGL(void);
    void resizeGL(int width, int height);

    vct3 mVector;
    vctQuatRot3 mCurrentOrientation, mDeltaOrientation;
    vctInt2 mStartMousePosition;
    bool mAutoResize;
    double mVectorNorm, mMaxNorm;
    float mScale; // = 1 / mMaxNorm
    float mAxisLength;
};

#endif // _vctVector3DQtWidget_h
