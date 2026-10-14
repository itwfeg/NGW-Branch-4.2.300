#include "squishexecutive.h"

#include "applicationattacher.h"
#include "recorder.h"
#include "seconfiguration.h"

#include <QFile>
#include <QProcess>
#include <QRegularExpression>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextStream>
#include <QXmlStreamReader>

SquishExecutive::SquishExecutive(SEConfiguration *config, QObject *parent)
    : QObject(parent)
    , m_config(config)
    , m_attacher(new ApplicationAttacher(m_config, this))
    , m_recorder(new Recorder(m_config, this))
{
    connect(m_attacher, SIGNAL(ready(int)), this, SLOT(startRecorder(int)));
    connect(m_attacher, SIGNAL(error()), this, SLOT(printAttachErrorAndStop()));
    connect(m_attacher, SIGNAL(done()), this, SLOT(stopAll()));

    connect(m_recorder, SIGNAL(done()), this, SLOT(saveRecording()));
}

void SquishExecutive::acceptConnection()
{
    QTcpServer *server = qobject_cast<QTcpServer*>(sender());
    Q_ASSERT(server);

    QTcpSocket *socket = server->nextPendingConnection();
    Q_ASSERT(socket);

    connect(socket, SIGNAL(readyRead()), this, SLOT(readNetworkCommand()));
}

void SquishExecutive::readNetworkCommand()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket->canReadLine()) // Wait until we can read a full command
        return;

    QByteArray line = socket->readLine().trimmed();
    if (line.startsWith("start")) {
        const QString outputFileName = QString::fromLatin1(line.mid(5).trimmed()); // 5 == length of "start"
        start(outputFileName);
    } else if (line == "stop") {
        stopRecorder();
    } else {
        qWarning("Unknown command: %s", line.trimmed().constData());
    }
}

void SquishExecutive::start(const QString &outputFileName)
{
    const State currentState = state();
    if (currentState != AttachedAndRecording) {
        if (outputFileName.isEmpty()) {
            qWarning("Output file name is empty, recording will not start");
            return;
        }
        m_outputFileName = outputFileName;
    }

    switch (currentState) {
    case Idle:
        startAttacher();
        break;
    case Attached:
        startRecorder(m_attacher->autId());
        break;
    case AttachedAndRecording:
        qWarning("Already recording");
        break;
    }
}

void SquishExecutive::startAttacher()
{
    const bool attachLaunched = m_attacher->start();
    if (!attachLaunched) {
        qWarning("Attaching failed: %s", qPrintable(m_attacher->lastErrorMessage()));
        m_attacher->stop();
    }
}

void SquishExecutive::startRecorder(int autId)
{
    m_recorder->setAutId(autId);
    const bool recordingLaunched = m_recorder->start();
    if (!recordingLaunched) {
        qWarning("Recording failed: %s", qPrintable(m_recorder->lastErrorMessage()));
        m_recorder->stop();
    }
}

void SquishExecutive::stopRecorder()
{
    if (state() != AttachedAndRecording) {
        qWarning("Not recording anything");
        return;
    }

    // Also stop attacher otherwise we cannot playback.
    m_recorder->stop();
    m_attacher->stop();
}

void SquishExecutive::stopAll()
{
    m_recorder->stop();
    m_attacher->stop();
}

SquishExecutive::State SquishExecutive::state() const
{
    const bool attached = m_attacher->isRunning();
    const bool recording = m_recorder->isRunning();

    if (!attached && !recording)
        return Idle;
    else if (attached && !recording)
        return Attached;
    else if (attached && recording)
        return AttachedAndRecording;
    else
        qWarning("Inconsistent state, attached: %d, recording: %d", attached, recording);
    return Idle;
}

void SquishExecutive::printAttachErrorAndStop()
{
    qWarning("Attach error: %s", qPrintable(m_attacher->lastErrorMessage()));

    stopAll();
}

void SquishExecutive::saveRecording()
{
    createScriptFromSnippet(m_recorder->snippetFileName());
}

void SquishExecutive::createScriptFromSnippet(const QString &snippetFileName) const
{
    QFile snippetFile(snippetFileName);
    if (!snippetFile.open(QIODevice::ReadOnly)) {
        qWarning("Error opening snippet file: %s", qPrintable(snippetFile.errorString()));
        return;
    }

    QString scriptSnippet;
    QXmlStreamReader reader(&snippetFile);

    while (!reader.atEnd() && !reader.hasError()) {
        reader.readNext();
        if (reader.isStartElement() && reader.name() == QStringLiteral("scriptSnippet")) {
            scriptSnippet = reader.readElementText();
            break;
        }
    }

    if (reader.hasError()) {
        qWarning("Error parsing recorded file: %s", qPrintable(reader.errorString()));
        return;
    }

    saveScript(scriptSnippet);
}

void SquishExecutive::saveScript(const QString &scriptSnippet) const
{
    if (scriptSnippet.isEmpty())
        return;

    int omitCount = 0;
    QStringList lines = truncateScript(scriptSnippet, &omitCount);

    QFile scriptFile(m_outputFileName);
    if (!scriptFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        qWarning("Could not open script file [%s] for saving", qPrintable(m_outputFileName));
        return;
    }

    QTextStream stream(&scriptFile);

    stream << "def main():" << endl;
    stream << "    attachToApplication(\"" << m_config->applicationName() << "\")" << endl;
    for (int i = 0; i < lines.size(); ++i)
        stream << addWaitForObject(lines[i]) << endl;

    if (omitCount > 0)
        stream << endl << "# WARNING: More than " << m_config->maximumEventCount() << " events recorded, "
               << omitCount << " events omitted." << endl;

    scriptFile.close();
}

static bool isEventStatement(const QString &line)
{
    return !line.trimmed().startsWith(QStringLiteral("snooze("));
}

QStringList SquishExecutive::truncateScript(const QString &script, int *omitCount) const
{
    QStringList lines = script.split(QStringLiteral("\n"));

    int eventCount = 0;
    int index = 0;
    for (; index < lines.size(); ++index) {
        if (isEventStatement(lines[index]))
            ++eventCount;

        if (eventCount > m_config->maximumEventCount())
            break;
    }

    const int totalEventCount = std::count_if(lines.cbegin(), lines.cend(), isEventStatement);

    if (eventCount > m_config->maximumEventCount()) {
        lines.erase(lines.begin() + index, lines.end());
    }

    *omitCount = totalEventCount - eventCount;

    return lines;
}

QString SquishExecutive::addWaitForObject(const QString &line) const
{
    if (!isEventStatement(line))
        return line;

    // Original form (without escapes): ":[^"\\]*(?:\\.[^"\\]*)*"
    QRegularExpression symbolicObjectNameRegExp(QStringLiteral("\":[^\"\\\\]*(?:\\\\.[^\"\\\\]*)*\""));
    QRegularExpressionMatch match = symbolicObjectNameRegExp.match(line);
    if (!match.hasMatch())
        return line;

    QString newLine = line;
    newLine.insert(match.capturedEnd(), QLatin1Char(')'));
    newLine.insert(match.capturedStart(), QStringLiteral("waitForObject("));

    return newLine;
}
