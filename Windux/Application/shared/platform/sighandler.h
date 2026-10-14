/**
 * @file sighandler.h
 * Class for handling UNIX signals.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup platformCsu
 * @{
 */

#ifndef SIGHANDLER_H
#define SIGHANDLER_H

#include <QMap>
#include <QMutex>
#include <QObject>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Class for handling UNIX signals.
class sigHandler : public QObject
{
    Q_OBJECT

    typedef sigHandler _Myt;
    typedef QObject _Mybase;

Q_SIGNALS:

    /// Emitted when application recieves SIGHUP.
    void hup();

    /// Emitted when application recieves SIGTERM.
    void term();

public:

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Retrieve pointer to singleton instance.
    /**
     * @return  pointer to instance
     */
    static sigHandler *instance();

private Q_SLOTS:

    /// Slot to handle signals.
    void onSignal( int sig );

    /// Slot to handle socket activation.
    void onSocketActivated( int fd );

private:

    QMutex m_;

    typedef QMap<int, int> sigHandlerMap;
    sigHandlerMap readers_; // socket -> SIGNAL
    sigHandlerMap writers_; // SIGNAL -> socket

    static sigHandler *instance_;
    static QMutex instanceMutex_;

    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    sigHandler( QObject *parent );

    /// Destructor.
    ~sigHandler();

    /// Initialize.
    void initialize();

    /// Signal handler.
    static void handlerFunc( int sig );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SIGHANDLER_H
