#include "led.h"
#include <QFile>
#include <QSettings>

LED::LED(const QString& redPin, const QString& bluePin, const QString& greenPin) :
    m_redPin(redPin),
    m_bluePin(bluePin),
    m_greenPin(greenPin),
    m_redOn(false),
    m_blueOn(false),
    m_greenOn(false)
{
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
    if (digitalRead(m_redPin) &&
        digitalRead(m_bluePin) &&
        digitalRead(m_greenPin))
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
    digitalWrite(m_redPin, m_redOn);
    digitalWrite(m_bluePin, m_blueOn);
    digitalWrite(m_greenPin, m_greenOn);
}

void LED::Off()
{
    digitalWrite(m_redPin, m_redOn = false);
    digitalWrite(m_bluePin, m_blueOn = false);
    digitalWrite(m_greenPin, m_greenOn = false);
}

bool LED::digitalRead( const QString& pin )
{
    bool result( false );
    QFile fd( pin );

    if ( fd.open( QIODevice::ReadOnly ) )
    {
        char buffer[16];

        if ( 1 == fd.read( buffer, 1 ) )
            result = ( '1' == buffer[0] );

        fd.close();
    }

    return result;
}

void LED::digitalWrite( const QString& pin, bool value )
{
    QFile fd( pin );

    if ( fd.open( QIODevice::WriteOnly ) )
    {
        fd.write( value ? "1" : "0" );
        fd.close();
    }
}
