#include "knifepower.h"
#include <QFile>

KNIFE::KNIFE(const QString& pin) :
    m_pin(pin),
    m_powerOn(false)
{
    slt_Off();
}

void KNIFE::SetPower(bool on)
{
    m_powerOn = on;
    Refresh();
}

void KNIFE::slt_On()
{
    SetPower(true);
}

void KNIFE::slt_Off()
{
    SetPower(false);
}

void KNIFE::Refresh()
{
    digitalWrite(m_pin, m_powerOn);
}

void KNIFE::Off()
{
    digitalWrite(m_pin, m_powerOn = false);
}

bool KNIFE::digitalRead( const QString& pin )
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

void KNIFE::digitalWrite( const QString& pin, bool value )
{
    QFile fd( pin );

    if ( fd.open( QIODevice::WriteOnly ) )
    {
        fd.write( value ? "1" : "0" );
        fd.close();
    }
}
