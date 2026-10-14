#include "recorder.h"

#include "seconfiguration.h"

#include <QStringList>
#include <QTemporaryFile>
#include <QTextStream>

Recorder::Recorder(SEConfiguration *config, QObject *parent)
    : RunnerProcess(config, parent)
    , m_autId(-1)
    , m_snippetFile(new QTemporaryFile)
    , m_testSuiteDirectory(new QTemporaryDir(QStringLiteral("%1/suite_XXXXXX").arg(QDir::tempPath())))
{
}

void Recorder::setAutId(int autId)
{
    m_autId = autId;
}

QString Recorder::lastErrorMessage() const
{
    return m_lastErrorMessage;
}

QString Recorder::snippetFileName() const
{
    return m_snippetFile->fileName();
}

bool Recorder::start()
{
    Q_ASSERT(m_autId != -1);

    if (!m_snippetFile->open()) {
        m_lastErrorMessage = m_snippetFile->errorString();
        return false;
    }

    if (!m_testSuiteDirectory->isValid()) {
        m_lastErrorMessage = tr("Could not create temporary test suite directory");
        return false;
    }

    if (!writeTestSuiteConf()) {
        m_lastErrorMessage = tr("Could not create temporary test suite configuration");
        return false;
    }

    QStringList arguments;
    arguments << QStringLiteral("--record")

              << QStringLiteral("--outfile")
              << m_snippetFile->fileName()

              << QStringLiteral("--suitedir")
              << m_testSuiteDirectory->path()

              << QStringLiteral("--recordStart")

              << QStringLiteral("--autid")
              << QString::number(m_autId);

    return RunnerProcess::start(QStringLiteral("%1/bin/squishrunner").arg(config()->squishPath()), arguments);
}

void Recorder::stop()
{
    if (!isRunning())
        return;

    write("endrecord\r\n");

    if (!waitForFinished())
        RunnerProcess::stop();

    emit done();
}

void Recorder::parseStandardOutputLine(const QByteArray &line)
{
    Q_UNUSED(line);
}

void Recorder::parseStandardErrorLine(const QByteArray &line)
{
    Q_UNUSED(line);
    // Do nothing
}

bool Recorder::writeTestSuiteConf()
{
    const QString confFileContent = QStringLiteral("AUT=\n"
                                                   "LANGUAGE=Python\n"
                                                   "TEST_CASES=\n"
                                                   "VERSION=3\n"
                                                   "WRAPPERS=Qt\n"
                                                   "OBJECTMAP=%1\n").arg(config()->objectMapFileName());

    QFile confFile(QStringLiteral("%1/suite.conf").arg(m_testSuiteDirectory->path()));
    if (!confFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        return false;

    QTextStream str(&confFile);
    str << confFileContent;
    return true;
}
