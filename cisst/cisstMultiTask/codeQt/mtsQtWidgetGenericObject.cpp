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

#include <cisstMultiTask/mtsQtWidgetGenericObject.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>


mtsQtWidgetGenericObjectRead::mtsQtWidgetGenericObjectRead(void):
    QWidget()
{
    Layout = new QHBoxLayout();
    Layout->setContentsMargins(0, 0, 0, 0);
    setLayout(Layout);
    setFocusPolicy(Qt::StrongFocus);
}

mtsQtWidgetGenericObjectWrite::mtsQtWidgetGenericObjectWrite(void):
    mtsQtWidgetGenericObjectRead()
{
}


// -- mtsInt
mtsQtWidgetIntRead::mtsQtWidgetIntRead(void):
    mtsQtWidgetGenericObjectRead()
{
    Label = new QLabel();
    Label->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    Layout->addWidget(Label);
}

bool mtsQtWidgetIntRead::SetValue(const mtsGenericObject & value)
{
    const mtsInt * intData = dynamic_cast<const mtsInt *>(&value);
    if (intData) {
        QString s;
        s.setNum(intData->GetData());
        Label->setText(s);
        return true;
    }
    return false;
}

mtsQtWidgetIntWrite::mtsQtWidgetIntWrite(void):
    mtsQtWidgetGenericObjectWrite()
{
    SpinBox = new QSpinBox();
    Layout->addWidget(SpinBox);
}

bool mtsQtWidgetIntWrite::SetValue(const mtsGenericObject & value)
{
    const mtsInt * intData = dynamic_cast<const mtsInt *>(&value);
    if (intData) {
        SpinBox->setValue(intData->GetData());
        return true;
    }
    return false;
}

bool mtsQtWidgetIntWrite::GetValue(mtsGenericObject & placeHolder) const
{
    mtsInt * intData = dynamic_cast<mtsInt *>(&placeHolder);
    if (intData) {
        intData->Data = SpinBox->value();
        return true;
    }
    return false;
}


// -- mtsBool
mtsQtWidgetBoolRead::mtsQtWidgetBoolRead(void):
    mtsQtWidgetGenericObjectRead()
{
    Label = new QLabel();
    Label->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    Layout->addWidget(Label);
}

bool mtsQtWidgetBoolRead::SetValue(const mtsGenericObject & value)
{
    const mtsBool * boolData = dynamic_cast<const mtsBool *>(&value);
    if (boolData) {
        Label->setText(boolData->GetData() ? "True" : "False");
        return true;
    }
    return false;
}

mtsQtWidgetBoolWrite::mtsQtWidgetBoolWrite(void):
    mtsQtWidgetGenericObjectWrite()
{
    ComboBox = new QComboBox();
    ComboBox->addItem("True");
    ComboBox->addItem("False");
    Layout->addWidget(ComboBox);
}

bool mtsQtWidgetBoolWrite::SetValue(const mtsGenericObject & value)
{
    const mtsBool * boolData = dynamic_cast<const mtsBool *>(&value);
    if (boolData) {
        ComboBox->setCurrentIndex(boolData->GetData() ? 0 : 1);
        return true;
    }
    return false;
}

bool mtsQtWidgetBoolWrite::GetValue(mtsGenericObject & placeHolder) const
{
    mtsBool * boolData = dynamic_cast<mtsBool *>(&placeHolder);
    if (boolData) {
        boolData->Data = (ComboBox->currentIndex() == 0);
        return true;
    }
    return false;
}


// -- mtsDouble
mtsQtWidgetDoubleRead::mtsQtWidgetDoubleRead(void):
    mtsQtWidgetGenericObjectRead()
{
    Label = new QLabel();
    Label->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    Layout->addWidget(Label);
}

bool mtsQtWidgetDoubleRead::SetValue(const mtsGenericObject & value)
{
    const mtsDouble * doubleData = dynamic_cast<const mtsDouble *>(&value);
    if (doubleData) {
        QString s;
        s.setNum(doubleData->GetData());
        Label->setText(s);
        return true;
    }
    return false;
}

mtsQtWidgetDoubleWrite::mtsQtWidgetDoubleWrite(void):
    mtsQtWidgetGenericObjectWrite()
{
    DoubleSpinBox = new QDoubleSpinBox();
    DoubleSpinBox->setDecimals(5);
    Layout->addWidget(DoubleSpinBox);
}

bool mtsQtWidgetDoubleWrite::SetValue(const mtsGenericObject & value)
{
    const mtsDouble * doubleData = dynamic_cast<const mtsDouble *>(&value);
    if (doubleData) {
        DoubleSpinBox->setValue(doubleData->GetData());
        return true;
    }
    return false;
}

bool mtsQtWidgetDoubleWrite::GetValue(mtsGenericObject & placeHolder) const
{
    mtsDouble * doubleData = dynamic_cast<mtsDouble *>(&placeHolder);
    if (doubleData) {
        doubleData->Data = DoubleSpinBox->value();
        return true;
    }
    return false;
}


// -- mtsStdString
mtsQtWidgetStdStringRead::mtsQtWidgetStdStringRead(void):
    mtsQtWidgetGenericObjectRead()
{
    Label = new QLabel();
    Label->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    Layout->addWidget(Label);
}

bool mtsQtWidgetStdStringRead::SetValue(const mtsGenericObject & value)
{
    const mtsStdString * stdStringData = dynamic_cast<const mtsStdString *>(&value);
    if (stdStringData) {
        Label->setText(stdStringData->GetData().c_str());
        return true;
    }
    return false;
}

mtsQtWidgetStdStringWrite::mtsQtWidgetStdStringWrite(void):
    mtsQtWidgetGenericObjectWrite()
{
    LineEdit = new QLineEdit();
    Layout->addWidget(LineEdit);
}

bool mtsQtWidgetStdStringWrite::SetValue(const mtsGenericObject & value)
{
    const mtsStdString * stdStringData = dynamic_cast<const mtsStdString *>(&value);
    if (stdStringData) {
        LineEdit->setText(stdStringData->GetData().c_str());
        return true;
    }
    return false;
}

bool mtsQtWidgetStdStringWrite::GetValue(mtsGenericObject & placeHolder) const
{
    mtsStdString * stdStringData = dynamic_cast<mtsStdString *>(&placeHolder);
    if (stdStringData) {
        stdStringData->Data = std::string(LineEdit->text().toAscii());
        return true;
    }
    return false;
}
