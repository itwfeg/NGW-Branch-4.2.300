/**
 * @file openauth.cpp
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#include "openauth.h"

#include <QMessageAuthenticationCode>
#include <QDateTime>
#include <QMap>

const char *OpenAuth::alphabet_ = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

const unsigned int OpenAuth::powTable_[] = {
    1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000
};

///////////////////////////////////////////////////////////////////////////////////////////////////
OpenAuth::OpenAuth( const QString &key, unsigned int period, QCryptographicHash::Algorithm algorithm ) :
    codeLength_( DEFAULT_CODE_LENGTH ),
    period_( period ),
    algorithm_( algorithm )
{
    initialize();
    decode( key.toUpper(), key_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
OpenAuth::OpenAuth( const QByteArray &key, unsigned int period, QCryptographicHash::Algorithm algorithm ) :
    codeLength_( DEFAULT_CODE_LENGTH ),
    key_( key ),
    period_( period ),
    algorithm_( algorithm )
{
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
OpenAuth::OpenAuth( const _Myt &other ) :
    codeLength_( other.codeLength_ ),
    key_( other.key_ ),
    period_( other.period_ ),
    algorithm_( other.algorithm_ )
{
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
OpenAuth::~OpenAuth()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
OpenAuth & OpenAuth::operator = ( const _Myt &rhs )
{
    codeLength_ = rhs.codeLength_;

    key_ = rhs.key_;
    period_ = rhs.period_;
    algorithm_ = rhs.algorithm_;

    return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString OpenAuth::generateCode() const
{
    quint64 now( QDateTime::currentMSecsSinceEpoch() / 1000 );
    now /= period_;

    return generateCode( messageArray( now ), codeLength() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool OpenAuth::validateCode( const QString &code, int delta ) const
{
    quint64 now( QDateTime::currentMSecsSinceEpoch() / 1000 );
    now /= period_;

    for ( int d( -delta ); d <= delta; ++d )
        if ( code == generateCode( messageArray( now + d ), codeLength() ) )
            return true;

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool OpenAuth::decode( const QString &s, QByteArray &a ) const
{
    if ( 0 != (s.length() % 8) )
        return false;

    int i( 0 );
    quint64 b( 0 );

    while ( i < s.length() )
    {
        for ( unsigned int j( 8 ); j--; )
        {
            b <<= 5;
            b |= decodeTable_[s[i++]];
        }

        a.append( (b >> 32) & 0xff );
        a.append( (b >> 24) & 0xff );
        a.append( (b >> 16) & 0xff );
        a.append( (b >> 8) & 0xff );
        a.append( b  & 0xff );
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString OpenAuth::generateCode( const QByteArray &message, int length ) const
{
    QMessageAuthenticationCode hmac( algorithm_, key_ );
    hmac.addData( message );

    const QByteArray hash( hmac.result() );

    unsigned int offset( hash[hash.length()-1] & 0x0f );

    unsigned int result( 0 );
    result |= (unsigned char) (hash[offset++] & 0x7f);
    result <<= 8;
    result |= (unsigned char) hash[offset++];
    result <<= 8;
    result |= (unsigned char) hash[offset++];
    result <<= 8;
    result |= (unsigned char) hash[offset++];

    QString code( QString::number( result % powTable_[length] ) );

    while ( code.length() < length )
        code.insert( 0, '0' );

    return code;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OpenAuth::initialize()
{
    for ( size_t i( strlen(alphabet_) ); i--; )
    {
        encodeTable_.insert( i, alphabet_[i] );
        decodeTable_.insert( alphabet_[i], i );
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
QByteArray OpenAuth::messageArray( quint64 message )
{
    int i;

    QByteArray a;
    a.resize( i = sizeof(message) );

    while ( i-- )
    {
        a[i] = message & 0xff;
        message >>= 8;
    }

    return a;
}

