#include "switch.h"

void SwitchIsr();

static Switch* s_pSwitch = NULL;

Switch::Switch(RaspberryPi::pin_t pin) :
    m_inputGpio(pin, RaspberryPi::GpioPullUp)
{
    s_pSwitch = this;
    (void)connect(&m_inputGpio, SIGNAL(sig_Low()), this, SIGNAL(sig_Off()));
    (void)connect(&m_inputGpio, SIGNAL(sig_High()), this, SIGNAL(sig_On()));

    m_inputGpio.EnableIsr(RaspberryPi::GpioEdgeBoth, SwitchIsr);
}

Switch::state_e Switch::State () const
{
    return (m_inputGpio.Value() == RaspberryPi::GpioLow ? On : Off);
}

void SwitchIsr()
{
    s_pSwitch->m_inputGpio.Isr();
}
