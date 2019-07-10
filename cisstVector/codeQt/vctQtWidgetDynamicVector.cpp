/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2011-12-08

  (C) Copyright 2011-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstVector/vctQtWidgetDynamicVector.h>

#include <QHeaderView>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>


vctQtWidgetDynamicVectorReadBase::vctQtWidgetDynamicVectorReadBase(void):
    QTableWidget()
{
    this->setRowCount(1);
    this->setContentsMargins(0, 0, 0, 0);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
#if CISST_HAS_QT4
    this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setResizeMode(QHeaderView::Stretch);
#else
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#endif
    this->setFixedHeight(this->verticalHeader()->sizeHint().height());
}

template <class _elementType>
vctQtWidgetDynamicVectorReadFloating<_elementType>::vctQtWidgetDynamicVectorReadFloating(void):
    vctQtWidgetDynamicVectorReadBase(),
    Format('f')
{
    SetPrecision(2);
}

template <class _elementType>
void vctQtWidgetDynamicVectorReadFloating<_elementType>::SetPrecision(const int precision)
{
    Precision = precision;
}

template <class _elementType>
void vctQtWidgetDynamicVectorReadFloating<_elementType>::SetFormat(const char format)
{
    Format = format;
}

template <class _elementType>
bool vctQtWidgetDynamicVectorReadFloating<_elementType>::SetValue(const vctDynamicConstVectorRef<_elementType> & vector)
{
    // Qt uses int, not size_t
    const int size = static_cast<int>(vector.size());
    if (this->columnCount() != size) {
        this->setColumnCount(size);
    }
    QTableWidgetItem * tableItem;
    for (int index = 0; index < size; ++index) {
        tableItem = this->item(0, index);
        if (tableItem == 0) {
            tableItem = new QTableWidgetItem();
            tableItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
            tableItem->setFlags(tableItem->flags() ^ Qt::ItemIsEditable);
            this->setItem(0, index, tableItem);
        }
        tableItem->setText(QString::number(vector.at(index), Format, Precision));
    }
    return true;
}

template <class _elementType>
vctQtWidgetDynamicVectorReadInteger<_elementType>::vctQtWidgetDynamicVectorReadInteger(void):
    vctQtWidgetDynamicVectorReadBase(),
    Base(10)
{
}

template <class _elementType>
void vctQtWidgetDynamicVectorReadInteger<_elementType>::SetBase(const int base)
{
    Base = base;
}


template <class _elementType>
bool vctQtWidgetDynamicVectorReadInteger<_elementType>::SetValue(const vctDynamicConstVectorRef<_elementType> & vector)
{
    // Qt uses int, not size_t
    const int size = static_cast<int>(vector.size());
    if (this->columnCount() != size) {
        this->setColumnCount(size);
    }
    QTableWidgetItem * tableItem;
    for (int index = 0; index < size; ++index) {
        tableItem = this->item(0, index);
        if (tableItem == 0) {
            tableItem = new QTableWidgetItem();
            tableItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
            tableItem->setFlags(tableItem->flags() ^ Qt::ItemIsEditable);
            this->setItem(0, index, tableItem);
        }
        tableItem->setText(QString::number(vector.at(index), Base));
    }
    return true;
}


template class vctQtWidgetDynamicVectorReadFloating<double>;
template class vctQtWidgetDynamicVectorReadFloating<float>;
template class vctQtWidgetDynamicVectorReadInteger<int>;
template class vctQtWidgetDynamicVectorReadInteger<unsigned int>;
template class vctQtWidgetDynamicVectorReadInteger<bool>;


vctQtWidgetDynamicVectorWriteBase::vctQtWidgetDynamicVectorWriteBase(const DisplayModeType displayMode):
    QTableWidget(),
    DisplayMode(displayMode)
{
    this->setRowCount(1);
    this->setContentsMargins(0, 0, 0, 0);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
#if CISST_HAS_QT4
    this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setResizeMode(QHeaderView::Stretch);
#else
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#endif
    this->setFixedHeight(this->verticalHeader()->sizeHint().height());
    connect(this, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(ItemChangedSlot(QTableWidgetItem *)));
}

void vctQtWidgetDynamicVectorWriteBase::SliderValueChangedSlot(int CMN_UNUSED(value))
{
    emit this->valueChanged();
}

void vctQtWidgetDynamicVectorWriteBase::DoubleSpinBoxEditingFinishedSlot(void)
{
    emit this->valueChanged();
}

void vctQtWidgetDynamicVectorWriteBase::SpinBoxEditingFinishedSlot(void)
{
    emit this->valueChanged();
}

void vctQtWidgetDynamicVectorWriteBase::ItemChangedSlot(QTableWidgetItem * CMN_UNUSED(item))
{
    emit this->valueChanged();
}

