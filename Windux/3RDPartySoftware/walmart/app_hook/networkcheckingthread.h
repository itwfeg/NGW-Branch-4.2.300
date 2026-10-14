/**
 * @file networkcheckingthread.h
 * Network Checking Thread.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef NETWORKCHECKINGTHREAD_H
#define NETWORKCHECKINGTHREAD_H

#include <QDateTime>
#include <QMutex>
#include <QPair>
#include <QThread>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Network Checking Thread.
class networkCheckingThread : public QThread
{
    Q_OBJECT

    typedef networkCheckingThread _Myt;
    typedef QThread _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    networkCheckingThread( QObject *parent = 0 );

    /// Destructor.
    virtual ~networkCheckingThread();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve checking interval.
    /**
     * @return  interval (milliseconds)
     */
    virtual int checkingInterval() const;

    /// Set checking interval.
    /**
     * @param[in] value  interval (milliseconds)
     */
    virtual void setCheckingInterval( int value );

    /// Check if online.
    /**
     * @return  @c true if online, @c false otherwise
     */
    virtual bool isOnline() const;

    /// Time of last successsfull network check.
    /**
     * @return  last online time
     */
    virtual QDateTime lastOnlineTime() const;

    /// Set last online time.
    /**
     * This method allows external services to update online time. For example, if an
     * api call completes successfully we are probably online.
     * @param[in] value  online time
     */
    virtual void setLastOnlineTime( const QDateTime& value );

    /// Set offline timer max.
    /**
     * Maximum value (i.e. starting point) of offline timer countdown.
     * @param[in] value  value (milliseconds)
     */
    virtual void setOfflineTimerMax( int value );

    /// Set offline dialog times.
    /**
     * Times control when offline warning dialog pop and at what frequency they occur.
     * @param[in] d  which dialog
     * @param[in] times  dialog start and end time (milliseconds)
     * @param[in] freq  dialog frequency (milliseconds)
     */
    virtual void setDialogTimes( int d, const QPair<int, int>& times, int freq );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Force checking of network status.
    bool forceCheckOnlineStatus();

protected:

    mutable QMutex m_;

    QDateTime lastSuccess_;

    bool wired_;
    bool wireless_;
    bool lockout_;

    int checkingInterval_;

    int offlineTimerMax_;

    QPair<int, int> dialog0_;
    int dialog0_freq_;

    QPair<int, int> dialog1_;
    int dialog1_freq_;

    QDateTime lastOfflineDialog_;

    void *d_;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Thread run method.
    virtual void run() override;

private:

    enum ConnectionType
    {
        CT_WIRED,
        CT_WIRELESS
    };

    enum {DEFAULT_CHECKING_INTERVAL = 60000}; // 1m
    enum {IDLE_TIME = 250};

    enum {QUERY_IP_ADDRESS = 10000}; // 10s

    /// Check network status.
    void checkOnlineStatus();

    /// Check dialog times.
    void checkDialogTimes();

    /// Query ip address information.
    void queryIpAddress();

    /// Parse status.
    static bool parseStatus( const QString& status, int& strength, ConnectionType& ct, bool& online );

    /// Export online time to file.
    static void exportOnlineTime( const QDateTime& value );

    /// Import online time to file.
    static void importOnlineTime( QDateTime& value );

    // not implemented
    networkCheckingThread( const _Myt& );

    // not implemented
    _Myt & operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // NETWORKCHECKINGTHREAD_H
