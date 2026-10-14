/**
 * @file configrequest.h
 * Configuration event request.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef CONFIGREQUEST_H
#define CONFIGREQUEST_H

#include "abstractrequest.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Configuration event request.
class configRequest : public abstractRequest
{
    Q_OBJECT

    typedef configRequest _Myt;
    typedef abstractRequest _Mybase;

public:

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in,out] parent  parent object
     */
    configRequest( QObject *parent = nullptr );

    /// Constructor.
    /**
     * @param[in] requestUuid  request uuid
     * @param[in,out] parent  parent object
     */
    configRequest( const QString& requestUuid, QObject *parent = nullptr );

    /// Destructor.
    virtual ~configRequest();

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Set scale info for init event.
    /**
     * @param[in] scaleVendor  scale vendor
     * @param[in] scaleModel  scale model
     */
    virtual void setInitScaleInfo( const QString& scaleVendor, const QString& scaleModel );

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
    configRequest( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // CONFIGREQUEST_H
