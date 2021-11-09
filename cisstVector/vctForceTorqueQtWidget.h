/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s): Anton Deguet, Dorothy Hu
  Created on: 2017-01-20

  (C) Copyright 2017-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _vctForceTorqueQtWidget_h
#define _vctForceTorqueQtWidget_h

// cisst include
#include <cisstVector/vctForwardDeclarations.h>
#include <cisstVector/vctForwardDeclarationsQt.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctForceTorque2DQtWidget.h>
#include <cisstVector/vctForceTorque3DQtWidget.h>

#include <QWidget>
#include <QtOpenGL>

class QWidget;
class QVBoxLayout;

// Always include last
#include <cisstVector/vctExportQt.h>

/*! Qt Widget to display force/torque using cisstVector. */
class CISST_EXPORT vctForceTorqueQtWidget: public QWidget
{
    Q_OBJECT;

 public:
    /*! Possible display modes.  See SetDisplayMode method.  Please
      note that UNDEFINED_WIDGET should never be used. */
    typedef enum {UNDEFINED_WIDGET,
                  TEXT_WIDGET,  // text display
                  PLOT_2D_WIDGET, // 2D plot
                  PLOT_3D_WIDGET  // 3D display
    } DisplayModeType;

    /*! Constructor.  Default display mode is 2D plot.  See
      also SetDisplayMode. */
    vctForceTorqueQtWidget(const DisplayModeType displayMode = PLOT_3D_WIDGET);

    inline ~vctForceTorqueQtWidget(void) {};

    /*! Set the force and torque.  For 2D plot, time is needed as
      well. */
    void SetValue(const vct3 & force,
                  const vct3 & torque,
                  const double & time = 0.0) {
        mForce.Assign(force);
        mTorque.Assign(torque);
        mTime = time;
        this->UpdateCurrentWidget();
    }

    /*! Set the display mode, i.e. the widget used to represent the
      wrench.  Options are TEXT_WIDGET (just numbers), PLOT_2D_WIDGET
      (x, y, z plot over time), PLOT_3D_WIDGET (3D lines representing
      force/torque direction and amplitude).  Please note that the
      display mode UNDEFINED_WIDGET will be silently ignored. */
    void SetDisplayMode(const DisplayModeType displayMode);

 protected slots:
    /*! Contextual menu showed when the user right clicks on the widget */
    void ShowContextMenu(const QPoint & position);

 protected:
    /*! Current display mode, somewhat redundant with the
      CurrentWidget pointer.  The current widget should always be
      set by changing the DisplayMode. */
    DisplayModeType DisplayMode;

    /*! Update the content of the current widget.  This method is
      called when the user provides a new wrench with SetValue or
      when the widget used to display the wrench is changed using
      SetDisplayMode. */
    void UpdateCurrentWidget(void);

    /*! Internal representation for the wrench/time to display. */
    vct3 mForce, mTorque;
    double mTime;

    // Text widgets
    vctQtWidgetDynamicVectorDoubleRead * ForceWidget;
    vctQtWidgetDynamicVectorDoubleRead * TorqueWidget;
    vctQtWidgetDynamicVectorDoubleRead * NormWidget;
    QWidget * ForceTorqueWidget;

    // 2D/3D widgets
    vctForceTorque2DQtWidget * Plot2DWidget;
    vctForceTorque3DQtWidget * Plot3DWidget;

    // current widget
    QWidget * CurrentWidget;
    QVBoxLayout * Layout;
};

#endif  // _vctForceTorqueQtWidget_h
