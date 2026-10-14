#ifndef SIMULATED_SCALE_INTERFACE_H
#define SIMULATED_SCALE_INTERFACE_H

#include <QObject>
#include <QTimer>
#include "scaleinterface.h"

class SimulatedScaleInterface : public QObject, public ScaleInterface
{
    Q_OBJECT

public:
    explicit SimulatedScaleInterface();
    ~SimulatedScaleInterface();
    void Zero();

public slots:
    void slt_setWeight(ScaleInterface::weightInPounds_t weight);

signals:
    void sig_weightUpdated(ScaleInterface::weightInPounds_t weight); //Emit signal to scale

private slots:
    void slt_TimerExpired();

private:
    QTimer m_timer;
    ScaleInterface::weightInPounds_t m_weight;
};

#endif // SIMULATED_SCALE_INTERFACE_H
