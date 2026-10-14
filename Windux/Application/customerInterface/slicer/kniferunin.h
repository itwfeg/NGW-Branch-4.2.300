#ifndef KNIFE_RUN_IN_H
#define KNIFE_RUN_IN_H

#include <QObject>
#include <QFileSystemWatcher>

class KnifeRunIn : public QObject
{
    Q_OBJECT

public:
    enum state_e
    {
        Asserted,
        DeAsserted
    };

    explicit KnifeRunIn();

    virtual state_e State () const;

public slots:
    void slt_SimulateAssert();
    void slt_SimulateDeAssert();

signals:
    void sig_Asserted(); //Emit signal to slicerstatemachine
    void sig_DeAsserted(); //Emit signal to slicerstatemachine

private slots:
    void slt_fileChanged(const QString &path);
private:
    QFileSystemWatcher knifeWatcher;

    state_e m_simulatedState;
};

#endif // KNIFE_RUN_IN_H
