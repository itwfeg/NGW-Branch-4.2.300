/**
 * @file abstractrequest.h
 * Abstract request object.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef ABSTRACTREQUEST_H
#define ABSTRACTREQUEST_H

#include <QJsonObject>
#include <QObject>

class QJsonDocument;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Abstract request object.
class abstractRequest : public QObject
{
    Q_OBJECT

    typedef abstractRequest _Myt;
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

    QJsonObject request_;                           ///< Request information.

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in] type  request type
     */
    abstractRequest( QObject *parent = nullptr );

    /// Destructor.
    virtual ~abstractRequest();

private:

    // not implemented
    abstractRequest( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ABSTRACTREQUEST_H
