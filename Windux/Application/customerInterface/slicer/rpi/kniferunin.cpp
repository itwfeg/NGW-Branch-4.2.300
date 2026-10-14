#include "kniferunin.h"

void KnifeRunInIsr();

//the wiringPi API is limited, in that it does not support
//passing an argument to an "ISR"
//as a workaround, make this class essentially a singleton
static RaspPiKnifeRunIn* s_pKnifeRunIn = NULL;

RaspPiKnifeRunIn::RaspPiKnifeRunIn(RaspberryPi::pin_t pin) :
    m_inputGpio(pin, RaspberryPi::GpioPullNone)
{
    s_pKnifeRunIn = this;
    (void)connect(&m_inputGpio, SIGNAL(sig_High()), this, SIGNAL(sig_Asserted()));
    (void)connect(&m_inputGpio, SIGNAL(sig_Low()), this, SIGNAL(sig_DeAsserted()));
    m_inputGpio.EnableIsr(RaspberryPi::GpioEdgeBoth, KnifeRunInIsr);
}

KnifeRunIn::state_e RaspPiKnifeRunIn::State () const
{
    return (m_inputGpio.Value() == RaspberryPi::GpioLow ? DeAsserted : Asserted);
}

void KnifeRunInIsr()
{
    s_pKnifeRunIn->m_inputGpio.Isr();
}
