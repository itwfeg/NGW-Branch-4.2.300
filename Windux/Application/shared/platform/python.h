#ifndef PYTHON_H
#define PYTHON_H

// only compile when PYTHON
#ifndef DISABLE_PYTHON

#include <QString>
#include "process.h"
#include <QObject>

class python : public QObject
{
    Q_OBJECT
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

    inline void setNonBlocking(){ nonBlocking = true; }
    inline bool getNonBlocking() const {return nonBlocking; }
    inline QString scriptName()const { return scriptName_; }


signals:
    void readyReadScript();

private:
    process *myProcess;
    bool nonBlocking;
    QString scriptName_;

private slots:

    void onProcFinished();
};

#endif // DISABLE_PYTHON
#endif // PYTHON_H
