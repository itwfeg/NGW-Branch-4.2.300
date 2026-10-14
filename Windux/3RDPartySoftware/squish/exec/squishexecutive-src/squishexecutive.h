#ifndef SQUISHEXECUTIVE_H
#define SQUISHEXECUTIVE_H

#include <QObject>

class ApplicationAttacher;
class Recorder;
class SEConfiguration;

class QIODevice;

class SquishExecutive : public QObject
{
    Q_OBJECT
public:
    explicit SquishExecutive(SEConfiguration *config, QObject *parent = 0);


public slots:
    void acceptConnection();
    void stopAll();

private slots:
    void readNetworkCommand();

    void printAttachErrorAndStop();

    void startRecorder(int autId);
    void saveRecording();

private:
    enum State {
        Idle,
        Attached,
        AttachedAndRecording
    };

    State state() const;

    void start(const QString &outputFileName);
    void startAttacher();
    void stopRecorder();

    void createScriptFromSnippet(const QString &snippetFileName) const;
    void saveScript(const QString &scriptSnippet) const;

    QStringList truncateScript(const QString &script, int *omitCount) const;
    QString addWaitForObject(const QString &line) const;

    SEConfiguration *m_config;
    ApplicationAttacher *m_attacher;
    Recorder *m_recorder;
    QString m_outputFileName;
};

#endif // SQUISHEXECUTIVE_H
