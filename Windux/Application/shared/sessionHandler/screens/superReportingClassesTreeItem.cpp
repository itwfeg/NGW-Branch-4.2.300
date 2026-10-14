#include "superReportingClassesTreeItem.h"

#include "superReportingClassesValidation.h"

#include <QStringList>

superReportingClassesTreeItem::superReportingClassesTreeItem(const QVector<QVariant> &data, superReportingClassesTreeItem *parent)
{
    parentItem = parent;
    itemData = data;
    validator = new superReportingClassesValidation();
}

superReportingClassesTreeItem::~superReportingClassesTreeItem()
{
    qDeleteAll(childItems);
}


superReportingClassesTreeItem *superReportingClassesTreeItem::child(int number)
{
    return childItems.value(number);
}

int superReportingClassesTreeItem::childCount() const
{
    return childItems.count();
}

int superReportingClassesTreeItem::childNumber() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<superReportingClassesTreeItem*>(this));

    return 0;
}

int superReportingClassesTreeItem::columnCount() const
{
    return itemData.count();
}

QVariant superReportingClassesTreeItem::data(int column) const
{
    return itemData.value(column);
}

bool superReportingClassesTreeItem::insertChildren(int position, int count, int columns)
{
    if (position < 0 || position > childItems.size())
        return false;

    for (int row = 0; row < count; ++row) {
        QVector<QVariant> data(columns);
        superReportingClassesTreeItem *item = new superReportingClassesTreeItem(data, this);
        childItems.insert(position, item);
    }

    return true;
}

bool superReportingClassesTreeItem::insertColumns(int position, int columns)
{
    if (position < 0 || position > itemData.size())
        return false;

    for (int column = 0; column < columns; ++column)
        itemData.insert(position, QVariant());

    foreach (superReportingClassesTreeItem *child, childItems)
        child->insertColumns(position, columns);

    return true;
}

superReportingClassesTreeItem *superReportingClassesTreeItem::parent()
{
    return parentItem;
}

bool superReportingClassesTreeItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > childItems.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete childItems.takeAt(position);

    return true;
}

bool superReportingClassesTreeItem::removeColumns(int position, int columns)
{
    if (position < 0 || position + columns > itemData.size())
        return false;

    for (int column = 0; column < columns; ++column)
        itemData.remove(position);

    foreach (superReportingClassesTreeItem *child, childItems)
        child->removeColumns(position, columns);

    return true;
}

bool superReportingClassesTreeItem::setData(int column, const QVariant &value)
{
    if (column < 0 || column >= itemData.size())
        return false;

    //if (!validator->validEntries(column, value.toString()))
    //    return false;

    itemData[column] = value;
    return true;
}
