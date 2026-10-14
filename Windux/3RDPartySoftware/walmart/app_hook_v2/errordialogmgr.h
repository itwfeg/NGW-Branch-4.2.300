/**
 * @file errordialogmgr.h
 * Error dialog manager.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef ERRORDIALOGMGR_H
#define ERRORDIALOGMGR_H

#include <QMutex>
#include <QObject>
#include <QProcess>

class QDateTime;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Error dialog manager.
class errorDialogManager : public QObject
{
    Q_OBJECT

    typedef errorDialogManager _Myt;
    typedef QObject _Mybase;

signals:

    /// Signal for dialog complete.
    /**
     * @param[in,out] d  dialog
     * @param[in] accepted  @c true if accepted, @c false otherwise
     */
    void dialogComplete( QProcess *d, bool accepted );

public:

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Show comm error dialog.
    /**
     * Basic flow step 6
     * Alternate flow 5.a and 5.b
     * @param[in] offlineTime  when offline was detected
     * @param[in] hoursRemaining  how long cache still valid (hours)
     * @return  @c true if they want to print, @c false otherwise
     */
    bool showCommError( const QDateTime& offlineTime, int hoursRemaining );

    /// Show continue to use scale dialog.
    /**
     * Sub flow 5.a and 5.b
     * @param[in] offlineTime  when offline was detected
     * @param[in] hoursRemaining  how long cache still valid (hours)
     * @param[out] d  pointer to dialog pointer
     */
    void showContinueToUse( const QDateTime& offlineTime, int hoursRemaining, void **d = nullptr );

    /// Show error still detected dialog.
    /**
     * Basic flow step 12
     * @param[in] waitTime  how long to display message (ms)
     */
    void showErrorStillDetected( int waitTime );

    /// Show scale not available dialog.
    /**
     * Alternate flow 5.c
     * @param[in] offlineTime  when offline was detected
     * @param[out] d  pointer to dialog pointer
     */
    void showNotAvailable( const QDateTime& offlineTime, void **d = nullptr );

    /// Show loading database dialog.
    /**
     * @param[in] waitTime  how long to display message (ms)
     */
    void showLoadingDatabase( int waitTime );

    /// Show loading database already in progress dialog.
    /**
     * @param[in] waitTime  how long to display message (ms)
     */
    void showLoadingDatabaseInProgressError( int waitTime );

    /// Show shutdown in progress dialog.
    /**
     * @param[out] d  pointer to dialog pointer
     */
    void showShutdownInProgress( void **d = nullptr );

    // ------------------------------------------------------------------------
    // Static Methods
    // ------------------------------------------------------------------------

    /// Retrieve global instance.
    /**
     * @return  pointer to instance
     */
    static _Myt *instance();

public slots:

    /// Terminate dialog.
    /**
     * @param[in] d  pointer to dialog or @c nullptr to eliminate currently displayed
     */
    void terminate( void *d = nullptr );

private slots:

    /// Slot to handle process finished.
    void onProcessFinished( int exitCode, QProcess::ExitStatus exitStatus );

    /// Raise dialog to front.
    void raiseDialog();

private:

    enum {CHECKING_INTERVAL = 5000};

    static QMutex instanceMutex_;
    static _Myt *instance_;

    mutable QMutex m_;

    QTimer *dialogChecking_;

    QProcess *d_;

    QList<QProcess*> dlist_;

    // Constructor.
    errorDialogManager( QObject *parent = nullptr );

    // Destructor.
    ~errorDialogManager();

    // Create process.
    bool createProcess( const QString& prog, const QStringList& args, bool blocking = false );

    // Show window fullscreen and update zorder.
    static bool bringToFront( const QString& name );

    // not implemented
    errorDialogManager( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ERRORDIALOGMGR_H
