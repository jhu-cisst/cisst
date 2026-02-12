/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2010-03-20

  (C) Copyright 2010-2021 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnLoggerQtWidget.h>
#include <cisstCommon/cmnClassRegister.h>

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QHeaderView>
#include <QAbstractTableModel>
#include <QItemDelegate>
#include <QTableView>

/*! Model class used for class register entries */
class cmnLoggerQtWidgetClassServicesModel: public QAbstractTableModel
{
public:
    cmnLoggerQtWidgetClassServicesModel(QObject * parent = 0);
    int rowCount(const QModelIndex & parent) const;
    int columnCount(const QModelIndex & parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex & index, int role) const;
    Qt::ItemFlags flags(const QModelIndex & index) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role);
};


/*! Delegate class to handle LoD changes in model */
class cmnLoggerQtWidgetLoDDelegate: public QItemDelegate
{
public:
    cmnLoggerQtWidgetLoDDelegate(QObject * parent = 0);
    QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
	void setEditorData(QWidget * editor, const QModelIndex & index) const;
    void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
    void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const;
};


cmnLoggerQtWidgetClassServicesModel::cmnLoggerQtWidgetClassServicesModel(QObject * parent):
    QAbstractTableModel(parent)
{
}


int cmnLoggerQtWidgetClassServicesModel::rowCount(const QModelIndex & CMN_UNUSED(parent)) const
{
    return cmnClassRegister::size();
}


int cmnLoggerQtWidgetClassServicesModel::columnCount(const QModelIndex & CMN_UNUSED(parent)) const
{
    return 2;
}


QVariant cmnLoggerQtWidgetClassServicesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            if (section == 0) {
                return QString("Class");
            } else if (section == 1) {
                return QString("Filter level");
            }
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}


QVariant cmnLoggerQtWidgetClassServicesModel::data(const QModelIndex & index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        cmnClassRegister::const_iterator iterator = cmnClassRegister::begin();
        std::advance(iterator, index.row());
        if (index.column() == 0) {
            return QString(iterator->first.c_str());
        } else {
            // get the string associated to the level of detail for display
            if (role == Qt::DisplayRole) {
                return QString(cmnLogLevelToString(iterator->second->GetLoD()).c_str());
            } else {
                // for edit, return the LoD itself
                return QVariant(iterator->second->GetLoD());
            }
        }
    }

    if (role == Qt::TextAlignmentRole) {
        return QVariant(Qt::AlignLeft | Qt::AlignTop);
    }

    if (role == Qt::ToolTipRole) {
        if (index.column() == 0) {
            return QString("Class name, can't be modified");
        } else {
            // get the string associated to the level of detail
            return QString("Level of Detail, modify using dropbox");
        }
    }

    return QVariant();
}


Qt::ItemFlags cmnLoggerQtWidgetClassServicesModel::flags(const QModelIndex & index) const
{
    if (!index.isValid()) {
        return QFlags<Qt::ItemFlag>();
    }
    // only the second column can be edited
    if (index.column() == 0) {
        return QAbstractItemModel::flags(index);
    } else {
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }
}


bool cmnLoggerQtWidgetClassServicesModel::setData(const QModelIndex & index,
                                                  const QVariant & value, int role)
{
    if (index.isValid() && (role == Qt::EditRole ||
                            role == Qt::DisplayRole)) {
        // add 1 to the row index to skip over the header
        // addressBook[index.row()+1][index.column()] = value.toString();
        cmnClassRegister::const_iterator iterator = cmnClassRegister::begin();
        std::advance(iterator, index.row());
        cmnClassRegister::SetLogMaskClass(iterator->first.c_str(),
                                          cmnIndexToLogLevel(value.toInt()));
    }
    return true;
}


cmnLoggerQtWidgetLoDDelegate::cmnLoggerQtWidgetLoDDelegate(QObject * parent):
    QItemDelegate(parent)
{
}


QWidget * cmnLoggerQtWidgetLoDDelegate::createEditor(QWidget * parent,
                                                     const QStyleOptionViewItem & CMN_UNUSED(option),
                                                     const QModelIndex & CMN_UNUSED(index)) const
{
    QComboBox * comboBox = new QComboBox(parent);
    unsigned int lod;
    for (lod = 0;
         lod <= 8;
         lod++) {
        comboBox->insertItem(lod, QString(cmnLogLevelToString(cmnIndexToLogLevel(lod)).c_str()), QVariant(lod));
    }
    return comboBox;
}


void cmnLoggerQtWidgetLoDDelegate::setEditorData(QWidget * editor,
                                                 const QModelIndex & index) const
{
    int value = index.model()->data(index, Qt::EditRole).toInt();
    QComboBox * comboBox = static_cast<QComboBox *>(editor);
    comboBox->setCurrentIndex(value);
}


void cmnLoggerQtWidgetLoDDelegate::setModelData(QWidget * editor, QAbstractItemModel * model,
                                                const QModelIndex & index) const
{
    QComboBox * comboBox = static_cast<QComboBox *>(editor);
    int value = comboBox->currentIndex();
    model->setData(index, value, Qt::EditRole);
}


void cmnLoggerQtWidgetLoDDelegate::updateEditorGeometry(QWidget * editor,
                                                        const QStyleOptionViewItem & option,
                                                        const QModelIndex & CMN_UNUSED(index)) const
{
    editor->setGeometry(option.rect);
}


cmnLoggerQtWidget::cmnLoggerQtWidget(QWidget * parent)
{
    // create main widget and layout
    this->Widget = new QWidget(parent);
    this->Layout = new QVBoxLayout(this->Widget);
    this->Layout->setContentsMargins(0, 0, 0, 0);

    // add overall logger filter
    this->MainFilterWidget = new QWidget(this->Widget);
    this->Layout->addWidget(this->MainFilterWidget);
    this->MainFilterLayout = new QHBoxLayout(this->MainFilterWidget);
    this->MainFilterLayout->setContentsMargins(0, 0, 0, 0);
    this->MainFilterLabel = new QLabel("Overall filter", this->MainFilterWidget);
    this->MainFilterLayout->addWidget(this->MainFilterLabel);
    this->MainFilterData = new QLabel(cmnLogMaskToString(cmnLogger::GetMask()).c_str(), this->MainFilterWidget);
    this->MainFilterLayout->addWidget(this->MainFilterData);
    this->MainFilterLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    this->MainFilterData->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    // table with level per class
    this->View = new QTableView(this->Widget);
    this->Layout->addWidget(this->View);
    this->Model = new cmnLoggerQtWidgetClassServicesModel(this->View); // parent to perform cleanup?
    this->Delegate = new cmnLoggerQtWidgetLoDDelegate(this->View); // parent to perform cleanup?
    this->View->verticalHeader()->hide(); // hide column of numbers
    this->View->setModel(this->Model);
    this->View->setItemDelegate(this->Delegate);
    // adjust size
    this->View->resizeColumnsToContents();
    this->View->resizeRowsToContents();
}
