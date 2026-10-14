#include "button.h"
#include <QSettings>

void ButtonIsr();

//the wiringPi API is limited, in that it does not support
//passing an argument to an "ISR"
//as a workaround, make this class essentially a singleton
static Button* s_pButton = NULL;

Button::Button(RaspberryPi::pin_t pin) :
    m_inputGpio(pin, RaspberryPi::GpioPullUp)
{
    QSettings settings;
    m_clickTimer.setInterval(settings.value("ShortLongButtonClickMs", 3000).toInt());
    m_clickTimer.setSingleShot(true);
    s_pButton = this;
    (void)connect(&m_inputGpio, SIGNAL(sig_Low()), this, SIGNAL(sig_Pressed()));
    (void)connect(&m_inputGpio, SIGNAL(sig_High()), this, SIGNAL(sig_Unpressed()));
    (void)connect(&m_inputGpio, SIGNAL(sig_Low()), &m_clickTimer, SLOT(start()));
    (void)connect(&m_inputGpio, SIGNAL(sig_High()), this, SLOT(slt_Release()));
    (void)connect(&m_clickTimer, SIGNAL(timeout()), this, SIGNAL(sig_LongClick()));
    m_inputGpio.EnableIsr(RaspberryPi::GpioEdgeBoth, ButtonIsr);
}

Button::state_e Button::State () const
{
    return (m_inputGpio.Value() == RaspberryPi::GpioLow ? Pressed : Unpressed);
}

void Button::slt_Release()
{
    if (m_clickTimer.isActive())
    {
        emit sig_ShortClick();
        m_clickTimer.stop();
    }
}

void ButtonIsr()
{
    s_pButton->m_inputGpio.Isr();
}
