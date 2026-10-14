/**
 * @file process.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "process.h"

#include <QCoreApplication>
#include <QProcessEnvironment>

///////////////////////////////////////////////////////////////////////////////////////////////////
process::process() :
    process_( NULL )
{
    setObjectName( "process" );
    myExitCode = 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
process::~process()
{
    cleanup();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool process::active() const
{
    return (( process_ ) && ( 0 != process_->pid() ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool process::start(
    const QString& command,
    const QStringList arguments,
    const QString& workingDirectory,
    unsigned int waitTimeout,
    const QString& inputFilename,
    const QString& outputFilename,
    const QString& errorFilename )
{
    LOG_CLASS_INFO( "process", "start" );

    LOG_TRACE( "cleaning up previous process...", NULL );
    cleanup();

    LOG_TRACE( "creating new process...", NULL );

    if ( !(process_ = new QProcess()) )
        LOG_ERROR( "failed to create process!", NULL );
    else
    {
        // connect signals
        connect( process_, SIGNAL( finished( int, QProcess::ExitStatus ) ), SLOT( processDone( int, QProcess::ExitStatus ) ) );

        // set working directory
        if ( workingDirectory.length() )
            process_->setWorkingDirectory( workingDirectory );

        if ( inputFilename.length() )
        {
            LOG_DEBUG( "input file is '%s'", qPrintable( inputFilename ) );
            process_->setStandardInputFile( inputFilename );
        }

        if ( outputFilename.length() )
        {
            LOG_DEBUG( "output file is '%s'", qPrintable( outputFilename ) );
            process_->setStandardOutputFile( outputFilename );
        }

        if ( errorFilename.length() )
        {
            LOG_DEBUG( "error file is '%s'", qPrintable( errorFilename ) );
            process_->setStandardErrorFile( errorFilename );
        }

        // check for environment variables
        QStringList commandList( command.split( ' ' ) );
        QStringList envList;

        foreach ( const QString &c, commandList )
        {
            if ( !c.contains( '=' ) )
                break;

            envList.append( c );
        }

        // setup environment
        QProcessEnvironment env( process_->processEnvironment() );

        if ( env.isEmpty() )
            env = QProcessEnvironment::systemEnvironment();

        foreach ( const QString &e, envList )
        {
            QStringList pair( e.split( '=' ) );

            LOG_DEBUG( "setting env '%s'", qPrintable( e ) );

            if ( 2 == pair.size() )
                env.insert( pair[0], pair[1] );
        }

        process_->setProcessEnvironment( env );

        // build command line
        QString proc;
        int i( 0 );

        foreach ( const QString &c, commandList )
            if ( envList.size() <= i++ )
            {
                proc.append( c );
                proc.append( " " );
            }

        proc = proc.trimmed();

        LOG_TRACE( "starting process with command '%s'...", qPrintable( proc ) );

        if ( !arguments.size() )
            process_->start( proc );
        else
        {
            foreach ( const QString& arg, arguments )
                LOG_TRACE( "command argument '%s'", qPrintable( arg ) );

            process_->start( proc, arguments );
        }

        if ( !process_->waitForStarted( waitTimeout ) )
        {
            LOG_ERROR( "process '%s' failed to start!", qPrintable( command ) );
            cleanup();
        }
    }

    return active();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool process::waitForStarted(
    unsigned int waitTimeout )
{
    LOG_CLASS_INFO( "process", "waitForStarted" );

    if ( !process_ )
        return false;

    LOG_TRACE( "waiting up to %d ms for started...", waitTimeout );

    return process_->waitForStarted( waitTimeout );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void process::stop(
    unsigned int waitTimeout )
{
    LOG_CLASS_INFO( "process", "stop" );

    if ( !active() )
        LOG_TRACE( "process not active", NULL );
    else if ( !process_ )
        LOG_WARN( "process NULL", NULL );
    else
    {
        LOG_DEBUG( "terminate process 0x%08x...", process_ );
        process_->terminate();

        LOG_TRACE( "waiting up to %d ms for finished...", waitTimeout );

        if ( !process_->waitForFinished( waitTimeout ) )
        {
            LOG_WARN( "process 0x%08x failed to terminate... killing...", process_ );
            process_->kill();
        }

        LOG_TRACE( "cleaning up...", NULL );
        cleanup();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool process::waitForFinished(
    unsigned int waitTimeout )
{
    LOG_CLASS_INFO( "process", "waitForFinished" );

    if ( !process_ )
        return false;

    LOG_TRACE( "waiting up to %d ms for finished...", waitTimeout );

    return process_->waitForFinished( waitTimeout );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QByteArray process::readStdOutput()
{
    return process_->readAllStandardOutput();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int process::pid() const
{
    if ( !process_ )
        return 0;

    return (unsigned int) process_->pid();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void process::processDone(
    int exitCode,
    QProcess::ExitStatus exitStatus )
{
    LOG_CLASS_INFO( "process", "processDone" );

    if ( QProcess::CrashExit == exitStatus )
        LOG_WARN( "process appears to have crashed code %d and status %d", exitCode, exitStatus );
    else
    {
        LOG_DEBUG( "process exited with code %d and status %d", exitCode, exitStatus );
    }

    myExitCode = exitCode;
    Q_EMIT finished();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
int process::getExitCode() const
{
    return myExitCode;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void process::cleanup()
{
    LOG_CLASS_INFO( "process", "cleanup" );

    if ( process_ )
    {
        LOG_TRACE( "deleting process 0x%08x...", process_ );

        process_->deleteLater();
        process_ = NULL;
    }
}