template <class _elementType>
vctQtWidgetDynamicVectorWriteFloating<_elementType>::vctQtWidgetDynamicVectorWriteFloating(const DisplayModeType displayMode):
    vctQtWidgetDynamicVectorWriteBase(displayMode),
    Format('f')
{
    SetPrecision(2);
    SetRange(static_cast<_elementType>(-100.0),
             static_cast<_elementType>(100.0));
    SetStep(static_cast<_elementType>(0.01));
}

template <class _elementType>
void vctQtWidgetDynamicVectorWriteFloating<_elementType>::SetPrecision(const int precision)
{
    Precision = precision;
    UpdateWidgetPrecision();
}

template <class _elementType>
void vctQtWidgetDynamicVectorWriteFloating<_elementType>::UpdateWidgetPrecision(void)
{
    // update widgets, if any
    const int size = this->columnCount();
    if (size == 0) {
        return;
    }
    switch (DisplayMode) {
    case TEXT_WIDGET:
        break;
    case SPINBOX_WIDGET:
        QDoubleSpinBox * spinBox;
        for (int index = 0; index < size; ++index) {
            spinBox = dynamic_cast<QDoubleSpinBox*>(this->cellWidget(0, index));
            spinBox->setDecimals(this->Precision);
        }
        break;
    case SLIDER_WIDGET:
        break;
    default:
        break;
    }
}

template <class _elementType>
void vctQtWidgetDynamicVectorWriteFloating<_elementType>::SetFormat(const char format)
{
    Format = format;
}

template <class _elementType>
typename vctQtWidgetDynamicVectorWriteFloating<_elementType>::value_type
vctQtWidgetDynamicVectorWriteFloating<_elementType>::GetMinimum(const size_t index) const {
    if (Minimums.ValidIndex(index)) {
        return Minimums.Element(index);
    }
    return Minimum;
}

template <class _elementType>
typename vctQtWidgetDynamicVectorWriteFloating<_elementType>::value_type
vctQtWidgetDynamicVectorWriteFloating<_elementType>::GetMaximum(const size_t index) const {
    if (Maximums.ValidIndex(index)) {
        return Maximums.Element(index);
    }
    return Maximum;
}

template <class _elementType>
void vctQtWidgetDynamicVectorWriteFloating<_elementType>::SetRange(const value_type minimum,
                                                                   const value_type maximum)
{
    // set internal data members
    Minimum = minimum;
    Maximum = maximum;
    Minimums.SetAll(minimum);
    Maximums.SetAll(maximum);
    // update widgets
    UpdateWidgetRange();
}

template <class _elementType>
void vctQtWidgetDynamicVectorWriteFloating<_elementType>::SetRange(const vctDynamicVector<value_type> & minimums,
                                                                   const vctDynamicVector<value_type> & maximums)
{
    // set internal data members
    Minimums.ForceAssign(minimums);
    Maximums.ForceAssign(maximums);
    // update widgets
    UpdateWidgetRange();
}

template <class _elementType>
void vctQtWidgetDynamicVectorWriteFloating<_elementType>::UpdateWidgetRange(void)
{
    // update widgets, if any
    const int size = this->columnCount();
    if (size == 0) {
        return;
    }
    switch (DisplayMode) {
    case TEXT_WIDGET:
        break;
    case SPINBOX_WIDGET:
        QDoubleSpinBox * spinBox;
        for (int index = 0; index < size; ++index) {
            spinBox = dynamic_cast<QDoubleSpinBox*>(this->cellWidget(0, index));
            spinBox->setRange(GetMinimum(index), GetMaximum(index));
        }
        break;
    case SLIDER_WIDGET:
        break;
    default:
        break;
    }
}

template <class _elementType>
void vctQtWidgetDynamicVectorWriteFloating<_elementType>::SetStep(const value_type step)
{
    Step = step;
}

