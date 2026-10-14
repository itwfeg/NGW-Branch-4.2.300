#ifndef AND_EK12KI_SERIALINTERFACE_H
#define AND_EK12KI_SERIALINTERFACE_H

#include <QObject>
#include <QThread>
#include "scaleinterface.h"

class AND_EK12Ki_SerialInterface_Parser : public QObject
{
    Q_OBJECT

public:
    void Weigh();

signals:
    void sig_Weight(ScaleInterface::weightInPounds_t weight, ScaleInterface::weightInPounds_t ounces);
};

class AND_EK12Ki_SerialInterface_WorkerThread : public QThread
{
    Q_OBJECT

public:
    AND_EK12Ki_SerialInterface_WorkerThread();
    void SetFd(int fd) { m_fd = fd; }
    void Terminate() { m_shouldRun = false; }

private:
    AND_EK12Ki_SerialInterface_Parser m_parser;
    volatile bool m_shouldRun;
    int m_fd;

    void run();

signals:
    void sig_weightReceived(ScaleInterface::weightInPounds_t weight,ScaleInterface::weightInPounds_t ounces);
};
    void sig_OnWeigherKeyPressed();

class AND_EK12Ki_SerialInterface : public QObject, public ScaleInterface
{
    Q_OBJECT

public:
    explicit AND_EK12Ki_SerialInterface();
    ~AND_EK12Ki_SerialInterface();
    void Zero();

signals:
    void sig_weightUpdated(ScaleInterface::weightInPounds_t weight,ScaleInterface::weightInPounds_t ounces);

private:
    int m_fd;
    AND_EK12Ki_SerialInterface_WorkerThread m_workerThread;
};

#endif // AND_EK12KI_SERIALINTERFACE_H
