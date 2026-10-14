#ifndef APPLICATIONATTACHER_H
#define APPLICATIONATTACHER_H

#include "runnerprocess.h"

class SEConfiguration;

class ApplicationAttacher : public RunnerProcess
{
    Q_OBJECT

public:
    ApplicationAttacher(SEConfiguration *config, QObject *parent = 0);

    bool start();

    int autId() const;
    QString lastErrorMessage() const;

signals:
    void ready(int autId);
    void done();
    void error();

private:
    void parseStandardOutputLine(const QByteArray &line);
    void parseStandardErrorLine(const QByteArray &line);

    int m_autId;
    QString m_lastErrorMessage;
};

#endif // APPLICATIONATTACHER_H
