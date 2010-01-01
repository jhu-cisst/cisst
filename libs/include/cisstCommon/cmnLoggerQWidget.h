#ifndef _cmnLoggerQWidget_h
#define _cmnLoggerQWidget_h

#include <QtGui>

class cmnLoggerQWidget
{
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

#endif // _cmnLoggerQWidget_h
