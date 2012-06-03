/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2011-12-08

  (C) Copyright 2011-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstVector/vctQtWidgetDynamicVector.h>

#include <QTableWidget>
#include <QHeaderView>
#include <QSpinBox>
#include <QDoubleSpinBox>



template <class _elementType>
vctQtWidgetDynamicVectorRead<_elementType>::vctQtWidgetDynamicVectorRead(void):
    Table(0)
{
    this->Table = new QTableWidget();
    this->Table->setRowCount(1);
    this->Table->verticalHeader()->hide();
    this->Table->horizontalHeader()->hide();
    this->Table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    this->Table->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    int verticalHeight =
        this->Table->horizontalHeader()->sizeHint().height()
        + this->Table->verticalHeader()->sizeHint().height();
    this->Table->setFixedHeight(verticalHeight);
}

template <class _elementType>
QWidget * vctQtWidgetDynamicVectorRead<_elementType>::GetWidget(void)
{
    return this->Table;
}

template <class _elementType>
bool vctQtWidgetDynamicVectorRead<_elementType>::SetValue(const vctDynamicVector<_elementType> & vector)
{
    const size_t size = vector.size();
    if (static_cast<size_t>(this->Table->columnCount()) != size) {
        this->Table->setColumnCount(size);
    }
    QTableWidgetItem * tableItem;
    QString itemValue;
    for (size_t index = 0; index < size; ++index) {
        tableItem = this->Table->item(0, index);
        if (tableItem == 0) {
            tableItem = new QTableWidgetItem();
            tableItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            this->Table->setItem(0, index, tableItem);
        }
        itemValue.setNum(vector.Element(index));
        tableItem->setText(itemValue);
    }
    return true;
}


template class vctQtWidgetDynamicVectorRead<double>;
template class vctQtWidgetDynamicVectorRead<float>;
template class vctQtWidgetDynamicVectorRead<int>;
template class vctQtWidgetDynamicVectorRead<unsigned int>;
template class vctQtWidgetDynamicVectorRead<bool>;



vctQtWidgetDynamicVectorDoubleWrite::vctQtWidgetDynamicVectorDoubleWrite(void):
    QTableWidget()
{
    this->setRowCount(1);
    this->setContentsMargins(0, 0, 0, 0);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
    this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    int verticalHeight = this->horizontalHeader()->sizeHint().height() + this->verticalHeader()->sizeHint().height();
    this->setFixedHeight(verticalHeight);
}

bool vctQtWidgetDynamicVectorDoubleWrite::SetValue(const vctDynamicVector<double> & vector)
{
    const size_t size = vector.size();
    if (static_cast<size_t>(this->columnCount()) != size) {
        this->setColumnCount(size);
    }
    QDoubleSpinBox * spinBox;
    for (size_t index = 0; index < size; ++index) {
        spinBox = dynamic_cast<QDoubleSpinBox*>(this->cellWidget(0, index));
        if(spinBox == 0) {
            spinBox = new QDoubleSpinBox();
            this->setCellWidget(0, index, spinBox);
            spinBox->setDecimals(5);
        }
        spinBox->setValue(vector.Element(index));
    }
    return true;
}

bool vctQtWidgetDynamicVectorDoubleWrite::GetValue(vctDynamicVector<double> & placeHolder) const
{
    int columns = this->columnCount();
    vctDynamicVector<double> newVector(columns);
    QDoubleSpinBox * spinBox;
    for(int column = 0; column < columns; ++column) {
        spinBox = dynamic_cast<QDoubleSpinBox*>(this->cellWidget(1, column));
        newVector.Element(column) = spinBox->value();
    }
    placeHolder.Assign(newVector);
    return true;
}


// -- for ints

vctQtWidgetDynamicVectorIntWrite::vctQtWidgetDynamicVectorIntWrite(void):
    QTableWidget()
{
    this->setRowCount(1);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
    this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    int verticalHeight = this->horizontalHeader()->sizeHint().height() + this->verticalHeader()->sizeHint().height();
    this->setFixedHeight(verticalHeight);
}


bool vctQtWidgetDynamicVectorIntWrite::SetValue(const vctDynamicVector<int> & vector)
{
    const size_t size = vector.size();
    if (static_cast<size_t>(this->columnCount()) != size) {
        this->setColumnCount(size);
    }
    QSpinBox * spinBox;
    for (size_t index = 0; index < size; ++index) {
        spinBox = dynamic_cast<QSpinBox*>(this->cellWidget(0, index));
        if(spinBox == 0) {
            spinBox = new QSpinBox();
            this->setCellWidget(0, index, spinBox);
        }
        spinBox->setValue(vector.Element(index));
    }
    return true;
}

bool vctQtWidgetDynamicVectorIntWrite::GetValue(vctDynamicVector<int> & placeHolder) const
{
    int columns = this->columnCount();
    vctDynamicVector<double> newVector(columns);
    QSpinBox * spinBox;
    for(int column = 0; column < columns; ++column) {
        spinBox = dynamic_cast<QSpinBox*>(this->cellWidget(1, column));
        newVector.Element(column) = spinBox->value();
    }
    placeHolder.Assign(newVector);
    return true;
}


vctQtWidgetDynamicVectorBoolWrite::vctQtWidgetDynamicVectorBoolWrite(void):
    QTableWidget()
{
    this->setRowCount(1);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
    this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    int verticalHeight = this->horizontalHeader()->sizeHint().height() + this->verticalHeader()->sizeHint().height();
    this->setFixedHeight(verticalHeight);
}

bool vctQtWidgetDynamicVectorBoolWrite::SetValue(const vctDynamicVector<bool> & vector)
{
    const size_t size = vector.size();
    if (static_cast<size_t>(this->columnCount()) != size) {
        this->setColumnCount(size);
    }
    QSpinBox * spinBox;
    for (size_t index = 0; index < size; ++index) {
        spinBox = dynamic_cast<QSpinBox*>(this->cellWidget(0, index));
        if(spinBox == 0) {
            spinBox = new QSpinBox();
            this->setCellWidget(0, index, spinBox);
            spinBox->setMinimum(0);
            spinBox->setMaximum(1);
        }
        spinBox->setValue(vector.Element(index));
    }
    return true;
}

bool vctQtWidgetDynamicVectorBoolWrite::GetValue(vctDynamicVector<bool> & placeHolder) const
{
    int columns = this->columnCount();
    vctDynamicVector<double> newVector(columns);
    QSpinBox * spinBox;
    for(int column = 0; column < columns; ++column) {
        spinBox = dynamic_cast<QSpinBox*>(this->cellWidget(1, column));
        newVector.Element(column) = (bool)spinBox->value();
    }
    placeHolder.Assign(newVector);
    return true;
}
