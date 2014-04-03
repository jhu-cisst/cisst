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

#include <cisstVector/vctQtWidgetDynamicMatrix.h>

#include <QTableWidget>
#include <QHeaderView>
#include <QSpinBox>
#include <QDoubleSpinBox>

// -- for doubles
vctQtWidgetDynamicMatrixDoubleRead::vctQtWidgetDynamicMatrixDoubleRead(void):
    QTableWidget()
{
    this->setRowCount(4);
    this->setColumnCount(4);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
    this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    int verticalHeight = this->horizontalHeader()->sizeHint().height() + this->verticalHeader()->sizeHint().height();
    this->setFixedHeight(verticalHeight);
}

bool vctQtWidgetDynamicMatrixDoubleRead::SetValue(const vctDynamicMatrix<double> & matrix)
{
    QTableWidgetItem * tableItem;
    QString itemValue;
    const size_t rows = matrix.rows();
    const size_t columns = matrix.cols();
    for (size_t row = 0; row < rows; ++row) {
        for (size_t column = 0; column < columns; ++column) {
            tableItem = this->item(row, column);
            if (tableItem == 0) {
                tableItem = new QTableWidgetItem();
                tableItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                this->setItem(row, column, tableItem);
            }
            itemValue.setNum(matrix.Element(row, column));
            tableItem->setText(itemValue);
        }
    }
    return true;
}

vctQtWidgetDynamicMatrixDoubleWrite::vctQtWidgetDynamicMatrixDoubleWrite(void):
    QTableWidget()
{
    this->setRowCount(4);
    this->setColumnCount(4);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
    this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    int verticalHeight = this->horizontalHeader()->sizeHint().height() + this->verticalHeader()->sizeHint().height();
    this->setFixedHeight(verticalHeight);
}

bool vctQtWidgetDynamicMatrixDoubleWrite::SetValue(const vctDynamicMatrix<double> & matrix)
{
    QDoubleSpinBox * spinBox;
    const size_t rows = matrix.rows();
    const size_t columns = matrix.cols();
    for (size_t row = 0; row < rows; ++row) {
        for (size_t column = 0; column < columns; ++column) {
            spinBox = dynamic_cast<QDoubleSpinBox*>(this->cellWidget(row, column));
            if(spinBox == 0) {
                spinBox = new QDoubleSpinBox();
                this->setCellWidget(row, column, spinBox);
                spinBox->setDecimals(5);
            }
            spinBox->setValue(matrix.Element(row, column));
        }
    }
    return true;
}

bool vctQtWidgetDynamicMatrixDoubleWrite::GetValue(vctDynamicMatrix<double> & placeHolder) const
{
    const size_t rows = this->rowCount();
    const size_t columns = this->columnCount();
    vctDynamicMatrix<double> newMatrix(rows, columns);
    QDoubleSpinBox * spinBox;
    for (size_t row = 0; row < rows; ++row) {
        for (size_t column = 0; column < columns; ++column) {
            spinBox = dynamic_cast<QDoubleSpinBox*>(this->cellWidget(row, column));
            newMatrix.Element(row, column) = spinBox->value();
        }
    }
    placeHolder.Assign(newMatrix);
    return true;
}


// -- for ints
vctQtWidgetDynamicMatrixIntRead::vctQtWidgetDynamicMatrixIntRead(void):
    QTableWidget()
{
    this->setRowCount(4);
    this->setColumnCount(4);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
    this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    int verticalHeight = this->horizontalHeader()->sizeHint().height() + this->verticalHeader()->sizeHint().height();
    this->setFixedHeight(verticalHeight);
}

bool vctQtWidgetDynamicMatrixIntRead::SetValue(const vctDynamicMatrix<int> & matrix)
{
    QTableWidgetItem * tableItem;
    QString itemValue;
    const size_t rows = matrix.rows();
    const size_t columns = matrix.cols();
    for (size_t row = 0; row < rows; ++row) {
        for (size_t column = 0; column < columns; ++column) {
            tableItem = this->item(row, column);
            if (tableItem == 0) {
                tableItem = new QTableWidgetItem();
                tableItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                this->setItem(row, column, tableItem);
            }
            itemValue.setNum(matrix.Element(row, column));
            tableItem->setText(itemValue);
        }
    }
    return true;
}

