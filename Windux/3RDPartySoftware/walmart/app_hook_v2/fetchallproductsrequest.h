/**
 * @file fetchallproductsrequest.h
 * Fetch all products event request.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef FETCHALLPRODUCTSREQUEST_H
#define FETCHALLPRODUCTSREQUEST_H

#include "abstractrequest.h"

#include <QDateTime>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Fetch all products event request.
class fetchAllProductsRequest : public abstractRequest
{
    Q_OBJECT

    typedef fetchAllProductsRequest _Myt;
    typedef abstractRequest _Mybase;

public:

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in,out] parent  parent object
     */
    fetchAllProductsRequest( QObject *parent = nullptr );

    /// Constructor.
    /**
     * @param[in] requestUuid  request uuid
     * @param[in,out] parent  parent object
     */
    fetchAllProductsRequest( const QString& requestUuid, QObject *parent = nullptr );

    /// Destructor.
    virtual ~fetchAllProductsRequest();

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Set business unit type.
    /**
     * @param[in] value  business unit type
     */
    virtual void setBusinessUnitType( int value ) {businessUnitType_ = value;}

    /// Set last fetch product offset timestamp.
    /**
     * @param[in] value  timestamp
     */
    virtual void setLastFetchProductStamp( const QString& value ) {lastFetch_ = value;}

    /// Set product count.
    /**
     * @param[in] value  number of products
     */
    virtual void setProductCount( int value ) {productCount_ = value;}

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Convert to @c QJsonDocument.
    /**
     * @return  json object
     */
    virtual QJsonDocument toJsonDoc() const;

protected:

    int businessUnitType_;                          ///< Business unit type.

    QString lastFetch_;                             ///< Last fetch product offset timestamp.

    int productCount_;                              ///< Number of products.

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Retrieve plu object.
    /**
     * @return  json object
     */
    virtual QJsonObject plu() const;

private:

    // not implemented
    fetchAllProductsRequest( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // FETCHALLPRODUCTSREQUEST_H
