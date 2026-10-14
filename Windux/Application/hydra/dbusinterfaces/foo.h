/**
 * @file foo.h
 * @brief This class does absolutely nothing, it exists in order to have a MOC target.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef FOO_H
#define FOO_H

#include <QObject>

///////////////////////////////////////////////////////////////////////////////////////////////////
class foo : public QObject
{
    Q_OBJECT
public:
    explicit foo(QObject *parent = nullptr);

signals:

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // FOO_H
