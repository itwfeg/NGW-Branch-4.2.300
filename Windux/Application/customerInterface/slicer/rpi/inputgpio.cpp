#include "inputgpio.h"
#include <QSettings>

InputGpio::InputGpio(RaspberryPi::pin_t pin, RaspberryPi::gpioPullUpType_e pud) :
    m_pin(pin),
    m_value(RaspberryPi::GpioLow)
{
    QSettings settings;
    m_debounceTimeMs = settings.value("InputGpioDebounceMs", 10).toInt();

    pinMode(m_pin, INPUT);
    pullUpDnControl(m_pin, pud);
    m_value = (RaspberryPi::gpioValue_e)digitalRead(m_pin); //initialize
}

void InputGpio::EnableIsr(RaspberryPi::gpioEdgeType_e edgeType, pfIsrHandler isrHandler)
{
    //queue handling of interrupts
    connect(this, SIGNAL(sig_Isr()), this, SLOT(slt_Isr()), Qt::QueuedConnection);

    m_debounceTimer.setSingleShot(true);
    connect(&m_debounceTimer, SIGNAL(timeout()), this, SLOT(slt_DebounceTimerExpired()));

    if (isrHandler != NULL)
    {
        (void)wiringPiISR(m_pin, edgeType,  isrHandler);
    }
}

void InputGpio::Isr()
{
    emit sig_Isr();
}

void InputGpio::slt_DebounceTimerExpired()
{
    m_value = (m_value == RaspberryPi::GpioHigh ? RaspberryPi::GpioLow : RaspberryPi::GpioHigh);

    if (m_value == RaspberryPi::GpioLow)
    {
        emit sig_Low();
    }
    else
    {
        emit sig_High();
    }
}

void InputGpio::slt_Isr()
{
    RaspberryPi::gpioValue_e val = (RaspberryPi::gpioValue_e)digitalRead(m_pin);

    if ((val == RaspberryPi::GpioLow && m_value == RaspberryPi::GpioHigh) ||
        (val == RaspberryPi::GpioHigh && m_value == RaspberryPi::GpioLow))
    {
        m_debounceTimer.start(m_debounceTimeMs);
    }
    else
    {
        m_debounceTimer.stop();
    }
}
