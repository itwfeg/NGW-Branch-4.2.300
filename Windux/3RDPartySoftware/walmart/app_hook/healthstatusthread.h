/**
 * @file healthstatusthread.h
 * Health Status Thread.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HEALTHSTATUSTHREAD_H
#define HEALTHSTATUSTHREAD_H

#include <QMutex>
#include <QThread>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Health Status Thread.
class healthStatusThread : public QThread
{
    Q_OBJECT

    typedef healthStatusThread _Myt;
    typedef QThread _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    healthStatusThread( QObject *parent = 0 );

    /// Destructor.
    virtual ~healthStatusThread();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve checking interval.
    /**
     * @return  interval (milliseconds)
     */
    virtual int checkingInterval() const;

    /// Retrieve health checking format.
    /**
     * @return  format
     */
    virtual QString format() const;

    /// Set checking interval.
    /**
     * @param[in] value  interval (milliseconds)
     */
    virtual void setCheckingInterval( int value );

    /// Set health checking format.
    /**
     * @param[in] value  format
     */
    virtual void setFormat( const QString& value );

protected:

    mutable QMutex m_;

    int checkingInterval_;
    int numBadAcks_;

    QString format_;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Thread run method.
    virtual void run() override;

private:

    enum {DEFAULT_CHECKING_INTERVAL = 3600000}; // 1h
    enum {IDLE_TIME = 250};

    enum {STARTUP_DELAY = 120000}; // 2m

    enum {WM_CA_BAD_ACK_CHECKING_INTERVAL = 2 * 60 * 1000}; // 2m

    /// Retrieve current time in UTC format.
    static QString systemUTC();

    /// Retrieve current timezone.
    static QString systemTimezone();

    /// Retrieve current ip address.
    static QString ipAddress();

    /// Retrieve disk information.
    static void systemDiskInfo( uint64_t& size, uint64_t& used, uint64_t& free );

    /// Retrieve last walmart api call and time.
    static void lastUsageInfo( QString& api, QString& utcTime );

    /// Retrieve firmware version information.
    static QString firmwareVersionInfo();

    /// Retrieve whitelist plu count.
    static int whitelistPluCount();

    // not implemented
    healthStatusThread( const _Myt& );

    // not implemented
    _Myt & operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HEALTHSTATUSTHREAD_H
