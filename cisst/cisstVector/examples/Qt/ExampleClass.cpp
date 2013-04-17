/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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

ExampleClass::ExampleClass(void):
    QObject()
{
    frame = new QFrame();
    layout = new QVBoxLayout();

    dynamicVectorDoubleRead = new vctQtWidgetDynamicVectorDoubleRead();
    dynamicVectorDoubleRead->SetPrecision(5);
    layout->addWidget(dynamicVectorDoubleRead->GetWidget());

    dynamicVectorDoubleWriteText = new vctQtWidgetDynamicVectorDoubleWrite(vctQtWidgetDynamicVectorDoubleWrite::TEXT_WIDGET);
    layout->addWidget(dynamicVectorDoubleWriteText->GetWidget());
    connect(dynamicVectorDoubleWriteText, SIGNAL(valueChanged()), this, SLOT(DoubleTextValueChangedSlot()));

    dynamicVectorDoubleWriteSpinBox = new vctQtWidgetDynamicVectorDoubleWrite(vctQtWidgetDynamicVectorDoubleWrite::SPINBOX_WIDGET);
    layout->addWidget(dynamicVectorDoubleWriteSpinBox->GetWidget());
    connect(dynamicVectorDoubleWriteSpinBox, SIGNAL(valueChanged()), this, SLOT(DoubleSpinBoxValueChangedSlot()));

    dynamicVectorDoubleWriteSlider = new vctQtWidgetDynamicVectorDoubleWrite(vctQtWidgetDynamicVectorDoubleWrite::SLIDER_WIDGET);
    layout->addWidget(dynamicVectorDoubleWriteSlider->GetWidget());
    connect(dynamicVectorDoubleWriteSlider, SIGNAL(valueChanged()), this, SLOT(DoubleSliderValueChangedSlot()));

    vctDynamicVector<double> vectorDouble(NUMBER_OF_ELEMENTS);
    vctRandom(vectorDouble, -100, 100);
    dynamicVectorDoubleRead->SetValue(vectorDouble);
    dynamicVectorDoubleWriteText->SetValue(vectorDouble);
    dynamicVectorDoubleWriteSpinBox->SetValue(vectorDouble);
    dynamicVectorDoubleWriteSlider->SetValue(vectorDouble);

    dynamicVectorIntRead = new vctQtWidgetDynamicVectorIntRead();
    dynamicVectorIntRead->SetBase(16);
    layout->addWidget(dynamicVectorIntRead->GetWidget());

    dynamicVectorIntWriteText = new vctQtWidgetDynamicVectorIntWrite(vctQtWidgetDynamicVectorIntWrite::TEXT_WIDGET);
    layout->addWidget(dynamicVectorIntWriteText->GetWidget());
    connect(dynamicVectorIntWriteText, SIGNAL(valueChanged()), this, SLOT(IntTextValueChangedSlot()));

    dynamicVectorIntWriteSpinBox = new vctQtWidgetDynamicVectorIntWrite(vctQtWidgetDynamicVectorIntWrite::SPINBOX_WIDGET);
    layout->addWidget(dynamicVectorIntWriteSpinBox->GetWidget());
    connect(dynamicVectorIntWriteSpinBox, SIGNAL(valueChanged()), this, SLOT(IntSpinBoxValueChangedSlot()));

    dynamicVectorIntWriteSlider = new vctQtWidgetDynamicVectorIntWrite(vctQtWidgetDynamicVectorIntWrite::SLIDER_WIDGET);
    layout->addWidget(dynamicVectorIntWriteSlider->GetWidget());
    connect(dynamicVectorIntWriteSlider, SIGNAL(valueChanged()), this, SLOT(IntSliderValueChangedSlot()));

    vctDynamicVector<int> vectorInt(NUMBER_OF_ELEMENTS);
    vctRandom(vectorInt, -100, 100);
    dynamicVectorIntRead->SetValue(vectorInt);
    dynamicVectorIntWriteText->SetValue(vectorInt);
    dynamicVectorIntWriteSpinBox->SetValue(vectorInt);
    dynamicVectorIntWriteSlider->SetValue(vectorInt);

    frame ->setLayout(layout);
    frame->show();
}

void ExampleClass::DoubleTextValueChangedSlot(void)
{
    vctDynamicVector<double> result(NUMBER_OF_ELEMENTS);
    dynamicVectorDoubleWriteText->GetValue(result);
    dynamicVectorDoubleRead->SetValue(result);
    dynamicVectorDoubleWriteSpinBox->SetValue(result);
    dynamicVectorDoubleWriteSlider->SetValue(result);
}

void ExampleClass::DoubleSpinBoxValueChangedSlot(void)
{
    vctDynamicVector<double> result(NUMBER_OF_ELEMENTS);
    dynamicVectorDoubleWriteSpinBox->GetValue(result);
    dynamicVectorDoubleRead->SetValue(result);
    dynamicVectorDoubleWriteText->SetValue(result);
    dynamicVectorDoubleWriteSlider->SetValue(result);
}

void ExampleClass::DoubleSliderValueChangedSlot(void)
{
    vctDynamicVector<double> result(NUMBER_OF_ELEMENTS);
    dynamicVectorDoubleWriteSlider->GetValue(result);
    dynamicVectorDoubleRead->SetValue(result);
    dynamicVectorDoubleWriteText->SetValue(result);
    dynamicVectorDoubleWriteSpinBox->SetValue(result);
}

void ExampleClass::IntTextValueChangedSlot(void)
{
    vctDynamicVector<int> result(NUMBER_OF_ELEMENTS);
    dynamicVectorIntWriteText->GetValue(result);
    dynamicVectorIntRead->SetValue(result);
    dynamicVectorIntWriteSpinBox->SetValue(result);
    dynamicVectorIntWriteSlider->SetValue(result);
}

void ExampleClass::IntSpinBoxValueChangedSlot(void)
{
    vctDynamicVector<int> result(NUMBER_OF_ELEMENTS);
    dynamicVectorIntWriteSpinBox->GetValue(result);
    dynamicVectorIntRead->SetValue(result);
    dynamicVectorIntWriteText->SetValue(result);
    dynamicVectorIntWriteSlider->SetValue(result);
}

void ExampleClass::IntSliderValueChangedSlot(void)
{
    vctDynamicVector<int> result(NUMBER_OF_ELEMENTS);
    dynamicVectorIntWriteSlider->GetValue(result);
    dynamicVectorIntRead->SetValue(result);
    dynamicVectorIntWriteText->SetValue(result);
    dynamicVectorIntWriteSpinBox->SetValue(result);
}
