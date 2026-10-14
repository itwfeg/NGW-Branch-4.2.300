/**
 * @file settingsrequest.h
 * Scale settings event request.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SETTINGSREQUEST_H
#define SETTINGSREQUEST_H

#include "abstractrequest.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Scale settings event request.
class settingsRequest : public abstractRequest
{
    Q_OBJECT

    typedef settingsRequest _Myt;
    typedef abstractRequest _Mybase;

public:

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in,out] parent  parent object
     */
    settingsRequest( QObject *parent = nullptr );

    /// Constructor.
    /**
     * @param[in] requestUuid  request uuid
     * @param[in,out] parent  parent object
     */
    settingsRequest( const QString& requestUuid, QObject *parent = nullptr );

    /// Destructor.
    virtual ~settingsRequest();

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
    settingsRequest( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SETTINGSREQUEST_H
