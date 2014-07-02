/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2013-04-01

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "ExampleClass.h"

#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstVector/vctRandom.h>
#include <cisstVector/vctQtWidgetDynamicVector.h>
#include <cisstVector/vctQtWidgetRotation.h>
#include <cisstVector/vctQtWidgetFrame.h>

ExampleClass::ExampleClass(void):
    QObject()
{
    Frame = new QFrame();
    Layout = new QVBoxLayout();

    DynamicVectorDoubleRead = new vctQtWidgetDynamicVectorDoubleRead();
    DynamicVectorDoubleRead->SetPrecision(5);
    Layout->addWidget(DynamicVectorDoubleRead);

    DynamicVectorDoubleWriteText = new vctQtWidgetDynamicVectorDoubleWrite(vctQtWidgetDynamicVectorDoubleWrite::TEXT_WIDGET);
    Layout->addWidget(DynamicVectorDoubleWriteText);
    connect(DynamicVectorDoubleWriteText, SIGNAL(valueChanged()), this, SLOT(DoubleTextValueChangedSlot()));

    DynamicVectorDoubleWriteSpinBox = new vctQtWidgetDynamicVectorDoubleWrite(vctQtWidgetDynamicVectorDoubleWrite::SPINBOX_WIDGET);
    Layout->addWidget(DynamicVectorDoubleWriteSpinBox);
    connect(DynamicVectorDoubleWriteSpinBox, SIGNAL(valueChanged()), this, SLOT(DoubleSpinBoxValueChangedSlot()));

    DynamicVectorDoubleWriteSlider = new vctQtWidgetDynamicVectorDoubleWrite(vctQtWidgetDynamicVectorDoubleWrite::SLIDER_WIDGET);
    Layout->addWidget(DynamicVectorDoubleWriteSlider);
    connect(DynamicVectorDoubleWriteSlider, SIGNAL(valueChanged()), this, SLOT(DoubleSliderValueChangedSlot()));

    vctDynamicVector<double> vectorDouble(NUMBER_OF_ELEMENTS);
    vctRandom(vectorDouble, -100, 100);
    DynamicVectorDoubleRead->SetValue(vectorDouble);
    DynamicVectorDoubleWriteText->SetValue(vectorDouble);
    DynamicVectorDoubleWriteSpinBox->SetValue(vectorDouble);
    DynamicVectorDoubleWriteSlider->SetValue(vectorDouble);

    DynamicVectorIntRead = new vctQtWidgetDynamicVectorIntRead();
    DynamicVectorIntRead->SetBase(16);
    Layout->addWidget(DynamicVectorIntRead);

    DynamicVectorIntWriteText = new vctQtWidgetDynamicVectorIntWrite(vctQtWidgetDynamicVectorIntWrite::TEXT_WIDGET);
    Layout->addWidget(DynamicVectorIntWriteText);
    connect(DynamicVectorIntWriteText, SIGNAL(valueChanged()), this, SLOT(IntTextValueChangedSlot()));

    DynamicVectorIntWriteSpinBox = new vctQtWidgetDynamicVectorIntWrite(vctQtWidgetDynamicVectorIntWrite::SPINBOX_WIDGET);
    Layout->addWidget(DynamicVectorIntWriteSpinBox);
    connect(DynamicVectorIntWriteSpinBox, SIGNAL(valueChanged()), this, SLOT(IntSpinBoxValueChangedSlot()));

    DynamicVectorIntWriteSlider = new vctQtWidgetDynamicVectorIntWrite(vctQtWidgetDynamicVectorIntWrite::SLIDER_WIDGET);
    Layout->addWidget(DynamicVectorIntWriteSlider);
    connect(DynamicVectorIntWriteSlider, SIGNAL(valueChanged()), this, SLOT(IntSliderValueChangedSlot()));

    vctDynamicVector<int> vectorInt(NUMBER_OF_ELEMENTS);
    vctRandom(vectorInt, -100, 100);
    DynamicVectorIntRead->SetValue(vectorInt);
    DynamicVectorIntWriteText->SetValue(vectorInt);
    DynamicVectorIntWriteSpinBox->SetValue(vectorInt);
    DynamicVectorIntWriteSlider->SetValue(vectorInt);

    RotationDoubleRead = new vctQtWidgetRotationDoubleRead();
    Layout->addWidget(RotationDoubleRead);
    vctRandom(Rotation3D);
    RotationDoubleRead->SetValue(Rotation3D);

    FrameDoubleRead = new vctQtWidgetFrameDoubleRead();
    Layout->addWidget(FrameDoubleRead);
    vctRandom(Frame3D.Rotation());
    vctRandom(Frame3D.Translation(), -10.0, 10.0);
    FrameDoubleRead->SetValue(Frame3D);

    Frame->setLayout(Layout);
    Frame->show();
}

void ExampleClass::DoubleTextValueChangedSlot(void)
{
    vctDynamicVector<double> result(NUMBER_OF_ELEMENTS);
    DynamicVectorDoubleWriteText->GetValue(result);
    DynamicVectorDoubleRead->SetValue(result);
    DynamicVectorDoubleWriteSpinBox->SetValue(result);
    DynamicVectorDoubleWriteSlider->SetValue(result);
}

void ExampleClass::DoubleSpinBoxValueChangedSlot(void)
{
    vctDynamicVector<double> result(NUMBER_OF_ELEMENTS);
    DynamicVectorDoubleWriteSpinBox->GetValue(result);
    DynamicVectorDoubleRead->SetValue(result);
    DynamicVectorDoubleWriteText->SetValue(result);
    DynamicVectorDoubleWriteSlider->SetValue(result);
}

void ExampleClass::DoubleSliderValueChangedSlot(void)
{
    vctDynamicVector<double> result(NUMBER_OF_ELEMENTS);
    DynamicVectorDoubleWriteSlider->GetValue(result);
    DynamicVectorDoubleRead->SetValue(result);
    DynamicVectorDoubleWriteText->SetValue(result);
    DynamicVectorDoubleWriteSpinBox->SetValue(result);
    vctAxAnRot3 axisAngleRotation;
    axisAngleRotation.FromNormalized(Rotation3D);
    if (axisAngleRotation.Axis().at(0) < 0.0) {
        axisAngleRotation.Axis().NegationSelf();
    }
    axisAngleRotation.Angle() = result.at(0) * cmnPI_180;
    Rotation3D.FromNormalized(axisAngleRotation);
    RotationDoubleRead->SetValue(Rotation3D);
}

void ExampleClass::IntTextValueChangedSlot(void)
{
    vctDynamicVector<int> result(NUMBER_OF_ELEMENTS);
    DynamicVectorIntWriteText->GetValue(result);
    DynamicVectorIntRead->SetValue(result);
    DynamicVectorIntWriteSpinBox->SetValue(result);
    DynamicVectorIntWriteSlider->SetValue(result);
}

void ExampleClass::IntSpinBoxValueChangedSlot(void)
{
    vctDynamicVector<int> result(NUMBER_OF_ELEMENTS);
    DynamicVectorIntWriteSpinBox->GetValue(result);
    DynamicVectorIntRead->SetValue(result);
    DynamicVectorIntWriteText->SetValue(result);
    DynamicVectorIntWriteSlider->SetValue(result);
}

void ExampleClass::IntSliderValueChangedSlot(void)
{
    vctDynamicVector<int> result(NUMBER_OF_ELEMENTS);
    DynamicVectorIntWriteSlider->GetValue(result);
    DynamicVectorIntRead->SetValue(result);
    DynamicVectorIntWriteText->SetValue(result);
    DynamicVectorIntWriteSpinBox->SetValue(result);
}
