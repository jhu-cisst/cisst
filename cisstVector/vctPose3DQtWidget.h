/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2020-09-04

  (C) Copyright 2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _vctPose3DQtWidget_h
#define _vctPose3DQtWidget_h

#include <QGridLayout>

// cisst include
#include <cisstVector/vctForwardDeclarations.h>
#include <cisstVector/vctForwardDeclarationsQt.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctTransformationTypes.h>
#include <cisstVector/vctBoundingBox3.h>

// Always include last
#include <cisstVector/vctExportQt.h>

class vctPose3DQtWidgetView;
class QTableWidget;

/*!  Widget to visualize a 3D position by projecting along x, y or z
  axis.
*/
class CISST_EXPORT vctPose3DQtWidget: public QWidget
{
    Q_OBJECT;

public:
    vctPose3DQtWidget(QWidget * parent = 0);
    inline ~vctPose3DQtWidget(void) {};

    void SetPrismaticRevoluteFactors(const double & prismatic, const double & revolute);

    /*! Add a pose to plot using x, y and z coordinates. */
    void SetValue(const vct3 & value);

    /*! Add a pose using a frame.  The rotation part is ignored. */
    inline void SetValue(const vctFrm3 & value) {
        SetValue(value.Translation());
    }

    void Clear(void);

    typedef std::list<vct3> PosesType;
    typedef std::list<vctPose3DQtWidgetView *> ViewsType;
protected:
    QGridLayout * mLayout;
    void keyPressEvent(QKeyEvent * event);

    QTableWidget * mTable;
    int mTableRow;
    ViewsType mViews;
    PosesType mPoses;
    vctBoundingBox3 mBB;

    double mPrismaticFactor;
};

class vctPose3DQtWidgetView: public vctQtOpenGLBaseWidget
{
    Q_OBJECT;
public:
    typedef vctPose3DQtWidget::PosesType PosesType;

    vctPose3DQtWidgetView(QWidget * parent, PosesType * poses, vctBoundingBox3 * bb);
    inline ~vctPose3DQtWidgetView(void) {};

    void SetPrismaticRevoluteFactors(const double & prismatic, const double & revolute);
    void SetAutoResize(const bool autoResize);
    void ResetSize(void);
    void SetDimensions(const size_t x, const size_t y);

protected:
    void keyPressEvent(QKeyEvent * event);
    void initializeGL(void);
    void paintGL(void);
    void resizeGL(int width, int height);

    double mPrismaticFactor;

    // pointers to shared data
    PosesType * mPoses;
    vctBoundingBox3 * mBB;
    bool mAutoResize;

    // dimensions to plot
    size_t mX, mY;

    // viewport
    vctDouble2 mViewportTranslation, mViewport;
    double mViewportScale;
};


#endif // _vctPose3DQtWidget_h
