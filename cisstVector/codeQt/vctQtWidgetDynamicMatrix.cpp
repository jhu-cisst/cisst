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
    this->setContentsMargins(0, 0, 0, 0);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

bool vctQtWidgetDynamicMatrixDoubleRead::SetValue(const Eigen::MatrixXd& matrix)
{
    // Qt uses int, not size_t
    const int rows = static_cast<int>(matrix.rows());
    const int columns = static_cast<int>(matrix.cols());
    this->setRowCount(rows);
    this->setColumnCount(columns);
    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            QTableWidgetItem* tableItem = this->item(row, column);
            if (tableItem == nullptr) {
                tableItem = new QTableWidgetItem();
                tableItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
                tableItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                this->setItem(row, column, tableItem);
            }

            QString itemValue;
            itemValue.setNum(matrix(row, column));
            tableItem->setText(itemValue);
        }
    }
    return true;
}

vctQtWidgetDynamicMatrixDoubleWrite::vctQtWidgetDynamicMatrixDoubleWrite(void):
    QTableWidget()
{
    this->setContentsMargins(0, 0, 0, 0);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

bool vctQtWidgetDynamicMatrixDoubleWrite::SetValue(const Eigen::MatrixXd& matrix)
{
    // Qt uses int, not size_t
    const int rows = static_cast<int>(matrix.rows());
    const int columns = static_cast<int>(matrix.cols());
    this->setRowCount(rows);
    this->setColumnCount(columns);
    for (int row = 0; row < rows; ++row) {
        for ( int column = 0; column < columns; ++column) {
            QDoubleSpinBox* spinBox = dynamic_cast<QDoubleSpinBox*>(this->cellWidget(row, column));
            if (spinBox == nullptr) {
                spinBox = new QDoubleSpinBox();
                this->setCellWidget(row, column, spinBox);
                spinBox->setDecimals(5);
            }
            spinBox->setValue(matrix(row, column));
        }
    }

    return true;
}

bool vctQtWidgetDynamicMatrixDoubleWrite::GetValue(Eigen::MatrixXd& placeHolder) const
{
    // Qt uses int, not size_t
    const int rows = static_cast<int>(this->rowCount());
    const int columns = static_cast<int>(this->columnCount());
    Eigen::MatrixXd newMatrix(rows, columns);
    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            QDoubleSpinBox* spinBox = dynamic_cast<QDoubleSpinBox*>(this->cellWidget(row, column));
            newMatrix(row, column) = spinBox->value();
        }
    }

    placeHolder = newMatrix;
    return true;
}


// -- for ints
vctQtWidgetDynamicMatrixIntRead::vctQtWidgetDynamicMatrixIntRead(void):
    QTableWidget()
{
    this->setContentsMargins(0, 0, 0, 0);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

bool vctQtWidgetDynamicMatrixIntRead::SetValue(const Eigen::MatrixXi& matrix)
{
    // Qt uses int, not size_t
    const int rows = static_cast<int>(matrix.rows());
    const int columns = static_cast<int>(matrix.cols());
    this->setRowCount(rows);
    this->setColumnCount(columns);
    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            QTableWidgetItem* tableItem = this->item(row, column);
            if (tableItem == nullptr) {
                tableItem = new QTableWidgetItem();
                tableItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
                tableItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                this->setItem(row, column, tableItem);
            }
            QString itemValue;
            itemValue.setNum(matrix(row, column));
            tableItem->setText(itemValue);
        }
    }

    return true;
}

vctQtWidgetDynamicMatrixIntWrite::vctQtWidgetDynamicMatrixIntWrite(void):
    QTableWidget()
{
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

bool vctQtWidgetDynamicMatrixIntWrite::SetValue(const Eigen::MatrixXi& matrix)
{
    // Qt uses int, not size_t
    const int rows = static_cast<int>(matrix.rows());
    const int columns = static_cast<int>(matrix.cols());
    this->setRowCount(rows);
    this->setColumnCount(columns);
    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            QSpinBox* spinBox = dynamic_cast<QSpinBox*>(this->cellWidget(row, column));
            if(spinBox == nullptr) {
                spinBox = new QSpinBox();
                this->setCellWidget(row, column, spinBox);
            }
            spinBox->setValue(matrix(row, column));
        }
    }
    return true;
}

bool vctQtWidgetDynamicMatrixIntWrite::GetValue(Eigen::MatrixXi& placeHolder) const
{
    // Qt uses int, not size_t
    const int rows = static_cast<int>(this->rowCount());
    const int columns = static_cast<int>(this->columnCount());

    placeHolder.resize(rows, columns);

    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            QSpinBox* spinBox = dynamic_cast<QSpinBox*>(this->cellWidget(row, column));
            placeHolder(row, column) = spinBox->value();
        }
    }

    return true;
}

// -- for bools
vctQtWidgetDynamicMatrixBoolRead::vctQtWidgetDynamicMatrixBoolRead(void):
    QTableWidget()
{
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

bool vctQtWidgetDynamicMatrixBoolRead::SetValue(const Eigen::MatrixX<bool>& matrix)
{
    QTableWidgetItem * tableItem;
    QString itemValue;
    // Qt uses int, not size_t
    const int rows = static_cast<int>(matrix.rows());
    const int columns = static_cast<int>(matrix.cols());
    this->setRowCount(rows);
    this->setColumnCount(columns);
    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            tableItem = this->item(row, column);
            if (tableItem == nullptr) {
                tableItem = new QTableWidgetItem();
                tableItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
                tableItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                this->setItem(row, column, tableItem);
            }

            itemValue.setNum(matrix(row, column));
            tableItem->setText(itemValue);
        }
    }
    return true;
}

vctQtWidgetDynamicMatrixBoolWrite::vctQtWidgetDynamicMatrixBoolWrite(void):
    QTableWidget()
{
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

bool vctQtWidgetDynamicMatrixBoolWrite::SetValue(const Eigen::MatrixX<bool>& matrix)
{
    const int rows = static_cast<int>(matrix.rows());
    const int columns = static_cast<int>(matrix.cols());
    this->setRowCount(rows);
    this->setColumnCount(columns);
    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            QSpinBox* spinBox = dynamic_cast<QSpinBox*>(this->cellWidget(row, column));
            if(spinBox == nullptr) {
                spinBox = new QSpinBox();
                this->setCellWidget(row, column, spinBox);
                spinBox->setMinimum(0);
                spinBox->setMaximum(1);
            }
            spinBox->setValue(matrix(row, column));
        }
    }
    return true;
}

bool vctQtWidgetDynamicMatrixBoolWrite::GetValue(Eigen::MatrixX<bool>& placeHolder) const
{
    // Qt uses int, not size_t
    const int rows = static_cast<int>(this->rowCount());
    const int columns = static_cast<int>(this->columnCount());
    placeHolder.resize(rows, columns);
    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            QSpinBox* spinBox = dynamic_cast<QSpinBox*>(this->cellWidget(row, column));
            placeHolder(row, column) = (bool)spinBox->value();
        }
    }

    return true;
}
