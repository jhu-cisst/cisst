/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Praneeth Sadda, Anton Deguet
  Created on: 2011-11-11

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstMultiTask/mtsArgumentWidget.h>
#include <cisstMultiTask/mtsArgumentWidgets.h>

#include <QHBoxLayout>
#include <QEvent>
#include <QFocusEvent>

FocusForwarder::FocusForwarder(QWidget** focusReceiver)
    : QObject(), focused(focusReceiver)
{

}

bool FocusForwarder::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::FocusIn) {
        QFocusEvent* focusEvent = dynamic_cast<QFocusEvent*>(event);
        (*focused)->setFocus(focusEvent->reason());
        focusEvent->accept();
        return true;
    } else {
        // Standard event processing
        return QObject::eventFilter(obj, event);
    }
}

ArgumentWidget::ArgumentWidget(void)
    : QWidget()
{
    Layout = new QHBoxLayout();
    Layout->setContentsMargins(0, 0, 0, 0);
    setLayout(Layout);
    setFocusPolicy(Qt::StrongFocus);
    //installEventFilter(new FocusForwarder(&focused));
}

ArgumentWidget* ArgumentWidget::CreateIntInputWidget(void)
{
    return new IntInputWidget();
}

ArgumentWidget* ArgumentWidget::CreateBoolInputWidget(void)
{
    return new BoolInputWidget();
}

ArgumentWidget* ArgumentWidget::CreateDoubleInputWidget(void)
{
    return new DoubleInputWidget();
}

ArgumentWidget* ArgumentWidget::CreateStdStringInputWidget(void)
{
    return new StdStringInputWidget();
}

ArgumentWidget* ArgumentWidget::CreateIntOutputWidget(void)
{
    return new IntOutputWidget();
}

ArgumentWidget* ArgumentWidget::CreateBoolOutputWidget(void)
{
    return new BoolOutputWidget();
}

ArgumentWidget* ArgumentWidget::CreateDoubleOutputWidget(void)
{
    return new DoubleOutputWidget();
}

ArgumentWidget* ArgumentWidget::CreateStdStringOutputWidget(void)
{
    return new StdStringOutputWidget();
}

IntInputWidget::IntInputWidget(void)
    : ArgumentWidget()
{
    spinBox = new QSpinBox();
    Layout->addWidget(spinBox);
}

void IntInputWidget::SetValue(const mtsGenericObject & value)
{
    const mtsInt* intData;
    if ((intData = dynamic_cast<const mtsInt*>(&value))) {
        spinBox->setValue(intData->GetData());
    }
}

const mtsGenericObject & IntInputWidget::GetValue(void) const
{
    mtsInt value(spinBox->value());
    data.Assign(value);
    return data;
}

mtsGenericObject & IntInputWidget::GetValueRef(void)
{
    return data;
}

BoolInputWidget::BoolInputWidget(void)
    : ArgumentWidget()
{
    comboBox = new QComboBox();
    comboBox->addItem("True");
    comboBox->addItem("False");
    Layout->addWidget(comboBox);
    Focused = comboBox;
}

void BoolInputWidget::SetValue(const mtsGenericObject& value)
{
    const mtsBool* boolData;
    if ((boolData = dynamic_cast<const mtsBool*>(&value))) {
        comboBox->setCurrentIndex(boolData->GetData() == true? 0 : 1);
    }
}

const mtsGenericObject& BoolInputWidget::GetValue(void) const
{
    mtsBool value(comboBox->currentIndex() == 0);
    data.Assign(value);
    return data;
}

mtsGenericObject& BoolInputWidget::GetValueRef(void)
{
    return data;
}

DoubleInputWidget::DoubleInputWidget(void)
    : ArgumentWidget()
{
    spinBox = new QDoubleSpinBox();
    spinBox->setDecimals(5);
    Layout->addWidget(spinBox);
    Focused = spinBox;
}

void DoubleInputWidget::SetValue(const mtsGenericObject& value)
{
    const mtsDouble* doubleData;
    if ((doubleData = dynamic_cast<const mtsDouble*>(&value))) {
        spinBox->setValue(doubleData->GetData());
    }
}

const mtsGenericObject& DoubleInputWidget::GetValue(void) const
{
    mtsDouble value(spinBox->value());
    data.Assign(value);
    return data;
}

mtsGenericObject& DoubleInputWidget::GetValueRef(void)
{
    return data;
}

StdStringInputWidget::StdStringInputWidget(void)
    : ArgumentWidget()
{
    QLayout* layout = new QHBoxLayout();
    lineEdit = new QLineEdit();
    Layout->addWidget(lineEdit);
    Focused = lineEdit;
}

void StdStringInputWidget::SetValue(const mtsGenericObject& value)
{
    const mtsStdString* stdStringData;
    if ((stdStringData = dynamic_cast<const mtsStdString*>(&value))) {
        lineEdit->setText(stdStringData->GetData().c_str());
    }
}

const mtsGenericObject& StdStringInputWidget::GetValue(void) const
{
    mtsStdString value(std::string(lineEdit->text().toAscii()));
    data.Assign(value);
    return data;
}

mtsGenericObject& StdStringInputWidget::GetValueRef(void)
{
    return data;
}

OutputWidget::OutputWidget(void)
    : ArgumentWidget()
{
    label = new QLabel();
    label->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    Layout->addWidget(label);
    Focused = label;
}

void IntOutputWidget::SetValue(const mtsGenericObject& value)
{
    const mtsInt* intData;
    if ((intData = dynamic_cast<const mtsInt*>(&value))) {
        QString s;
        s.setNum(intData->GetData());
        label->setText(s);
    }
}

mtsGenericObject& IntOutputWidget::GetValueRef(void)
{
    return data;
}

const mtsGenericObject& IntOutputWidget::GetValue(void) const
{
    mtsInt value(label->text().toInt());
    data.Assign(value);
    return data;
}

void BoolOutputWidget::SetValue(const mtsGenericObject& value)
{
    const mtsBool* boolData;
    if ((boolData = dynamic_cast<const mtsBool*>(&value))) {
        label->setText(boolData->GetData() == true ? "True" : "False");
    }
}

const mtsGenericObject& BoolOutputWidget::GetValue(void) const
{
    mtsBool value(label->text() == "True");
    data.Assign(value);
    return data;
}

mtsGenericObject& BoolOutputWidget::GetValueRef(void)
{
    return data;
}

void DoubleOutputWidget::SetValue(const mtsGenericObject& value)
{
    const mtsDouble* doubleData;
    if ((doubleData = dynamic_cast<const mtsDouble*>(&value))) {
        QString s;
        s.setNum(doubleData->GetData());
        label->setText(s);
    }
}

const mtsGenericObject& DoubleOutputWidget::GetValue(void) const
{
    mtsDouble value(label->text().toDouble());
    data.Assign(value);
    return data;
}

mtsGenericObject& DoubleOutputWidget::GetValueRef(void)
{
    return data;
}

void StdStringOutputWidget::SetValue(const mtsGenericObject& value)
{
    const mtsStdString* stdStringData;
    if ((stdStringData = dynamic_cast<const mtsStdString*>(&value))) {
        label->setText(stdStringData->GetData().c_str());
    }
}

const mtsGenericObject& StdStringOutputWidget::GetValue(void) const
{
    mtsStdString value(std::string(label->text().toAscii()));
    data.Assign(value);
    return data;
}

mtsGenericObject& StdStringOutputWidget::GetValueRef(void)
{
    return data;
}
