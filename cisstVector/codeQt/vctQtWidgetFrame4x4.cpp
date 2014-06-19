/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Praneeth Sadda
  Created on: 2012-02-09

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstVector/vctQtWidgetFrame4x4.h>

#include <QTableWidget>
#include <QHeaderView>
#include <QSpinBox>
#include <QDoubleSpinBox>

// -- for doubles
vctQtWidgetFrame4x4DoubleRead::vctQtWidgetFrame4x4DoubleRead(void):
    QTableWidget()
{
    this->setRowCount(4);
    this->setColumnCount(4);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
    int verticalHeight = this->horizontalHeader()->sizeHint().height() + this->verticalHeader()->sizeHint().height();
    this->setFixedHeight(verticalHeight);
}

bool vctQtWidgetFrame4x4DoubleRead::SetValue(const vctFrame4x4<double> & frame)
{
    QTableWidgetItem * tableItem;
    QString itemValue;
    for (unsigned int row = 0; row < 4; ++row) {
        for(unsigned int column = 0; column < 4; ++column) {
            tableItem = this->item(row, column);
            if (tableItem == 0) {
                tableItem = new QTableWidgetItem();
                tableItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                this->setItem(row, column, tableItem);
            }
            itemValue.setNum(frame.Element(row, column));
            tableItem->setText(itemValue);
        }
    }
    return true;
}

vctQtWidgetFrame4x4DoubleWrite::vctQtWidgetFrame4x4DoubleWrite(void):
    QTableWidget()
{
    this->setRowCount(4);
    this->setColumnCount(4);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
    int verticalHeight = this->horizontalHeader()->sizeHint().height() + this->verticalHeader()->sizeHint().height();
    this->setFixedHeight(verticalHeight);
}

bool vctQtWidgetFrame4x4DoubleWrite::SetValue(const vctFrame4x4<double> & frame)
{
    QDoubleSpinBox * spinBox;
    for (unsigned int row = 0; row < 4; ++row) {
        for(unsigned int column = 0; column < 4; ++column) {
            spinBox = dynamic_cast<QDoubleSpinBox*>(this->cellWidget(row, column));
            if(spinBox == 0) {
                spinBox = new QDoubleSpinBox();
                this->setCellWidget(row, column, spinBox);
                spinBox->setDecimals(5);
            }
            spinBox->setValue(frame.Element(row, column));
        }
    }
    return true;
}

bool vctQtWidgetFrame4x4DoubleWrite::GetValue(vctFrame4x4<double> & placeHolder) const
{
    vctFrame4x4<double> newFrame;
    QDoubleSpinBox * spinBox;
    for(int row = 0; row < 4; ++row) {
        for(int column = 0; column < 4; ++column) {
            spinBox = dynamic_cast<QDoubleSpinBox*>(this->cellWidget(row, column));
            newFrame.Element(row, column) = spinBox->value();
        }
    }
    placeHolder.Assign(newFrame);
    return true;
}
