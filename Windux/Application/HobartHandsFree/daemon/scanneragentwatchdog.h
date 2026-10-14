/**
 * @file scanneragentwatchdog.h
 * Scanner agent watchdog class.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SCANNERAGENTWATCHDOG
#define SCANNERAGENTWATCHDOG

#include <QObject>

class hobartTagManager;
class otaUpgradeManager;
class scannerAgent;

class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Toggle scanner agent on/off based on active connections.
class scannerAgentWatchdog : public QObject
{
    Q_OBJECT

    typedef scannerAgentWatchdog _Myt;
    typedef QObject _Mybase;

public:

    /// Constructor.
    /**
     * @param[in] agent  scanner agent to watch
     * @param[in] parent  parent object
     */
    scannerAgentWatchdog( scannerAgent *agent, QObject *parent = 0 );

    /// Destructor.
    virtual ~scannerAgentWatchdog();

private slots:

    /// Slot to handle timeout.
    void onTimeout();

    /// Slot to handle active connections.
    void onUpdateActiveConnections( int active );

private:

    enum {ACTIVE_TIMEOUT = 1800000}; // 30m
    enum {RESET_AGENT_TIMEOUT = 10000}; // 10s

    hobartTagManager *tagManager_;
    otaUpgradeManager *upgradeManager_;
    scannerAgent *agent_;

    int active_;

    QTimer *activeTimer_;
    QTimer *resetAgentTimer_;

    // not implemented
    scannerAgentWatchdog( const _Myt & );

    // not implemented
    _Myt & operator = ( const _Myt & );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SCANNERAGENTWATCHDOG

