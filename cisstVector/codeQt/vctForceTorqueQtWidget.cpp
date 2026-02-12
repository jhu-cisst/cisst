/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet, Dorothy Hu
  Created on: 2017-01-20

  (C) Copyright 2017-2021 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstConfig.h>
#include <cisstCommon/cmnPortability.h>

#if (CISST_OS == CISST_WINDOWS)
#include <windows.h>
#endif

#if (CISST_OS == CISST_DARWIN)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <cisstVector/vctForceTorqueQtWidget.h>

// all these widgets should be replaced to use static vectors/matrices
#include <cisstVector/vctQtWidgetDynamicVector.h>
#include <cisstVector/vctDynamicVectorTypes.h>

vctForceTorqueQtWidget::vctForceTorqueQtWidget(const DisplayModeType displayMode):
    DisplayMode(UNDEFINED_WIDGET),
    CurrentWidget(0)
{
    mForce.Zeros();
    mTorque.Zeros();

    QVBoxLayout * forceTorqueLayout = new QVBoxLayout;
    forceTorqueLayout->setContentsMargins(0, 0, 0, 0);

    QGridLayout * gridLayout = new QGridLayout;
    gridLayout->setSpacing(0);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    forceTorqueLayout->addLayout(gridLayout);
    forceTorqueLayout->addStretch();

    gridLayout->addWidget(new QLabel("Force"), 0, 0);
    ForceWidget = new vctQtWidgetDynamicVectorDoubleRead();
    ForceWidget->SetPrecision(4);
    gridLayout->addWidget(ForceWidget, 0, 1);

    gridLayout->addWidget(new QLabel("Torque"), 1, 0);
    TorqueWidget = new vctQtWidgetDynamicVectorDoubleRead();
    TorqueWidget->SetPrecision(4);
    gridLayout->addWidget(TorqueWidget, 1, 1);

    gridLayout->addWidget(new QLabel("Norm"), 2, 0);
    NormWidget = new vctQtWidgetDynamicVectorDoubleRead();
    NormWidget->SetPrecision(4);
    gridLayout->addWidget(NormWidget, 2, 1);

    ForceTorqueWidget = new QWidget();
    ForceTorqueWidget->setLayout(forceTorqueLayout);

    Plot2DWidget = new vctForceTorque2DQtWidget();
    Plot3DWidget = new vctForceTorque3DQtWidget();

    Layout = new QVBoxLayout;
    Layout->setSpacing(0);
    Layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(Layout);
    this->setWindowTitle("vctForceTorqueQtWidget");

    // Set display mode
    SetDisplayMode(displayMode);

    // Context menu
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(ShowContextMenu(const QPoint&)));
}

void vctForceTorqueQtWidget::ShowContextMenu(const QPoint & pos)
{
    QPoint globalPos = this->mapToGlobal(pos);
    QMenu menu;
    QAction * text = new QAction("Text", this);
    QAction * plot2D = new QAction("2D", this);
    QAction * plot3D = new QAction("3D", this);

    if (DisplayMode != TEXT_WIDGET) {
        menu.addAction(text);
    }
    if (DisplayMode != PLOT_2D_WIDGET) {
        menu.addAction(plot2D);
    }
    if (DisplayMode != PLOT_3D_WIDGET) {
        menu.addAction(plot3D);
    }

    QAction * selectedItem = menu.exec(globalPos);
    if (selectedItem) {
        if (selectedItem == text) {
            SetDisplayMode(TEXT_WIDGET);
        } else if (selectedItem == plot2D) {
            SetDisplayMode(PLOT_2D_WIDGET);
        } else if (selectedItem == plot3D) {
            SetDisplayMode(PLOT_3D_WIDGET);
        }
    }
}

void vctForceTorqueQtWidget::UpdateCurrentWidget(void)
{
    // compute the value based on the internal values
    switch (DisplayMode) {
    case TEXT_WIDGET:
        ForceWidget->SetValue(mForce);
        TorqueWidget->SetValue(mTorque);
        NormWidget->SetValue(vct1(mForce.Norm()));
        break;
    case PLOT_2D_WIDGET:
        Plot2DWidget->SetValue(mTime, mForce, mTorque);
        break;
    case PLOT_3D_WIDGET:
        Plot3DWidget->SetValue(mForce, mTorque);
        break;
    default:
        break;
    }
}

void vctForceTorqueQtWidget::SetDisplayMode(const DisplayModeType displayMode)
{
    // should never allow anyone to use undefined
    if (displayMode == UNDEFINED_WIDGET) {
        return;
    }

    // if the mode is unchanged, nothing to do
    if (displayMode == this->DisplayMode) {
        return;
    }

    // set the new display mode
    this->DisplayMode = displayMode;

    // mostly for initialization, there was no widget defined prior this call
    if (CurrentWidget) {
        Layout->removeWidget(CurrentWidget);
        CurrentWidget->hide();
    }

    // set the new current widget, these have been created in the ctor
    switch (displayMode) {
    case TEXT_WIDGET:
        CurrentWidget = ForceTorqueWidget;
        break;
    case PLOT_2D_WIDGET:
        CurrentWidget = Plot2DWidget;
        break;
    case PLOT_3D_WIDGET:
        CurrentWidget = Plot3DWidget;
        break;
    default:
        break;
    }
    Layout->addWidget(CurrentWidget);
    UpdateCurrentWidget();
    CurrentWidget->show();
    repaint();
}
