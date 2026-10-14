/**
 * @file abstractrequest.h
 * Abstract request object.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef ABSTRACTEVENT_H
#define ABSTRACTEVENT_H

#include <QJsonObject>
#include <QObject>

class QJsonDocument;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Abstract event/request object.
class abstractEvent : public QObject
{
    Q_OBJECT

    typedef abstractEvent _Myt;
    typedef QObject _Mybase;

public:

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Convert to @c QJsonDocument.
    /**
     * @return  json object
     */
    virtual QJsonDocument toJsonDoc() const = 0;

protected:

    QJsonObject event_;                           ///< Event information.

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in] type  event/request type
     */
    abstractEvent( QObject *parent = nullptr );

    /// Destructor.
    virtual ~abstractEvent();

private:

    // not implemented
    abstractEvent( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ABSTRACTEVENT_H
