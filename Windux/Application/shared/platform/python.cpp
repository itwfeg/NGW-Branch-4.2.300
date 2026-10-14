/**
 * @file python.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 */

// only compile when PYTHON
#ifndef DISABLE_PYTHON


#include <logmanager.h>
#include "python.h"
#include "filesystem.h"
#include "process.h"
#include "qdebug.h"

#include <QFile>

python::python()
{
    myProcess = new process();
    nonBlocking = false;
}

python::~python()
{
    cleanup();
}

bool python::execCommand(QString commandName,
                         QStringList arguments,
                         bool logToStdOut,
                         bool logToStdErr,
                         unsigned int waitTimeout)
{

    LOG_CLASS_INFO( "python", "execCommand" );

    LOG_DEBUG( "python file to execute: %s", qPrintable( commandName ) );

    QStringList localArgs;

    qDebug("python command is %s", qPrintable(filesystem::absoluteFilePath(commandName + ".py",folderType::Python)));

    int i=0;
    foreach(QString arg, arguments)
    {
        qDebug("argument %d is %s\n", i+1, qPrintable(arguments.at(i)));
        i++;
    }

    scriptName_ = commandName;

    QString script( filesystem::absoluteFilePath( commandName + ".py", folderType::Python ) );

    // look for compiled script
    if ( !QFile::exists( script ) )
        if ( QFile::exists( script + "c" ) )
            script.append( "c" );

    localArgs.append(script);
    localArgs.append(arguments);

    QString stdoutLog = QString("");
    QString stderrLog = QString("");

    if (logToStdOut)
        stdoutLog = QString(filesystem::absoluteFilePath(commandName + ".out",folderType::Python));

    if (logToStdErr)
        stderrLog = QString(filesystem::absoluteFilePath(commandName + ".err",folderType::Python));

    if (myProcess->start("python",
                    localArgs,
                    filesystem::folder(folderType::Python),
                    waitTimeout,
                    "",
                    stdoutLog, // can not write to log file if need stdout results
                    stderrLog
                    ))
    {
        if ( nonBlocking == false )
            return myProcess->waitForFinished(waitTimeout);

        // run process in the background
        else
        {
            connect(myProcess, SIGNAL (finished()), SLOT(onProcFinished()));
            return true;
        }
    }
    else
        return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString python::readStdOut( bool trim )
{
    QString output;

    output = QString(myProcess->readStdOutput());
    if (trim)
        output = output.trimmed();
    return output;
}

int python::getExitCode()
{
    return myProcess->getExitCode();
}

void python::cleanup()
{
    if (myProcess)
    {
        delete myProcess;
        myProcess = NULL;
    }
}

void python::onProcFinished()
{
    Q_EMIT readyReadScript();
}

#endif // DISABLE_PYTHON
