#include "weighcalibmodel.h"


weighmodel::weighmodel(QObject *parent)
    :QAbstractTableModel(parent),
     rows(7),columns(4)
{

}

int weighmodel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return rows;
}

int weighmodel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return columns;
}

QVariant weighmodel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
             return QVariant();

     if (orientation == Qt::Horizontal) {
         switch (section) {
             case 0:
                 return tr("lbs");

             case 1:
                 return tr("Zeroed");

             case 2:
                 return tr("delta");

             case 3:
                return tr("Non-zeroed");

             default:
                 return QVariant();
         }
     }
         return QVariant();
}


QVariant weighmodel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }
    return QVariant();
}

bool weighmodel::setData(const QModelIndex &index, const QVariant &value, int role)
{

}

bool weighmodel::insertRows(int position, int rows, const QModelIndex &index)
{

}


bool weighmodel::removeRows(int position, int rows, const QModelIndex &index)
{

}

Qt::ItemFlags weighmodel::flags(const QModelIndex &index) const
 {
     if (!index.isValid())
         return Qt::ItemIsEnabled;

     return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
 }
