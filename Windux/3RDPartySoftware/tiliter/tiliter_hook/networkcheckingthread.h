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

signals:

    /// Signal for online status changed.
    /**
     * @param[in] online  @c true if online, @c false otherwise
     * @param[in] offlineAmount  amount of time offline (ms)
     */
    void onlineStatusChanged( bool online, int offlineAmount );

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    networkCheckingThread( QObject *parent = nullptr );

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

    // ========================================================================
    // Methods
    // ========================================================================

    /// Force checking of network status.
    /**
     * @return  @c true if online, @c false otherwise
     */
    virtual bool forceCheckOnlineStatus();

protected:

    mutable QMutex m_;

    QDateTime lastSuccess_;

    bool wired_;
    bool wireless_;

    int checkingInterval_;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Thread run method.
    virtual void run() override;

private:

    typedef QPair<int, int> dialogStartStopTime;

    enum ConnectionType
    {
        CT_WIRED,
        CT_WIRELESS
    };

    enum {DEFAULT_CHECKING_INTERVAL = 60000}; // 1m
    enum {IDLE_TIME = 250};

    /// Check network status.
    void checkOnlineStatus();

    /// Parse status.
    static bool parseStatus( const QString& status, int& strength, ConnectionType& ct, bool& online );

    /// Export online time to file.
    static void exportOnlineTime( const QDateTime& value );

    /// Import online time to file.
    static void importOnlineTime( QDateTime& value );

    // not implemented
    networkCheckingThread( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // NETWORKCHECKINGTHREAD_H
