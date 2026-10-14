#include "applicationattacher.h"
#include "seconfiguration.h"

#include <QStringList>

ApplicationAttacher::ApplicationAttacher(SEConfiguration *config, QObject *parent)
    : RunnerProcess(config, parent)
    , m_autId(-1)
{
}

bool ApplicationAttacher::start()
{
    QStringList arguments;
    arguments << QStringLiteral("--startapp")
              << QStringLiteral("--cwd")
              << QStringLiteral("@app")
              << QStringLiteral("--wrapper")
              << QStringLiteral("Qt")
              << QStringLiteral("--attach")
              << config()->applicationName();

    return RunnerProcess::start(QStringLiteral("%1/bin/squishrunner").arg(config()->squishPath()), arguments);
}

int ApplicationAttacher::autId() const
{
    return m_autId;
}

QString ApplicationAttacher::lastErrorMessage() const
{
    return m_lastErrorMessage;
}

void ApplicationAttacher::parseStandardOutputLine(const QByteArray &line)
{
    if (line.startsWith("AUTID")) {
        m_autId = line.mid(line.indexOf(": ") + 2).trimmed().toInt();
    } else if (line.startsWith("application registered server")) {
        Q_ASSERT(m_autId != -1);
        emit ready(m_autId);
    } else if (line.startsWith("application exited")) {
        emit done();
    }
}

void ApplicationAttacher::parseStandardErrorLine(const QByteArray &line)
{
    if (line.startsWith("Attempt to attach to AUT failed")
            || line.startsWith("Internal Error")) {
        m_lastErrorMessage = QString::fromLatin1(line.trimmed());
        emit error();
    }
}