vctQtWidgetDynamicMatrixIntWrite::vctQtWidgetDynamicMatrixIntWrite(void):
    QTableWidget()
{
    this->setRowCount(4);
    this->setColumnCount(4);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
    this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    int verticalHeight = this->horizontalHeader()->sizeHint().height() + this->verticalHeader()->sizeHint().height();
    this->setFixedHeight(verticalHeight);
}

bool vctQtWidgetDynamicMatrixIntWrite::SetValue(const vctDynamicMatrix<int> & matrix)
{
    QSpinBox * spinBox;
    const size_t rows = matrix.rows();
    const size_t columns = matrix.cols();
    for (size_t row = 0; row < rows; ++row) {
        for (size_t column = 0; column < columns; ++column) {
            spinBox = dynamic_cast<QSpinBox*>(this->cellWidget(row, column));
            if(spinBox == 0) {
                spinBox = new QSpinBox();
                this->setCellWidget(row, column, spinBox);
            }
            spinBox->setValue(matrix.Element(row, column));
        }
    }
    return true;
}

bool vctQtWidgetDynamicMatrixIntWrite::GetValue(vctDynamicMatrix<int> & placeHolder) const
{
    const size_t rows = this->rowCount();
    const size_t columns = this->columnCount();
    vctDynamicMatrix<int> newMatrix(rows, columns);
    QSpinBox * spinBox;
    for (size_t row = 0; row < rows; ++row) {
        for (size_t column = 0; column < columns; ++column) {
            spinBox = dynamic_cast<QSpinBox*>(this->cellWidget(row, column));
            newMatrix.Element(row, column) = spinBox->value();
        }
    }
    placeHolder.Assign(newMatrix);
    return true;
}


// -- for bools
vctQtWidgetDynamicMatrixBoolRead::vctQtWidgetDynamicMatrixBoolRead(void):
    QTableWidget()
{
    this->setRowCount(4);
    this->setColumnCount(4);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
    this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    int verticalHeight = this->horizontalHeader()->sizeHint().height() + this->verticalHeader()->sizeHint().height();
    this->setFixedHeight(verticalHeight);
}

bool vctQtWidgetDynamicMatrixBoolRead::SetValue(const vctDynamicMatrix<bool> & matrix)
{
    QTableWidgetItem * tableItem;
    QString itemValue;
    const size_t rows = matrix.rows();
    const size_t columns = matrix.cols();
    for (size_t row = 0; row < rows; ++row) {
        for (size_t column = 0; column < columns; ++column) {
            tableItem = this->item(row, column);
            if (tableItem == 0) {
                tableItem = new QTableWidgetItem();
                tableItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                this->setItem(row, column, tableItem);
            }
            itemValue.setNum(matrix.Element(row, column));
            tableItem->setText(itemValue);
        }
    }
    return true;
}

vctQtWidgetDynamicMatrixBoolWrite::vctQtWidgetDynamicMatrixBoolWrite(void):
    QTableWidget()
{
    this->setRowCount(4);
    this->setColumnCount(4);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
    this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    int verticalHeight = this->horizontalHeader()->sizeHint().height() + this->verticalHeader()->sizeHint().height();
    this->setFixedHeight(verticalHeight);
}

bool vctQtWidgetDynamicMatrixBoolWrite::SetValue(const vctDynamicMatrix<bool> & matrix)
{
    QSpinBox * spinBox;
    const size_t rows = matrix.rows();
    const size_t columns = matrix.cols();
    for (size_t row = 0; row < rows; ++row) {
        for (size_t column = 0; column < columns; ++column) {
            spinBox = dynamic_cast<QSpinBox*>(this->cellWidget(row, column));
            if(spinBox == 0) {
                spinBox = new QSpinBox();
                this->setCellWidget(row, column, spinBox);
                spinBox->setMinimum(0);
                spinBox->setMaximum(1);
            }
            spinBox->setValue(matrix.Element(row, column));
        }
    }
    return true;
}

bool vctQtWidgetDynamicMatrixBoolWrite::GetValue(vctDynamicMatrix<bool> & placeHolder) const
{
    const size_t rows = this->rowCount();
    const size_t columns = this->columnCount();
    vctDynamicMatrix<bool> newMatrix(rows, columns);
    QSpinBox * spinBox;
    for (size_t row = 0; row < rows; ++row) {
        for (size_t column = 0; column < columns; ++column) {
            spinBox = dynamic_cast<QSpinBox*>(this->cellWidget(row, column));
            newMatrix.Element(row, column) = (bool)spinBox->value();
        }
    }
    placeHolder.Assign(newMatrix);
    return true;
}
