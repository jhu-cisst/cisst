#pragma once

#ifndef _cmnLoggerQWidget_h
#define _cmnLoggerQWidget_h

#include <QtGui>

#include <cisstCommon/cmnGenericObject.h>

// Always include last
#include <cisstCommon/cmnExport.h>

class CISST_EXPORT cmnLoggerQWidget: public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

 public:

    /*! Model class used for class register entries */
    class ClassServicesModel: public QAbstractTableModel
    {
    public:
	ClassServicesModel(QObject * parent = 0);
	int rowCount(const QModelIndex & parent) const;
	int columnCount(const QModelIndex & parent) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	QVariant data(const QModelIndex & index, int role) const;
	Qt::ItemFlags flags(const QModelIndex & index) const;
	bool setData(const QModelIndex & index, const QVariant & value, int role);
    };

    /*! Delegate class to handle LoD changes in model */
    class LoDDelegate: public QItemDelegate
    {
    public:
	LoDDelegate(QObject * parent = 0);
	QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
	void setEditorData(QWidget * editor, const QModelIndex & index) const;
	void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
	void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const;
    };

    cmnLoggerQWidget(void);

    void setupUi(void);

    void retranslateUi(void);

 protected:
    ClassServicesModel Model;
    LoDDelegate Delegate;
    QTableView View;

};

CMN_DECLARE_SERVICES_INSTANTIATION(cmnLoggerQWidget);

#endif // _cmnLoggerQWidget_h
