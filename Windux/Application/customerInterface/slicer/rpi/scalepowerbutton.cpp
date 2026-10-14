#include "scalepowerbutton.h"
#include <unistd.h>
#include <QSettings>

ScalePowerButton::ScalePowerButton(RaspberryPi::pin_t pin) :
    m_pin(pin)
{
    pinMode(m_pin, OUTPUT);
    digitalWrite(m_pin, RaspberryPi::GpioLow);
}

void ScalePowerButton::On()
{
    QSettings settings;
    digitalWrite(m_pin, RaspberryPi::GpioHigh);
    usleep(settings.value("ScalePowerButtonUs", 200000).toInt());
    digitalWrite(m_pin, RaspberryPi::GpioLow);
}
