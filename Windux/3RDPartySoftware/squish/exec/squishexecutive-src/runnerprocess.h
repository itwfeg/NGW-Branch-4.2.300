#ifndef RUNNERPROCESS_H
#define RUNNERPROCESS_H

#include <QObject>

class SEConfiguration;
class QProcess;

class RunnerProcess : public QObject
{
    Q_OBJECT
public:
    explicit RunnerProcess(SEConfiguration *config, QObject *parent = 0);

    virtual bool start() = 0;
    virtual void stop();

    bool isRunning() const;

protected:
    bool start(const QString &path, const QStringList &arguments);
    qint64 write(const QByteArray &data);

    bool waitForFinished(int timeout = 30000);

    SEConfiguration *config() const;

private slots:
    void readStandardOutput();
    void readStandardError();

private:
    virtual void parseStandardOutputLine(const QByteArray &line) = 0;
    virtual void parseStandardErrorLine(const QByteArray &line) = 0;

    SEConfiguration *m_config;
    QProcess *m_process;
};

#endif // RUNNERPROCESS_H
