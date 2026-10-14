#ifndef PYTHON_H
#define PYTHON_H

// only compile when PYTHON
#ifndef DISABLE_PYTHON

#include <QString>
#include "process.h"

class python
{
public:
    enum {DEFAULT_WAIT_TIMEOUT = 600000};

    python();
    ~python();
    bool execCommand(QString scriptName,
                     QStringList arguments=QStringList(),
                     bool logToStdOut=false,
                     bool logToStdErr=false,
                     unsigned intWaitTimeout=DEFAULT_WAIT_TIMEOUT);
    QString readStdOut(bool trim=true);
    int getExitCode();
    void cleanup();

private:
    process *myProcess;
};

#endif // DISABLE_PYTHON
#endif // PYTHON_H

