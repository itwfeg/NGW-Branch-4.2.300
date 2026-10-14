#include "led.h"
#include <QSettings>

LED::LED(RaspberryPi::pin_t redPin, RaspberryPi::pin_t bluePin, RaspberryPi::pin_t greenPin) :
    m_redPin(redPin),
    m_bluePin(bluePin),
    m_greenPin(greenPin),
    m_redOn(false),
    m_blueOn(false),
    m_greenOn(false)
{
    pinMode(m_redPin, OUTPUT);
    pinMode(m_bluePin, OUTPUT);
    pinMode(m_greenPin, OUTPUT);
    slt_Off();

    QSettings settings;
    m_flashTimer.setInterval(settings.value("LedFlashMs", 250).toInt());
    m_flashTimer.setSingleShot(false);
    (void)connect(&m_flashTimer, SIGNAL(timeout()), this, SLOT(slt_FlashToggle()));
}

void LED::SetColor(bool redOn, bool blueOn, bool greenOn)
{
    m_redOn = redOn;
    m_blueOn = blueOn;
    m_greenOn = greenOn;
    Refresh();
}

void LED::slt_Off()
{
    SetColor(false, false, false);
    m_flashTimer.stop();
}

void LED::slt_Red()
{
    SetColor(true, false, false);
    m_flashTimer.stop();
}

void LED::slt_FlashRed()
{
    SetColor(true, false, false);
    m_flashTimer.start();
}

void LED::slt_Blue()
{
    SetColor(false, false, true);
    m_flashTimer.stop();
}

void LED::slt_Green()
{
    SetColor(false, true, false);
    m_flashTimer.stop();
}

void LED::slt_White()
{
    SetColor(true, true, true);
    m_flashTimer.stop();
}

void LED::slt_FlashToggle()
{
    if ((RaspberryPi::gpioValue_e)digitalRead(m_redPin) == RaspberryPi::GpioHigh &&
        (RaspberryPi::gpioValue_e)digitalRead(m_bluePin) == RaspberryPi::GpioHigh &&
        (RaspberryPi::gpioValue_e)digitalRead(m_greenPin) == RaspberryPi::GpioHigh)
    {
        Refresh();
    }
    else
    {
        Off();
    }
}

void LED::Refresh()
{
    digitalWrite(m_redPin, m_redOn ? RaspberryPi::GpioLow : RaspberryPi::GpioHigh);
    digitalWrite(m_bluePin, m_blueOn ? RaspberryPi::GpioLow : RaspberryPi::GpioHigh);
    digitalWrite(m_greenPin, m_greenOn ? RaspberryPi::GpioLow : RaspberryPi::GpioHigh);
}

void LED::Off()
{
    digitalWrite(m_redPin, RaspberryPi::GpioHigh);
    digitalWrite(m_bluePin, RaspberryPi::GpioHigh);
    digitalWrite(m_greenPin, RaspberryPi::GpioHigh);
}