template <class _elementType>
bool vctQtWidgetDynamicVectorWriteFloating<_elementType>::SetValue(const vctDynamicVector<value_type> & vector, bool blockSignals)
{
    const bool previousBlockSignals = this->blockSignals(blockSignals);

    // Qt uses int, not size_t
    const int size = static_cast<int>(vector.size());
    switch (DisplayMode) {
    case TEXT_WIDGET:
        if (this->columnCount() != size) {
            this->setColumnCount(size);
        }
        QTableWidgetItem * tableItem;
        for (int index = 0; index < size; ++index) {
            tableItem = this->item(0, index);
            if (tableItem == 0) {
                tableItem = new QTableWidgetItem();
                tableItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
                this->setItem(0, index, tableItem);
            }
            tableItem->setText(QString::number(vector.at(index), Format, Precision));
        }
        break;
    case SPINBOX_WIDGET:
        if (this->columnCount() != size) {
            this->setColumnCount(size);
        }
        QDoubleSpinBox * spinBox;
        for (int index = 0; index < size; ++index) {
            spinBox = dynamic_cast<QDoubleSpinBox*>(this->cellWidget(0, index));
            if (spinBox == 0) {
                spinBox = new QDoubleSpinBox();
                spinBox->setRange(Minimum, Maximum);
                spinBox->setDecimals(this->Precision);
                spinBox->setSingleStep(Step);
                connect(spinBox, SIGNAL(editingFinished()), this, SLOT(DoubleSpinBoxEditingFinishedSlot()));
                this->setCellWidget(0, index, spinBox);
            }
            spinBox->setValue(vector.at(index));
        }
        break;
    case SLIDER_WIDGET:
        if (this->columnCount() != size) {
            this->setColumnCount(size);
        }
        QSlider * slider;
        for (int index = 0; index < size; ++index) {
            slider = dynamic_cast<QSlider*>(this->cellWidget(0, index));
            if (slider == 0) {
                slider = new QSlider(Qt::Horizontal);
                slider->setRange(0, SLIDER_RESOLUTION);
                slider->setSingleStep(1);
                connect(slider, SIGNAL(valueChanged(int)), this, SLOT(SliderValueChangedSlot(int)));
                this->setCellWidget(0, index, slider);
            }
            slider->setValue((vector.at(index) - GetMinimum(index)) / (GetMaximum(index) - GetMinimum(index)) * SLIDER_RESOLUTION);
        }
        break;
    default:
        break;
    }
    this->blockSignals(previousBlockSignals);
    return true;
}

template <class _elementType>
bool vctQtWidgetDynamicVectorWriteFloating<_elementType>::GetValue(vctDynamicVector<value_type> & placeHolder) const
{
    size_t index;
    const int columns = this->columnCount();
    if (columns != static_cast<int>(placeHolder.size())) {
        return false;
    }
    switch (DisplayMode) {
    case TEXT_WIDGET:
        QTableWidgetItem * item;
        for (int column = 0; column < columns; ++column) {
            item = dynamic_cast<QTableWidgetItem*>(this->item(0, column));
            placeHolder.at(column) = item->text().toDouble();
        }
        break;
        break;
    case SPINBOX_WIDGET:
        QDoubleSpinBox * spinBox;
        for (int column = 0; column < columns; ++column) {
            spinBox = dynamic_cast<QDoubleSpinBox*>(this->cellWidget(0, column));
            placeHolder.at(column) = spinBox->value();
        }
        break;
    case SLIDER_WIDGET:
        QSlider * slider;
        for (int column = 0; column < columns; ++column) {
            index = static_cast<size_t>(column);
            slider = dynamic_cast<QSlider*>(this->cellWidget(0, column));
            placeHolder.at(column) = static_cast<double>(slider->value()) / SLIDER_RESOLUTION * (GetMaximum(index) - GetMinimum(index)) + GetMinimum(index);
        }
        break;
    default:
        break;
    }
    return true;
}

template class vctQtWidgetDynamicVectorWriteFloating<double>;
template class vctQtWidgetDynamicVectorWriteFloating<float>;



template <class _elementType>
vctQtWidgetDynamicVectorWriteInteger<_elementType>::vctQtWidgetDynamicVectorWriteInteger(const DisplayModeType displayMode):
    vctQtWidgetDynamicVectorWriteBase(displayMode),
    Base(10),
    Minimum(-100),
    Maximum(100),
    Step(1)
{
}

template <class _elementType>
void vctQtWidgetDynamicVectorWriteInteger<_elementType>::SetBase(const int base)
{
    Base = base;
}

template <class _elementType>
void vctQtWidgetDynamicVectorWriteInteger<_elementType>::SetRange(const value_type minimum,
                                                                  const value_type maximum)
{
    Minimum = minimum;
    Maximum = maximum;
}

template <class _elementType>
void vctQtWidgetDynamicVectorWriteInteger<_elementType>::SetStep(const value_type step)
{
    Step = step;
}

