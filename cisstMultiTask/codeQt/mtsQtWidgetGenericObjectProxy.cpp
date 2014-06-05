/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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

#include <cisstMultiTask/mtsQtWidgetGenericObjectProxy.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>
#include <cisstMultiTask/mtsStateIndex.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>

// -- mtsInt
mtsQtWidgetIntRead::mtsQtWidgetIntRead(void):
    mtsQtWidgetGenericObjectRead()
{
    Label = new QLabel();
    Label->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    Layout->addWidget(Label);
    Layout->addStretch();
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
    Layout->addStretch();
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
    Layout->addStretch();
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
    Layout->addStretch();
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
    Layout->addStretch();
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
    Layout->addStretch();
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
    Layout->addStretch();
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
    Layout->addStretch();
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
        stdStringData->Data = std::string(LineEdit->text().toLatin1());
        return true;
    }
    return false;
}


// -- mtsStateIndex

mtsQtWidgetStateIndexRead::mtsQtWidgetStateIndexRead(void):
    mtsQtWidgetGenericObjectRead()
{
    Label = new QLabel();
    Label->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    Layout->addWidget(Label);
    Layout->addStretch();
}

bool mtsQtWidgetStateIndexRead::SetValue(const mtsGenericObject & value)
{
    const mtsStateIndex * stateIndexData = dynamic_cast<const mtsStateIndex *>(&value);
    if (stateIndexData) {
        Label->setText(stateIndexData->ToString().c_str());
        return true;
    }
    return false;
}

mtsQtWidgetStateIndexWrite::mtsQtWidgetStateIndexWrite(void):
    mtsQtWidgetGenericObjectWrite()
{
    TableWidget = new QTableWidget();

    TableWidget->setRowCount(1);
    TableWidget->setColumnCount(3);

    QStringList members;
    members << "Index" << "Ticks" << "Length";

    TableWidget->setHorizontalHeaderLabels(members);

    IndexSpinBox = new QSpinBox();
    TicksSpinBox = new QDoubleSpinBox();
    LengthSpinBox = new QDoubleSpinBox();

    TableWidget->setCellWidget(0, 0, IndexSpinBox);
    TableWidget->setCellWidget(0, 1, TicksSpinBox);
    TableWidget->setCellWidget(0, 2, LengthSpinBox);
    TableWidget->verticalHeader()->hide();

    int verticalHeight = TableWidget->horizontalHeader()->sizeHint().height() + TableWidget->verticalHeader()->sizeHint().height();

    TableWidget->setFixedHeight(verticalHeight);

    Layout->addWidget(TableWidget);
    Layout->addStretch();
}

bool mtsQtWidgetStateIndexWrite::SetValue(const mtsGenericObject & value)
{
    const mtsStateIndex * stateIndexData = dynamic_cast<const mtsStateIndex *>(&value);
    if (stateIndexData) {
        IndexSpinBox->setValue(stateIndexData->Index());
        TicksSpinBox->setValue(stateIndexData->Ticks());
        LengthSpinBox->setValue(stateIndexData->Length());
        return true;
    }
    return false;
}

bool mtsQtWidgetStateIndexWrite::GetValue(mtsGenericObject & placeHolder) const
{
    mtsStateIndex * stateIndexData = dynamic_cast<mtsStateIndex *>(&placeHolder);
    if (stateIndexData) {
        mtsStateIndex newStateIndexData(
                                        0.0,
                                        IndexSpinBox->value(),
                                        TicksSpinBox->value(),
                                        LengthSpinBox->value()
                                        );
        stateIndexData->ReconstructFrom(newStateIndexData);
        return true;
    }
    return false;
}
