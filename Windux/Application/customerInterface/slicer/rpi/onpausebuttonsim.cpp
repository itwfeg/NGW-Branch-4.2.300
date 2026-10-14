#include "onpausebuttonsim.h"
#include <QSettings>

RaspPiOnPauseButtonSim::RaspPiOnPauseButtonSim(RaspberryPi::pin_t pin) :
    m_pin(pin)
{
    pinMode(m_pin, OUTPUT);
    digitalWrite(m_pin, RaspberryPi::GpioLow);
}

void RaspPiOnPauseButtonSim::slt_SimulatePress()
{
    digitalWrite(m_pin, RaspberryPi::GpioHigh);

    OnPauseButtonSim::slt_SimulatePress();
}

void RaspPiOnPauseButtonSim::slt_SimulateUnpress()
{
    digitalWrite(m_pin, RaspberryPi::GpioLow);

    OnPauseButtonSim::slt_SimulateUnpress();
}
