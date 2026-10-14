#ifndef WEIGHCALIBMODEL_H
#define WEIGHCALIBMODEL_H

#include <QAbstractTableModel>

class weighmodel: public QAbstractTableModel
{
    Q_OBJECT
public:
    weighmodel(QObject* parent = 0);

    //redefine from the model
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    //API for model
    bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);

private:
    int rows;
    int columns;
};



#endif // WEIGHCALIBMODEL_H
