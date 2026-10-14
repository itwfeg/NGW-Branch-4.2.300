/**
 * @file process.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "process.h"

#include <iostream>

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

    cleanup();

    if ( !(process_ = new QProcess()) )
        std::cout << "failed to create process!" << std::endl;
    else
    {
        // connect signals
        connect( process_, SIGNAL( finished( int, QProcess::ExitStatus ) ), SLOT( processDone( int, QProcess::ExitStatus ) ) );

        // set working directory
        if ( workingDirectory.length() )
            process_->setWorkingDirectory( workingDirectory );

        if ( inputFilename.length() )
        {
            std::cout << "input file is " << qPrintable( inputFilename ) << std::endl;
            process_->setStandardInputFile( inputFilename );
        }

        if ( outputFilename.length() )
        {
            std::cout << "output file is " << qPrintable( outputFilename ) << std::endl;
            process_->setStandardOutputFile( outputFilename );
        }

        if ( errorFilename.length() )
        {
            std::cout << "error file is" << qPrintable( errorFilename ) << std::endl;
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

            std::cout << "setting env is " << qPrintable( e ) << std::endl;
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

        std::cout << "starting process with command " << qPrintable( proc ) << std::endl;

        if ( !arguments.size() )
            process_->start( proc );
        else
        {
            foreach ( const QString& arg, arguments )
                std::cout << "command argument " << qPrintable( arg ) << std::endl;

            process_->start( proc, arguments );
        }

        if ( !process_->waitForStarted( waitTimeout ) )
        {
            std::cout << "process failed to start " << qPrintable( command ) << std::endl;

            cleanup();
        }
    }

    return active();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool process::waitForStarted(
    unsigned int waitTimeout )
{

    if ( !process_ )
        return false;

    return process_->waitForStarted( waitTimeout );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void process::stop(
    unsigned int waitTimeout )
{

    if ( !active() )
        std::cout << "process not active " << std::endl;
    else if ( !process_ )
        std::cout << "process NULL" << std::endl;
    else
    {
        std::cout << "terminateprocess " << std::endl;
        process_->terminate();

        if ( !process_->waitForFinished( waitTimeout ) )
        {
            std::cout << "process failed to terminate " << std::endl;
            process_->kill();
        }

        cleanup();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool process::waitForFinished(
    unsigned int waitTimeout )
{

    if ( !process_ )
        return false;

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

    if ( QProcess::CrashExit == exitStatus )
    {
        std::cout << "crashed process exitCode  " << qPrintable( exitCode ) << std::endl;
        std::cout << "crashed process exit status " << qPrintable( exitStatus ) << std::endl;
    }
    else
    {
        std::cout << "exited process exitCode  " << qPrintable( exitCode ) << std::endl;
        std::cout << "exited process exit status " << qPrintable( exitStatus ) << std::endl;
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

    if ( process_ )
    {
        process_->deleteLater();
        process_ = NULL;
    }
}

