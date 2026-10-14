#ifndef RECORDER_H
#define RECORDER_H

#include "runnerprocess.h"

#include <QTemporaryDir>
#include <QTemporaryFile>

class Recorder : public RunnerProcess
{
    Q_OBJECT

public:
    Recorder(SEConfiguration *config, QObject *parent = 0);

    void setAutId(int autId);
    QString lastErrorMessage() const;

    QString snippetFileName() const;

    bool start();
    void stop();

signals:
    void done();

private:
    void parseStandardOutputLine(const QByteArray &line);
    void parseStandardErrorLine(const QByteArray &line);

    bool writeTestSuiteConf();

    int m_autId;
    QScopedPointer<QTemporaryFile> m_snippetFile; // TODO: Change to FIFO?
    QScopedPointer<QTemporaryDir> m_testSuiteDirectory;
    QString m_lastErrorMessage;
};

#endif // RECORDER_H
