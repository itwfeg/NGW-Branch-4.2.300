#include "simulatedscaleinterface.h"

SimulatedScaleInterface::SimulatedScaleInterface() :
    m_weight(0.0)
{
    m_timer.setInterval(100);
    m_timer.setSingleShot(false);
    (void)connect(&m_timer, SIGNAL(timeout()), this, SLOT(slt_TimerExpired()));
    m_timer.start();
}

SimulatedScaleInterface::~SimulatedScaleInterface()
{
}

void SimulatedScaleInterface::Zero()
{
    m_weight = 0;
}

void SimulatedScaleInterface::slt_setWeight(ScaleInterface::weightInPounds_t weight)
{
    m_weight = weight;
}

void SimulatedScaleInterface::slt_TimerExpired()
{
    emit sig_weightUpdated(m_weight); //Emit signal to scale
}
