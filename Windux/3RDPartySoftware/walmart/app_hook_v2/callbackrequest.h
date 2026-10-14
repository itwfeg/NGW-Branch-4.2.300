/**
 * @file callbackrequest.h
 * Callback request.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef CALLBACKREQUEST_H
#define CALLBACKREQUEST_H

#include "abstractrequest.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Callback request.
class callbackRequest : public abstractRequest
{
    Q_OBJECT

    typedef callbackRequest _Myt;
    typedef abstractRequest _Mybase;

public:

    /// Event result types.
    enum EventResult
    {
        SUCCESS,                                    ///< Event succeeded.
        FAILURE_BACKOUT_NOT_ATTEMPTED,              ///< Event failed, backout was not attempted.
        FAILURE_BACKOUT_SUCCESS,                    ///< Event failed, backout succeeded.
        FAILURE_BACKOUT_FAILED                      ///< Event failed, backout failed.
    };

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in] uuid  callback uuid
     * @param[in] eventType  callback event type
     * @param[in] result  event result
     * @param[in] errorText  error text
     * @param[in] parent  parent object
     */
    callbackRequest( const QString& uuid, const QString& eventType, EventResult result = SUCCESS, const QString& errorText = QString(), QObject *parent = nullptr );

    /// Destructor.
    virtual ~callbackRequest();

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Convert to @c QJsonDocument.
    /**
     * @return  json object
     */
    virtual QJsonDocument toJsonDoc() const;

protected:

    QString uuid_;                                  ///< Callback uuid
    QString eventType_;                             ///< Callback event type

    bool success_;                                  ///< Event was successful.

    bool backoutAttempted_;                         ///< Event backout was attempted.
    bool backout_;                                  ///< Event backout was successful.

    QString errors_;                                ///< Errors.
    QString logs_;                                  ///< Logs.

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Retrieve callback object.
    /**
     * @return  json object
     */
    virtual QJsonObject callback() const;

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Collect logs.
    virtual void collectLogs();

private:

    enum {NUM_LOG_LINES = 10};

    // not implemented
    callbackRequest( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // CALLBACKREQUEST_H
