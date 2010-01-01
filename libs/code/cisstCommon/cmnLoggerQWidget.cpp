
#include <cisstCommon/cmnLoggerQWidget.h>
#include <cisstCommon/cmnClassRegister.h>


cmnLoggerQWidget::ClassServicesModel::ClassServicesModel(QObject * parent):
    QAbstractTableModel(parent)
{
}


int cmnLoggerQWidget::ClassServicesModel::rowCount(const QModelIndex & CMN_UNUSED(parent)) const
{
    return cmnClassRegister::size();
}


int cmnLoggerQWidget::ClassServicesModel::columnCount(const QModelIndex & CMN_UNUSED(parent)) const
{
    return 2;
}


QVariant cmnLoggerQWidget::ClassServicesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
	if (role == Qt::DisplayRole) {
	    if (section == 0) {
		return QString("Class");
	    } else if (section == 1) {
		return QString("Level of Detail");
	    }
	}
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}


QVariant cmnLoggerQWidget::ClassServicesModel::data(const QModelIndex & index, int role) const
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
	    // get the string associated to the level of detail
	    return QString(cmnLogLoDString[iterator->second->GetLoD()]);
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


Qt::ItemFlags cmnLoggerQWidget::ClassServicesModel::flags(const QModelIndex & index) const
{
    if (!index.isValid()) {
	return 0;
    }
    // only the second column can be edited
    if (index.column() == 0) {
	return QAbstractItemModel::flags(index);
    } else {
	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }
}


bool cmnLoggerQWidget::ClassServicesModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (index.isValid() && (role == Qt::EditRole || 
			    role == Qt::DisplayRole)) {
	// add 1 to the row index to skip over the header
	// addressBook[index.row()+1][index.column()] = value.toString();
	cmnClassRegister::const_iterator iterator = cmnClassRegister::begin();
	std::advance(iterator, index.row());
	cmnClassRegister::SetLoD(iterator->first.c_str(),
				 static_cast<cmnLogLoD>(value.toInt() + 1));

	std::cout << "class: " << index.row() 
		  << " : " << iterator->first.c_str()
		  << " changed to " << value.toString().toStdString() << std::endl;
    }
    return true;

}


cmnLoggerQWidget::LoDDelegate::LoDDelegate(QObject * parent):
    QItemDelegate(parent)
{
}


QWidget * cmnLoggerQWidget::LoDDelegate::createEditor(QWidget * parent,
						      const QStyleOptionViewItem & option,
						      const QModelIndex & index) const
{
    QComboBox * comboBox = new QComboBox(parent);
    unsigned int lod;
    for (lod = CMN_LOG_LOD_INIT_ERROR;
	 lod < CMN_LOG_LOD_NOT_USED;
	 lod++) {
	comboBox->insertItem(lod, QString(cmnLogLoDString[lod]), QVariant(lod));
    }
    return comboBox;
}


void cmnLoggerQWidget::LoDDelegate::setEditorData(QWidget * editor,
						  const QModelIndex & index) const
{
    int value = index.model()->data(index, Qt::EditRole).toInt();
    QComboBox * comboBox = static_cast<QComboBox *>(editor);
    comboBox->setCurrentIndex(value);
}


void cmnLoggerQWidget::LoDDelegate::setModelData(QWidget * editor, QAbstractItemModel * model,
						 const QModelIndex & index) const
{
    QComboBox * comboBox = static_cast<QComboBox *>(editor);
    int value = comboBox->currentIndex();
    model->setData(index, value, Qt::EditRole);
}


void cmnLoggerQWidget::LoDDelegate::updateEditorGeometry(QWidget * editor,
							 const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    editor->setGeometry(option.rect);
}


cmnLoggerQWidget::cmnLoggerQWidget(void)
{
}


void cmnLoggerQWidget::setupUi(void)
{
    this->View.verticalHeader()->hide(); // hide column of numbers
    this->View.setModel(&(this->Model));
    this->View.setItemDelegate(&(this->Delegate));
    this->View.show();
}


void cmnLoggerQWidget::retranslateUi(void)
{
 
}
