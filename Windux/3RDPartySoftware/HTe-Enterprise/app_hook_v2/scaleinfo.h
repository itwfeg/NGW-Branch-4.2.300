/**
 * @file scaleconfig.h
 * Scale configuration for HTe servers.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SCALEINFO_H
#define SCALEINFO_H

#include <QDateTime>
#include <QJsonObject>
#include <QJsonValue>
#include <QMap>
#include <QMutex>
#include <QObject>

class QJsonDocument;
class QString;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Scale configuration.
class scaleInfo : public QObject
{
    Q_OBJECT

    typedef scaleInfo _Myt;
    typedef QObject _Mybase;

public:

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    scaleInfo( QObject *parent = nullptr );

    /// Destructor.
    virtual ~scaleInfo();


    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Convert to @c QJsonObject.
    /**
     * @return  json object
     */
    QJsonObject toJsonObject() const;

    // ------------------------------------------------------------------------
    // Static Methods
    // ------------------------------------------------------------------------

    /// Retrieve global instance.
    /**
     * @return  pointer to instance
     */
    static _Myt *instance();

private:

    static _Myt *instance_;

    // not implemented
    scaleInfo( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SCALEINFO_H
