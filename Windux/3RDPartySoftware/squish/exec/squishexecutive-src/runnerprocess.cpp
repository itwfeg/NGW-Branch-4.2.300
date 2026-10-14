#include "runnerprocess.h"

#include <QProcess>

RunnerProcess::RunnerProcess(SEConfiguration *config, QObject *parent)
    : QObject(parent)
    , m_config(config)
    , m_process(new QProcess(this))
{
    connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(readStandardOutput()));
    connect(m_process, SIGNAL(readyReadStandardError()), this, SLOT(readStandardError()));
}

void RunnerProcess::stop()
{
    m_process->terminate();
    if (!m_process->waitForFinished()) {
        m_process->kill();
        m_process->waitForFinished();
    }
}

bool RunnerProcess::isRunning() const
{
    return m_process->state() == QProcess::Running;
}

bool RunnerProcess::start(const QString &path, const QStringList &arguments)
{
#ifdef SQUISHEXECUTIVE_DEBUG
    qDebug("Launching '%s %s'", qPrintable(path), qPrintable(arguments.join(QLatin1Char(' '))));
#endif
    m_process->start(path, arguments);
    return m_process->waitForStarted();
}

qint64 RunnerProcess::write(const QByteArray &data)
{
    return m_process->write(data);
}

bool RunnerProcess::waitForFinished(int timeout)
{
    return m_process->waitForFinished(timeout);
}

SEConfiguration *RunnerProcess::config() const
{
    return m_config;
}

void RunnerProcess::readStandardOutput()
{
    m_process->setReadChannel(QProcess::StandardOutput);
    while (m_process->canReadLine()) {
        QByteArray line = m_process->readLine();
#ifdef SQUISHEXECUTIVE_DEBUG
        qDebug("stdout [PID: %lld]: %s", m_process->processId(), line.trimmed().constData());
#endif
        parseStandardOutputLine(line);
    }
}

void RunnerProcess::readStandardError()
{
    m_process->setReadChannel(QProcess::StandardError);
    while (m_process->canReadLine()) {
        QByteArray line = m_process->readLine();
#ifdef SQUISHEXECUTIVE_DEBUG
        qDebug("stderr [PID: %lld]: %s", m_process->processId(), line.trimmed().constData());
#endif
        parseStandardErrorLine(line);
    }
}
