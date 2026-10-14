/**
 * @file python.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 */

// only compile when PYTHON
#ifndef DISABLE_PYTHON

#include "python.h"
#include "filesystem.h"
#include "process.h"
#include "qdebug.h"


#include <QFile>

python::python()
{
    myProcess = new process();
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
    QStringList localArgs;

    qDebug("python command is %s", qPrintable(filesystem::absoluteFilePath(commandName + ".py",folderType::Python)));

    int i=0;
    foreach(QString arg, arguments)
    {
        qDebug("argument %d is %s\n", i+1, qPrintable(arguments.at(i)));
        i++;
    }

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
        return myProcess->waitForFinished(waitTimeout);
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

#endif // DISABLE_PYTHON