template <class _elementType>
bool vctQtWidgetDynamicVectorWriteInteger<_elementType>::SetValue(const vctDynamicVector<value_type> & vector, bool blockSignals)
{
    const bool previousBlockSignals = this->blockSignals(blockSignals);

    // Qt uses int, not size_t
    const int size = static_cast<int>(vector.size());
    switch (DisplayMode) {
    case TEXT_WIDGET:
        if (this->columnCount() != size) {
            this->setColumnCount(size);
        }
        QTableWidgetItem * tableItem;
        for (int index = 0; index < size; ++index) {
            tableItem = this->item(0, index);
            if (tableItem == 0) {
                tableItem = new QTableWidgetItem();
                tableItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
                this->setItem(0, index, tableItem);
            }
            tableItem->setText(QString::number(vector.at(index), Base));
        }
        break;
    case SPINBOX_WIDGET:
        if (this->columnCount() != size) {
            this->setColumnCount(size);
        }
        QSpinBox * spinBox;
        for (int index = 0; index < size; ++index) {
            spinBox = dynamic_cast<QSpinBox*>(this->cellWidget(0, index));
            if (spinBox == 0) {
                spinBox = new QSpinBox();
                spinBox->setRange(Minimum, Maximum);
                spinBox->setSingleStep(Step);
                connect(spinBox, SIGNAL(editingFinished()), this, SLOT(SpinBoxEditingFinishedSlot()));
                this->setCellWidget(0, index, spinBox);
            }
            spinBox->setValue(vector.at(index));
        }
        break;
    case SLIDER_WIDGET:
        if (this->columnCount() != size) {
            this->setColumnCount(size);
        }
        QSlider * slider;
        for (int index = 0; index < size; ++index) {
            slider = dynamic_cast<QSlider*>(this->cellWidget(0, index));
            if (slider == 0) {
                slider = new QSlider(Qt::Horizontal);
                slider->setRange(Minimum, Maximum);
                slider->setSingleStep(Step);
                connect(slider, SIGNAL(valueChanged(int)), this, SLOT(SliderValueChangedSlot(int)));
                this->setCellWidget(0, index, slider);
            }
            slider->setValue(vector.at(index));
        }
        break;
    default:
        break;
    }
    this->blockSignals(previousBlockSignals);
    return true;
}

template <class _elementType>
bool vctQtWidgetDynamicVectorWriteInteger<_elementType>::GetValue(vctDynamicVector<value_type> & placeHolder) const
{
    const int columns = this->columnCount();
    if (columns != static_cast<int>(placeHolder.size())) {
        return false;
    }
    switch (DisplayMode) {
    case TEXT_WIDGET:
        QTableWidgetItem * item;
        for (int column = 0; column < columns; ++column) {
            item = dynamic_cast<QTableWidgetItem*>(this->item(0, column));
            placeHolder.at(column) = item->text().toDouble();
        }
        break;
        break;
    case SPINBOX_WIDGET:
        QSpinBox * spinBox;
        for (int column = 0; column < columns; ++column) {
            spinBox = dynamic_cast<QSpinBox*>(this->cellWidget(0, column));
            placeHolder.at(column) = spinBox->value();
        }
        break;
    case SLIDER_WIDGET:
        QSlider * slider;
        for (int column = 0; column < columns; ++column) {
            slider = dynamic_cast<QSlider*>(this->cellWidget(0, column));
            placeHolder.at(column) = slider->value();
        }
        break;
    default:
        break;
    }
    return true;
}

template class vctQtWidgetDynamicVectorWriteInteger<int>;
template class vctQtWidgetDynamicVectorWriteInteger<unsigned int>;


vctQtWidgetDynamicVectorBoolWrite::vctQtWidgetDynamicVectorBoolWrite(void):
    QTableWidget()
{
    this->setRowCount(1);
    this->setContentsMargins(0, 0, 0, 0);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
#if CISST_HAS_QT4
    this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setResizeMode(QHeaderView::Stretch);
#else
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#endif
    this->setFixedHeight(this->verticalHeader()->sizeHint().height());
}

void vctQtWidgetDynamicVectorBoolWrite::ValueChangedSlot(bool CMN_UNUSED(value))
{
    emit this->valueChanged();
}

bool vctQtWidgetDynamicVectorBoolWrite::SetValue(const vctDynamicVector<bool> & vector, bool blockSignals)
{
    const bool previousBlockSignals = this->blockSignals(blockSignals);

    // Qt uses int, not size_t
    const int size = static_cast<int>(vector.size());
    if (this->columnCount() != size) {
        this->setColumnCount(size);
    }
    QCheckBox * checkBox;
    for (int index = 0; index < size; ++index) {
        checkBox = dynamic_cast<QCheckBox*>(this->cellWidget(0, index));
        if (checkBox == 0) {
            checkBox = new QCheckBox();
            this->setCellWidget(0, index, checkBox);
            connect(checkBox, SIGNAL(toggled(bool)), this, SLOT(ValueChangedSlot(bool)));
        }
        checkBox->setChecked(vector.at(index));
    }
    this->blockSignals(previousBlockSignals);
    return true;
}

bool vctQtWidgetDynamicVectorBoolWrite::GetValue(vctDynamicVector<bool> & placeHolder) const
{
    int columns = this->columnCount();
    QCheckBox * checkBox;
    for (int column = 0; column < columns; ++column) {
        checkBox = dynamic_cast<QCheckBox*>(this->cellWidget(0, column));
        placeHolder.at(column) = checkBox->isChecked();
    }
    return true;
}
