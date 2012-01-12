/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2011-12-08

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
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

// -- for doubles
vctQtWidgetDynamicVectorDoubleRead::vctQtWidgetDynamicVectorDoubleRead(void):
    QTableWidget()
{
    this->setContentsMargins(0, 0, 0, 0);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
}

bool vctQtWidgetDynamicVectorDoubleRead::SetValue(const vctDynamicVector<double> & vector)
{
    this->setRowCount(1);
    size_t size = vector.size();
    if (this->columnCount() != size) {
        this->setColumnCount(size);
    }
    QTableWidgetItem * tableItem;
    QString itemValue;
    for (unsigned int index = 0; index < size; ++index) {
        tableItem = this->item(0, index);
        if (tableItem == 0) {
            tableItem = new QTableWidgetItem();
            tableItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            this->setItem(0, index, tableItem);
        }
        itemValue.setNum(vector.Element(index));
        tableItem->setText(itemValue);
    }
    return true;
}

vctQtWidgetDynamicVectorDoubleWrite::vctQtWidgetDynamicVectorDoubleWrite(void):
    QTableWidget()
{
    this->setContentsMargins(0, 0, 0, 0);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
}

bool vctQtWidgetDynamicVectorDoubleWrite::SetValue(const vctDynamicVector<double> & vector)
{
    return true;
}

bool vctQtWidgetDynamicVectorDoubleWrite::GetValue(vctDynamicVector<double> & placeHolder) const
{
    return true;
}


// -- for ints
vctQtWidgetDynamicVectorIntRead::vctQtWidgetDynamicVectorIntRead(void):
    QTableWidget()
{
    this->setContentsMargins(0, 0, 0, 0);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
}

bool vctQtWidgetDynamicVectorIntRead::SetValue(const vctDynamicVector<int> & vector)
{
    this->setRowCount(1);
    size_t size = vector.size();
    if (this->columnCount() != size) {
        this->setColumnCount(size);
    }
    QTableWidgetItem * tableItem;
    QString itemValue;
    for (unsigned int index = 0; index < size; ++index) {
        tableItem = this->item(0, index);
        if (tableItem == 0) {
            tableItem = new QTableWidgetItem();
            tableItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            this->setItem(0, index, tableItem);
        }
        itemValue.setNum(vector.Element(index));
        tableItem->setText(itemValue);
    }
    return true;
}

vctQtWidgetDynamicVectorIntWrite::vctQtWidgetDynamicVectorIntWrite(void):
    QTableWidget()
{
    this->setContentsMargins(0, 0, 0, 0);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
}

bool vctQtWidgetDynamicVectorIntWrite::SetValue(const vctDynamicVector<int> & vector)
{
    return true;
}

bool vctQtWidgetDynamicVectorIntWrite::GetValue(vctDynamicVector<int> & placeHolder) const
{
    return true;
}


// -- for bools
vctQtWidgetDynamicVectorBoolRead::vctQtWidgetDynamicVectorBoolRead(void):
    QTableWidget()
{
    this->setContentsMargins(0, 0, 0, 0);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
}

bool vctQtWidgetDynamicVectorBoolRead::SetValue(const vctDynamicVector<bool> & vector)
{
    this->setRowCount(1);
    size_t size = vector.size();
    if (this->columnCount() != size) {
        this->setColumnCount(size);
    }
    QTableWidgetItem * tableItem;
    QString itemValue;
    for (unsigned int index = 0; index < size; ++index) {
        tableItem = this->item(0, index);
        if (tableItem == 0) {
            tableItem = new QTableWidgetItem();
            tableItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            this->setItem(0, index, tableItem);
        }
        itemValue.setNum(vector.Element(index));
        tableItem->setText(itemValue);
    }
    return true;
}

vctQtWidgetDynamicVectorBoolWrite::vctQtWidgetDynamicVectorBoolWrite(void):
    QTableWidget()
{
    this->setContentsMargins(0, 0, 0, 0);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
}

bool vctQtWidgetDynamicVectorBoolWrite::SetValue(const vctDynamicVector<bool> & vector)
{
    return true;
}

bool vctQtWidgetDynamicVectorBoolWrite::GetValue(vctDynamicVector<bool> & placeHolder) const
{
    return true;
}
