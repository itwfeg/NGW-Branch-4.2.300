/**
 * @file eventcontrollerrequest.h
 * Event controller request.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef EVENTCONTROLLERREQUEST_H
#define EVENTCONTROLLERREQUEST_H

#include "fetchallproductsrequest.h"

#include <QJsonArray>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Event controller request.
class eventControllerRequest : public fetchAllProductsRequest
{
    Q_OBJECT

    typedef eventControllerRequest _Myt;
    typedef fetchAllProductsRequest _Mybase;

public:

    /// Event request types.
    enum RequestType
    {
        WasOffline,                                 ///< Was offline for an amount of time.
        ScalePowerUp,                               ///< Scale powering up.
        ScalePowerDown,                             ///< Scale powering down.
        Interval,                                   ///< Periodic interval.
        Manual                                      ///< Manual execution.
    };

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in] type  request type
     * @param[in] parent  parent object
     */
    eventControllerRequest( RequestType type, QObject *parent = nullptr );

    /// Destructor.
    virtual ~eventControllerRequest();

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Set label version information.
    /**
     * @param[in] value  label versions
     */
    virtual void setLabelVersionInfo( const QJsonArray& value ) {labels_ = value;}

    /// Set last communication date and time.
    /**
     * @param[in] value  date and time
     */
    virtual void setLastComm( const QDateTime& value ) {lastComm_ = value;}

    /// Set last item received date and time.
    /**
     * @param[in] value  date and time
     */
    virtual void setLastItemReceived( const QDateTime& value ) {lastItemReceived_ = value;}

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Convert to @c QJsonDocument.
    /**
     * @return  json object
     */
    virtual QJsonDocument toJsonDoc() const;

    // ------------------------------------------------------------------------
    // Static Methods
    // ------------------------------------------------------------------------

    /// Convert request type to string.
    /**
     * @param[in] type  request type
     * @return  string
     */
    static QString requestTypeToString( RequestType type );

protected:

    QJsonArray labels_;                             ///< Label version information.

    QDateTime lastComm_;                            ///< Last Communication.
    QDateTime lastItemReceived_;                    ///< Last Item Received.

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Retrieve health check object.
    /**
     * @return  json object
     */
    virtual QJsonObject healthCheck() const;

private:

    QJsonObject healthCheck_;

    /// Retrieve system uptime.
    static qint64 uptime();

    /// Retrieve disk info.
    static QJsonArray systemDiskInfo();

    // not implemented
    eventControllerRequest( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // EVENTCONTROLLERREQUEST_H
