/**
 * @file process.h
 * Platform neutral process wrapper class.
 *
 * This is a helper class that wraps the Qt @c QProcess class.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup platformCsu
 * @{
 */

#ifndef PROCESS_H
#define PROCESS_H

#include <QObject>
#include <QProcess>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Process wrapper class.
class process : public QObject
{
    Q_OBJECT

    typedef process _Myt;
    typedef QObject _Mybase;

Q_SIGNALS:

    /// This signal is emitted when process completes normally.
    void finished();

public:

    enum {DEFAULT_WAIT_TIMEOUT = 5000};                 ///< Default wait time for process starting and stopping.

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    process();

    /// Destructor.
    virtual ~process();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if process is active.
    /**
     * @return  @c true if process is active, @c false otherwise.
     */
    virtual bool active() const;

    /// Gets exit code of process execution
    /**
     * @return  exit code
     */
    virtual int getExitCode() const;

    /// Retrieve process id.
    /**
     * @return  process id
     */
    virtual unsigned int pid() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Read stdout from result process execution
    /**
     * @return  output of process
     */
    virtual QByteArray readStdOutput();

    /// Starts process.
    /**
     * This command is asyncronous.  Once the process starts control is returned to caller.  This
     * function does not block.  The command below takes command as shell binary or script to execute
     * and arguments are the command line arguments to the command.  One should just specify the
     * binary or script name for command and for arguments provide a string list of all command line
     * options.  You can ignore the command parameter and just put entire binary/script name and arguments
     * in the command parameter but you will have to get the quoting right of whitespace arguments and even
     * then no guarantee that DOS will interpret it right
     *
     * @param[in] command
     * @param[in] arguments  arguments to command
     * @param[in] workingDirectory  working directory where command should run
     * @param[in] waitTimeout  max amount of time to wait for process to become active
     * @param[in] inputFilename  file to pass as input to process
     * @param[in] outputFilename  file to pipe process standard output into
     * @param[in] errorFilename  file to pipe process standard error into
     * @return  @c true if process launched successfully, @c false otherwise
     */
    virtual bool start(
        const QString& command,
        const QStringList arguments=QStringList(),
        const QString& workingDirectory=QString(),
        unsigned int waitTimeout = DEFAULT_WAIT_TIMEOUT,
        const QString& inputFilename = "",
        const QString& outputFilename = "",
        const QString& errorFilename = "" );

    /// Stop process.
    /**
     * If the process does not stop within the passed in time it will be terminated forcefully.
     *
     * @param[in] waitTimeout  max amount of time to wait for process to stop
     */
    virtual void stop(
        unsigned int waitTimeout = DEFAULT_WAIT_TIMEOUT );

    /// Wait for a process to finish.
    /**
     * @param[in] waitTimeout  max amount of time to wait for process to stop
     * @return  @c true if process finished successfully, @c false otherwise
     */
    virtual bool waitForFinished(
        unsigned int waitTimeout = DEFAULT_WAIT_TIMEOUT );

    /// Wait for a process to start.
    /**
     * @param[in] waitTimeout  max amount of time to wait for process to become active
     * @return  @c true if process launched successfully, @c false otherwise
     */
    virtual bool waitForStarted(
        unsigned int waitTimeout = DEFAULT_WAIT_TIMEOUT );

protected:

    QProcess *process_;                                 ///< pointer to process object

    // ========================================================================
    // Methods
    // ========================================================================

    /// Cleanup variables.
    virtual void cleanup();

private Q_SLOTS:

    /// Slot to receve process exit signal.
    /**
     * This slot is called when the process ends naturally (i.e. we are not killing it).
     *
     * @param[in] exitCode  exit code of process
     * @param[in] exitStatus  exit status of process
     */
    void processDone( int exitCode, QProcess::ExitStatus exitStatus );

private:

    int myExitCode;

    // ========================================================================

    // not implemented
    process( const _Myt& );

    // not implemented
    _Myt& operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // PROCESS_H

