#ifndef SUPERREPORTINGCLASSESTREEIIEM_H
#define SUPERREPORTINGCLASSESTREEIIEM_H

#include <QList>
#include <QVariant>
#include <QVector>

class superReportingClassesValidation;

class superReportingClassesTreeItem
{
public:
    superReportingClassesTreeItem(const QVector<QVariant> &data, superReportingClassesTreeItem *parent = 0);
    ~superReportingClassesTreeItem();

    superReportingClassesTreeItem *child(int number);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    bool insertChildren(int position, int count, int columns);
    bool insertColumns(int position, int columns);
    superReportingClassesTreeItem *parent();
    bool removeChildren(int position, int count);
    bool removeColumns(int position, int columns);
    int childNumber() const;
    bool setData(int column, const QVariant &value);

private:
    QList<superReportingClassesTreeItem*> childItems;
    QVector<QVariant> itemData;
    superReportingClassesTreeItem *parentItem;
    superReportingClassesValidation *validator;

};

#endif // SUPERREPORTINGCLASSESTREEIIEM_H
