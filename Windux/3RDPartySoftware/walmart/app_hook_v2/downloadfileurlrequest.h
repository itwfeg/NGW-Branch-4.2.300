/**
 * @file downloadfileurlrequest.h
 * Download file URL event request.
 *
 * @copyright Copyright (c) 2022 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef DOWNLOADFILEURLREQUEST_H
#define DOWNLOADFILEURLREQUEST_H

#include "abstractrequest.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Download file URL event request.
class downloadFileUrlRequest : public abstractRequest
{
    Q_OBJECT

    typedef downloadFileUrlRequest _Myt;
    typedef abstractRequest _Mybase;

public:

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in,out] parent  parent object
     */
    downloadFileUrlRequest( QObject *parent = nullptr );

    /// Constructor.
    /**
     * @param[in] requestUuid  request uuid
     * @param[in,out] parent  parent object
     */
    downloadFileUrlRequest( const QString& requestUuid, QObject *parent = nullptr );

    /// Destructor.
    virtual ~downloadFileUrlRequest();

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Convert to @c QJsonDocument.
    /**
     * @return  json object
     */
    virtual QJsonDocument toJsonDoc() const;

private:

    // not implemented
    downloadFileUrlRequest( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DOWNLOADFILEURLREQUEST_H
