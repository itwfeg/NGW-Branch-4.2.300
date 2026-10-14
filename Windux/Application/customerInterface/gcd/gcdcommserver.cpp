/**
 * @file gcdcommserver.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "gcdcommserver.h"

#include <devicemsgsession.h>
#include <devicemsgweigher.h>
#include <gcdmsg.h>

#include <filesystem.h>

#include <QFile>
#include <QFileInfo>

#include <QDateTime>

///////////////////////////////////////////////////////////////////////////////////////////////////
gcdCommServer::gcdCommServer( unsigned int port ) :
#ifdef QT_DEBUG
    _Mybase( QHostAddress::Any, port ),
#else
    _Mybase( QHostAddress::LocalHost, port ),
#endif
    _DeviceMsgDispatcher(),
    _GcdMsgDispatcher(),
    versionMajor_( 0 ),
    versionMinor_( 0 )
{
    setObjectName( "gcdCommServer" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
gcdCommServer::~gcdCommServer()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcdCommServer::onUpdateStatus(
    unsigned int versionMajor,
    unsigned int versionMinor,
    const QString& language,
    const QString& weighMode )
{
    LOG_CLASS_INFO( "gcdCommServer", "onUpdateStatus" );

    LOG_INFO( "gcd version is %d.%d", versionMajor, versionMinor );

    versionMajor_ = versionMajor;
    versionMinor_ = versionMinor;

    LOG_DEBUG( "language '%s' weigh mode '%s'", qPrintable( language ), qPrintable( weighMode ) );

    language_ = language;
    weighMode_ = weighMode;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcdCommServer::onPlaylistError(
    const QString& file,
    const QString& errorDescription )
{
    LOG_CLASS_INFO( "gcdCommServer", "onPlaylistError" );

    LOG_WARN( "playlist error '%s' ('%s')", qPrintable( errorDescription ), qPrintable( file ) );

    gcdErrorMsg msg;
    msg.append( QString::number( 5 ) ); // error type
    msg.append( errorDescription ); // description
    msg.append( file ); // file

    write( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcdCommServer::read( const QByteArray& data, unsigned int& dataLen )
{
    return (( _DeviceMsgDispatcher::parse( data, dataLen ) ) ||
        ( _GcdMsgDispatcher::parse( data, dataLen ) ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcdCommServer::write( const msgResponse& m ) const
{
    return _Mybase::write( m.serialize() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcdCommServer::disconnect()
{
    LOG_CLASS_INFO( "gcdCommServer", "disconnect" );

    LOG_DEBUG( "client disconnect", NULL );

    // we are out of service until new connection...
    Q_EMIT controlMessage( gcdControlMessageType::OutOfService, "" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void gcdCommServer::handleMsgRequestT<weigherStatusDeviceMsg>( const weigherStatusDeviceMsg *m )
{
    LOG_CLASS_INFO( "gcdCommServer", "handleMsgRequestT" );

    if ( m->size() < 3 )
        LOG_ERROR( "badly formed weigher status message!", NULL );
    else
    {
        // ignore this message!
        // do not process because weight is already handled in the gcd data message and by
        // processing here also you will get artifacts on screen (especially for quantity plus)
        //Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::Weight, m->field<QString>( 2 ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcdCommServer::handleMsgRequest( const deviceMsg *m )
{
    LOG_CLASS_INFO( "gcdCommServer", "handleMsgRequest" );

    if ( !m )
        return;

    unsigned int event = m->event();

    LOG_INFO( "handle device %d event %d...", m->device(), event );

    switch ( m->device() )
    {
    //-------------------------------------------------------------------------
    // weigher messages
    //-------------------------------------------------------------------------
    case deviceMsgType::Weigher:

        if ( weigherDeviceMsgEventType::Status == event )
            handleMsgRequestT( dynamic_cast<const weigherStatusDeviceMsg*>( m ) );

        break;

    //-------------------------------------------------------------------------
    default:
        LOG_WARN( "unknown message!", NULL );
        break;

    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void gcdCommServer::handleMsgRequestT<gcdDataMsg>( const gcdDataMsg *m )
{
    LOG_CLASS_INFO( "gcdCommServer", "handleMsgRequestT" );

    QString name = "data message";
    LOG_DEBUG( "%s", qPrintable( name ) );

    if ( !m->size() )
    {
        LOG_ERROR( "%s without version!", qPrintable( name ) );
        return;
    }

    LOG_DEBUG( "version '%s'", qPrintable( m->at( 0 ) ) );

    // handle specific version
    if ( "1" == m->at( 0 ) )
    {
        // fields:
        // 0 version
        // 1 tare
        // 2 weight
        // 3 price
        // 4 total price
        // 5 zero annun
        // 6 net weight annun
        // 7 weight for annun
        // 8 pre pack annun
        // 9 fs discounted price
        // 10 fs discounted total price
        // 11 fs total savings
        // 12 plu
        // 13 plu description
        // 14 marquee
        // 15 plu type (1 random weight, 2 fixed weight, 3 by count, 4 fluid oz)
        // 16 dual range enabled
        // 17 dual range direction
        // 18 weigher attached
        // 19 fractional pricing annun
        // 20 product advert type
        // 21 product advert data
        // 22 unit price annun
        // 23 message terminator character '-'

        if ( m->size() < 20 )
            LOG_ERROR( "badly formed %s!", qPrintable( name ) );
        else
        {
            bool weigherAttached = !m->at( 18 ).isEmpty();
            int pluType =  m->at( 15 ).toInt();

            Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::Tare, m->at( 1 ) );

            // weight
            QString weight =  m->at( 2 );

            // FT-589: added case for by count item
            // FT-621: added case for fixed weight item
            // Note:
            // For some unknown reason, the scale application sets the unit price to "-1"
            // before the count is entered if "force count" is enabled. Discussed with
            // Larry Pevoar and he said it's been that way for awhile.
            if ( "-1" == weight )
                if (( 1 == pluType ) || ( 2 == pluType ) || ( 3 == pluType ))
                    weight = "";

            // When no weigher and no product make sure weight is empty.
            if (( !weigherAttached ) && ( 0 == pluType ))
                weight = "";

            Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::Weight, weight );

            Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::UnitPrice, m->at( 3 ) );
            Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::TotalPrice, m->at( 4 ) );

            Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::ZeroAnnuc, ("CZ" == m->at( 5 ) ? m->at( 5 ) : "") );
            Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::PrePackAnnuc, ("PP" == m->at( 8 ) ? m->at( 8 ) : "") );

            Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::WeigherAttached, m->at( 18 ) );
            Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::DualRangeIndicator, ("DR" == m->at( 16 ) ? m->at( 17 ) : "") );

            // do not bother updating labels for a scale that is unstable
            if ( weight.length() )
            {
                // weight type
                QString weightType = "Weight";

                // Note:
                // HLX v1.5 can be configured to display weight for a byCount item if the
                // byCount = 1. Finding a '.' will determine if a weight is being displayed or not.
                if (( "none" == m->at( 12 ) ) ||
                    ( 1 == pluType ) ||
                    ( weight.contains( "." ) ) ||
                    ( "----" == weight ))
                {
                    if ( "MAN" == m->at( 6 ) )
                        weightType = "ManualWeight";
                    else if ( "NT" == m->at( 6 ) )
                        weightType = "NetWeight";
                }
                else
                {
                    weightType = "Quantity";
                }

                Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::WeightType, weightType );

                // unit price type
                QString unitPriceType = "Weight";

                if ( "none" != m->at( 12 ) )
                {
                    // each
                    if (( 3 == pluType ) || ( 4 == pluType ))
                    {
                        unitPriceType = "Each";

                        if (( 23 < m->size() ) && ( "up" == m->at( 22 ) ))
                            if ( m->at( 3 ).length() )
                                unitPriceType = "UnitPrice";
                    }
                    // weight for
                    else if (( m->at( 7 ).length() ) && ( "-" != m->at( 7 ) ))
                        unitPriceType = "_p" + m->at( 7 );
                    // fractional
                    else if (( m->at( 19 ).length() ) && ( "-" != m->at( 19 ) ))
                        unitPriceType = "_s" + m->at( 19 );
                }

                Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::UnitPriceType, unitPriceType );
            }

            // no product and no weigher
            else if (( 0 == pluType ) && ( !weigherAttached ))
            {
                Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::WeightType, "Weight" );
                Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::UnitPriceType, "Weight" );
            }

            Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::FreqShopUnitPrice, m->at( 9 ) );
            Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::FreqShopTotalPrice, m->at( 10 ) );
            Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::FreqShopSavings, m->at( 11 ) );

            Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::PluNumber, m->at( 12 ) );
            Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::Description, m->at( 13 ) );

            // marquee
            if ( pluType )
                Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::Marquee, m->at( 14 ) );
				
			// when plu type absent we have store marquee
            else
            {
                if ( m->at( 14 ).length() )
                    storeMarquee_ = m->at( 14 );

                // restore store marquee
                Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::Marquee, storeMarquee_ );
            }
        }

        if ( 22 < m->size() )
        {
            // product advert
            int advertType( m->at( 20 ).toInt() );
            QString advertLocation( m->at( 21 ) );

            if (( 1 == advertType ) && ( advertLocation.length() ))
                Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::ProductAdvertImage, advertLocation );
            else if (( 2 == advertType ) && ( advertLocation.length() ))
                Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::ProductAdvertText, advertLocation );
            else if (( 3 == advertType ) && ( advertLocation.length() ))
                Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::ProductAdvertPlaylist, advertLocation );
            else
                Q_EMIT updateUserInterface( gcdUpdateUserInterfaceType::ProductAdvertNone, "" );
        }
    }
    else
    {
        LOG_ERROR( "unhandled %s version!", qPrintable( name ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void gcdCommServer::handleMsgRequestT<gcdLoadDisplayObjectXMLMsg>( const gcdLoadDisplayObjectXMLMsg *m )
{
    LOG_CLASS_INFO( "gcdCommServer", "handleMsgRequestT" );

    QString name = "load display object xml message";
    LOG_DEBUG( "%s", qPrintable( name ) );

    if ( !m->size() )
    {
        LOG_ERROR( "%s without version!", qPrintable( name ) );
        return;
    }

    LOG_DEBUG( "version '%s'", qPrintable( m->at( 0 ) ) );

    // handle specific version
    if ( "1" == m->at( 0 ) )
    {
        if ( m->size() < 2 )
            LOG_ERROR( "badly formed %s!", qPrintable( name ) );
        else
        {
            // get filename
            QString filename( filesystem::absoluteFilePath( stripPreviousVersionQualifiers( m->at( 1 ) ), folderType::Layouts ) );

            // set layout
            Q_EMIT controlMessage( gcdControlMessageType::SetLayout, filename );

            // send response
            gcdLoadDisplayObjectXMLResponseMsg msg;
            write( msg );
        }
    }
    else if ( "2" == m->at( 0 ) )
    {
        if ( m->size() < 4 )
            LOG_ERROR( "badly formed %s!", qPrintable( name ) );
        else
        {
            // get message details
            QString filename( filesystem::absoluteFilePath( stripPreviousVersionQualifiers( m->at( 1 ) ), folderType::Layouts ) );
            QString idleAdvertSetting( m->at( 2 ) );
            QString website( m->at( 3 ) );

            // set layout
            Q_EMIT controlMessage( gcdControlMessageType::SetLayout, filename );

            // advert settings
            // 0 = use playlist
            // 1 = use website
            Q_EMIT controlMessage( gcdControlMessageType::SetLayoutAdvertSetting, idleAdvertSetting );

            // website
            if ( "1" == idleAdvertSetting )
            {
                LOG_INFO( "idle website is '%s'", qPrintable( website ) );

                Q_EMIT controlMessage( gcdControlMessageType::Website, website );
            }

            // send response
            gcdLoadDisplayObjectXMLResponseMsg msg;
            write( msg );
        }
    }
    else
    {
        LOG_ERROR( "unhandled %s version!", qPrintable( name ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void gcdCommServer::handleMsgRequestT<gcdTestColorMsg>( const gcdTestColorMsg *m )
{
    LOG_CLASS_INFO( "gcdCommServer", "handleMsgRequestT" );

    QString name = "test color message";
    LOG_DEBUG( "%s", qPrintable( name ) );

    if ( !m->size() )
    {
        LOG_ERROR( "%s without version!", qPrintable( name ) );
        return;
    }

    LOG_DEBUG( "version '%s'", qPrintable( m->at( 0 ) ) );

    // handle specific version
    if ( "1" == m->at( 0 ) )
    {
        if ( m->size() < 2 )
            LOG_ERROR( "badly formed %s!", qPrintable( name ) );
        else
        {
            // '0' means hide test color
            // '1' means show test color bar pattern
            // '2' means show test color white screen
            if ( "0" == m->at( 1 ) )
                Q_EMIT controlMessage( gcdControlMessageType::HideTestColor, "" );
            else if ( "1" == m->at( 1 ) )
                Q_EMIT controlMessage( gcdControlMessageType::ShowTestColor, "1" );
            else if ( "2" == m->at( 1 ) )
                Q_EMIT controlMessage( gcdControlMessageType::ShowTestColor, "2" );
            else if ( "4" == m->at( 1 ) )
                Q_EMIT controlMessage( gcdControlMessageType::ShowTestColor, "4" );
            else if ( "6" == m->at( 1 ) )
                Q_EMIT controlMessage( gcdControlMessageType::ShowTestColor, "6" );
            else if ( "7" == m->at( 1 ) )
                Q_EMIT controlMessage( gcdControlMessageType::ShowTestColor, "7" );
            else
                LOG_WARN( "unknown command %s!", qPrintable( m->at( 1 ) ) );
        }
    }
    else if ( "2" == m->at( 0 ) )
    {
        if ( m->size() < 3 )
            LOG_ERROR( "badly formed %s!", qPrintable( name ) );
        else
        {
            // '0' means hide test color
            // '1' means show test color bar pattern
            // '2' means show test color white screen
            // '3' means show test image on screen
            if ( "0" == m->at( 1 ) )
                Q_EMIT controlMessage( gcdControlMessageType::HideTestColor, "" );
            else if ( "1" == m->at( 1 ) )
                Q_EMIT controlMessage( gcdControlMessageType::ShowTestColor, "1" );
            else if ( "2" == m->at( 1 ) )
                Q_EMIT controlMessage( gcdControlMessageType::ShowTestColor, "2" );
            else if ( "3" == m->at( 1 ) )
                Q_EMIT controlMessage( gcdControlMessageType::ShowTestColor, m->at( 2 ) );
            else if ( "4" == m->at( 1 ) )
                Q_EMIT controlMessage( gcdControlMessageType::ShowTestColor, "4" );
            else if ( "6" == m->at( 1 ) )
                Q_EMIT controlMessage( gcdControlMessageType::ShowTestColor, "6" );
            else if ( "7" == m->at( 1 ) )
                Q_EMIT controlMessage( gcdControlMessageType::ShowTestColor, "7" );
            else
                LOG_WARN( "unknown command %s!", qPrintable( m->at( 1 ) ) );
        }
    }
    else
    {
        LOG_ERROR( "unhandled %s version!", qPrintable( name ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void gcdCommServer::handleMsgRequestT<gcdShutdownDeviceMsg>( const gcdShutdownDeviceMsg *m )
{
    Q_UNUSED( m );

    LOG_CLASS_INFO( "gcdCommServer", "handleMsgRequestT" );

    QString name = "shutdown device message";
    LOG_DEBUG( "%s", qPrintable( name ) );

    // nothing to do
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void gcdCommServer::handleMsgRequestT<gcdLanguageAndWeighModeMsg>( const gcdLanguageAndWeighModeMsg *m )
{
    LOG_CLASS_INFO( "gcdCommServer", "handleMsgRequestT" );

    QString name = "language and weigh mode message";
    LOG_DEBUG( "%s", qPrintable( name ) );

    if ( !m->size() )
    {
        LOG_ERROR( "%s without version!", qPrintable( name ) );
        return;
    }

    LOG_DEBUG( "version '%s'", qPrintable( m->at( 0 ) ) );

    // handle specific version
    if (( !m->at( 0 ).length() ) ||         // empty version
        ( "1" == m->at( 0 ) ))              // version 1
    {
        for ( unsigned int i = 1; i < (unsigned int) m->size(); ++i )
        {
            QString value = m->at( i );

            if (( "En" == value ) || ( "Fr" == value ) || ( "Sp" == value ))
                Q_EMIT controlMessage( gcdControlMessageType::SetLanguage, value );
            else if (( "Lb" == value ) || ( "Kg" == value ))
                Q_EMIT controlMessage( gcdControlMessageType::SetWeighMode, value );
        }
    }
    else
    {
        LOG_ERROR( "unhandled %s version!", qPrintable( name ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void gcdCommServer::handleMsgRequestT<gcdSetVolumeMsg>( const gcdSetVolumeMsg *m )
{
    LOG_CLASS_INFO( "gcdCommServer", "handleMsgRequestT" );

    QString name = "set volume message";
    LOG_DEBUG( "%s", qPrintable( name ) );

    if ( !m->size() )
    {
        LOG_ERROR( "%s without version!", qPrintable( name ) );
        return;
    }

    LOG_DEBUG( "version '%s'", qPrintable( m->at( 0 ) ) );

    // handle specific version
    if ( "1" == m->at( 0 ) )
    {
        if ( m->size() < 2 )
            LOG_ERROR( "badly formed %s!", qPrintable( name ) );
        else
        {
            QString value = "1.0";

            int volume = m->at( 1 ).toInt();

            // second field contains volume
            // convert it from range [0,5] to percentage
            if ( volume <= 0 )
                value = "0.0";
            else if ( 1 == volume )
                value = "0.2";
            else if ( 2 == volume )
                value = "0.4";
            else if ( 3 == volume )
                value = "0.6";
            else if ( 4 == volume )
                value = "0.8";

            // handle message
            Q_EMIT controlMessage( gcdControlMessageType::SetVolume, value );
        }
    }
    else
    {
        LOG_ERROR( "unhandled %s version!", qPrintable( name ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void gcdCommServer::handleMsgRequestT<gcdHeartbeatMsg>( const gcdHeartbeatMsg *m )
{
    Q_UNUSED( m );

    LOG_CLASS_INFO( "gcdCommServer", "handleMsgRequestT" );

    QString name = "heartbeat message";
    LOG_DEBUG( "%s", qPrintable( name ) );

    // send heartbeat response
    gcdHeartbeatResponseMsg msg;
    write( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void gcdCommServer::handleMsgRequestT<gcdGetStatusMsg>( const gcdGetStatusMsg *m )
{
    Q_UNUSED( m );

    LOG_CLASS_INFO( "gcdCommServer", "handleMsgRequestT" );

    QString name = "get status message";
    LOG_DEBUG( "%s", qPrintable( name ) );

    // Note:
    // The old gcd was a standalone freescale board running an app within windows ce. The
    // new gcd is just a standalone app running within the same host. Due to this a lot of
    // these fields dont really apply, so we will send 0...

    // send status message
    gcdStatusMsg msg;

    // board major, minor
    msg.append( "0" );
    msg.append( "0" );

    // wince major, minor
    msg.append( "0" );
    msg.append( "0" );

    // gcd dll major, minor
    msg.append( "0" );
    msg.append( "0" );

    // gcd app major, minor
    msg.append( QString( "%1" ).arg( versionMajor_ ) );
    msg.append( QString( "%1" ).arg( versionMinor_ ) );

    // language, weigh mode
    msg.append( language_ );
    msg.append( weighMode_ );

    // memory test
    msg.append( "MemTstPass" );

    write( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void gcdCommServer::handleMsgRequestT<gcdGetFileInfoMsg>( const gcdGetFileInfoMsg *m )
{
    LOG_CLASS_INFO( "gcdCommServer", "handleMsgRequestT" );

    QString name = "get file info message";
    LOG_DEBUG( "%s", qPrintable( name ) );

    if ( !m->size() )
    {
        LOG_ERROR( "badly formed %s!", qPrintable( name ) );
        return;
    }

    QString filename = m->at( 0 );

    // send file info message
    gcdFileInfoMsg msg;

    // filename
    msg.append( filename );

    // check for existance
    if ( !QFile::exists( filename ) )
    {
        // size, last write time
        msg.append( "0" );
        msg.append( "0" );
    }
    else
    {
        QFileInfo info( filename );

        // size, last write time
        msg.append( QString( "%1" ).arg( info.size() ) );
        msg.append( QString( "%1" ).arg( info.lastModified().toMSecsSinceEpoch() ) );
    }

    write( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void gcdCommServer::handleMsgRequestT<gcdFileUploadMsg>( const gcdFileUploadMsg *m )
{
    Q_UNUSED( m );

    LOG_CLASS_INFO( "gcdCommServer", "handleMsgRequestT" );

    QString name = "file upload message";
    LOG_DEBUG( "%s", qPrintable( name ) );

    // nothing to do
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void gcdCommServer::handleMsgRequestT<gcdLoadPlaylistMsg>( const gcdLoadPlaylistMsg *m )
{
    LOG_CLASS_INFO( "gcdCommServer", "handleMsgRequestT" );

    QString name = "load playlist message";
    LOG_DEBUG( "%s", qPrintable( name ) );

    if ( !m->size() )
    {
        LOG_ERROR( "%s without version!", qPrintable( name ) );
        return;
    }

    LOG_DEBUG( "version '%s'", qPrintable( m->at( 0 ) ) );

    // handle specific version
    if ( "1" == m->at( 0 ) )
    {
        if ( 2 != m->size() )
            LOG_ERROR( "badly formed %s!", qPrintable( name ) );
        else
        {
            // get filename
            QString filename( filesystem::absoluteFilePath( stripPreviousVersionQualifiers( m->at( 1 ) ), folderType::Playlists ) );

            // handle message
            Q_EMIT controlMessage( gcdControlMessageType::SetPlaylist, filename );

            // send response
            gcdLoadPlaylistResponseMsg msg;
            write( msg );
        }
    }
    else
    {
        LOG_ERROR( "unhandled %s version!", qPrintable( name ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void gcdCommServer::handleMsgRequestT<gcdStartFirmwareUpdateMsg>( const gcdStartFirmwareUpdateMsg *m )
{
    Q_UNUSED( m );

    LOG_CLASS_INFO( "gcdCommServer", "handleMsgRequestT" );

    QString name = "start firmware update message";
    LOG_DEBUG( "%s", qPrintable( name ) );

    // nothing to do
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void gcdCommServer::handleMsgRequestT<gcdStartPlaylistUpdateMsg>( const gcdStartPlaylistUpdateMsg *m )
{
    Q_UNUSED( m );

    LOG_CLASS_INFO( "gcdCommServer", "handleMsgRequestT" );

    QString name = "start playlist update message";
    LOG_DEBUG( "%s", qPrintable( name ) );

    // nothing to do
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void gcdCommServer::handleMsgRequestT<gcdSetPlaybackWaitTimeMsg>( const gcdSetPlaybackWaitTimeMsg *m )
{
    LOG_CLASS_INFO( "gcdCommServer", "handleMsgRequestT" );

    QString name = "set playback wait time message";
    LOG_DEBUG( "%s", qPrintable( name ) );

    if ( !m->size() )
    {
        LOG_ERROR( "%s without version!", qPrintable( name ) );
        return;
    }

    LOG_DEBUG( "version '%s'", qPrintable( m->at( 0 ) ) );

    // handle specific version
    if ( "1" == m->at( 0 ) )
    {
        if ( 2 != m->size() )
            LOG_ERROR( "badly formed %s!", qPrintable( name ) );
        else
        {
            QString value = m->at( 1 );

            // second field contains time
            // convert it from seconds to milliseconds
            value += "000";

            // handle message
            Q_EMIT controlMessage( gcdControlMessageType::SetPlaybackWaitTime, value );
        }
    }
    else
    {
        LOG_ERROR( "unhandled %s version!", qPrintable( name ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void gcdCommServer::handleMsgRequestT<gcdSetTimeInfoMsg>( const gcdSetTimeInfoMsg *m )
{
    Q_UNUSED( m );

    LOG_CLASS_INFO( "gcdCommServer", "handleMsgRequestT" );

    QString name = "set time info message";
    LOG_DEBUG( "%s", qPrintable( name ) );

    // nothing to do
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void gcdCommServer::handleMsgRequestT<gcdSetUnitSeparatorMsg>( const gcdSetUnitSeparatorMsg *m )
{
    Q_UNUSED( m );

    LOG_CLASS_INFO( "gcdCommServer", "handleMsgRequestT" );

    QString name = "set unit separator message";
    LOG_DEBUG( "%s", qPrintable( name ) );

    if ( !m->size() )
    {
        LOG_ERROR( "%s without version!", qPrintable( name ) );
        return;
    }

    LOG_DEBUG( "version '%s'", qPrintable( m->at( 0 ) ) );

    // handle specific version
    if ( "1" == m->at( 0 ) )
    {
        if ( 2 != m->size() )
            LOG_ERROR( "badly formed %s!", qPrintable( name ) );
        else
        {
            // handle message
            Q_EMIT controlMessage( gcdControlMessageType::SetUnitSeparator, m->at( 1 ) );
        }
    }
    else
    {
        LOG_ERROR( "unhandled %s version!", qPrintable( name ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void gcdCommServer::handleMsgRequestT<gcdSetCurrencySymbolMsg>( const gcdSetCurrencySymbolMsg *m )
{
    Q_UNUSED( m );

    LOG_CLASS_INFO( "gcdCommServer", "handleMsgRequestT" );

    QString name = "set currency symbol message";
    LOG_DEBUG( "%s", qPrintable( name ) );

    if ( !m->size() )
    {
        LOG_ERROR( "%s without version!", qPrintable( name ) );
        return;
    }

    LOG_DEBUG( "version '%s'", qPrintable( m->at( 0 ) ) );

    // handle specific version
    if ( "1" == m->at( 0 ) )
    {
        if ( 2 != m->size() )
            LOG_ERROR( "badly formed %s!", qPrintable( name ) );
        else
        {
            // handle message
            Q_EMIT controlMessage( gcdControlMessageType::SetCurrencySymbol, m->at( 1 ) );
        }
    }
    else
    {
        LOG_ERROR( "unhandled %s version!", qPrintable( name ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void gcdCommServer::handleMsgRequestT<gcdNowServingCustomerNumberMsg>( const gcdNowServingCustomerNumberMsg *m )
{
    Q_UNUSED( m );

    LOG_CLASS_INFO( "gcdCommServer", "handleMsgRequestT" );

    QString name = "now serving customer number message";
    LOG_DEBUG( "%s", qPrintable( name ) );

    if ( !m->size() )
    {
        LOG_ERROR( "%s without version!", qPrintable( name ) );
        return;
    }

    LOG_DEBUG( "version '%s'", qPrintable( m->at( 0 ) ) );

    // handle specific version
    if ( "1" == m->at( 0 ) )
    {
        if ( 2 != m->size() )
            LOG_ERROR( "badly formed %s!", qPrintable( name ) );
        else
        {
            // handle message
            Q_EMIT controlMessage( gcdControlMessageType::NowServingCustomerNumber, m->at( 1 ) );
        }
    }
    else
    {
        LOG_ERROR( "unhandled %s version!", qPrintable( name ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void gcdCommServer::handleMsgRequestT<gcdDismissNowServingCustomerNumberMsg>( const gcdDismissNowServingCustomerNumberMsg *m )
{
    Q_UNUSED( m );

    LOG_CLASS_INFO( "gcdCommServer", "handleMsgRequestT" );

    QString name = "dismiss now serving customer number message";
    LOG_DEBUG( "%s", qPrintable( name ) );

    if ( !m->size() )
    {
        LOG_ERROR( "%s without version!", qPrintable( name ) );
        return;
    }

    LOG_DEBUG( "version '%s'", qPrintable( m->at( 0 ) ) );

    // handle specific version
    if ( "1" == m->at( 0 ) )
    {
        if ( 2 != m->size() )
            LOG_ERROR( "badly formed %s!", qPrintable( name ) );
        else
        {
            // handle message
            Q_EMIT controlMessage( gcdControlMessageType::DismissOverlay, QString() );
        }
    }
    else
    {
        LOG_ERROR( "unhandled %s version!", qPrintable( name ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcdCommServer::handleMsgRequest( const gcdMsg *m )
{
    LOG_CLASS_INFO( "gcdCommServer", "handleMsgRequest" );

    if ( !m )
        return;

    LOG_INFO( "handle type %d...", m->type() );

    switch ( m->type() )
    {
    //-------------------------------------------------------------------------
    case gcdMsgType::Data:
        handleMsgRequestT( dynamic_cast<const gcdDataMsg*>( m ) );
        break;

    //-------------------------------------------------------------------------
    case gcdMsgType::LoadDisplayObjectXML:
        handleMsgRequestT( dynamic_cast<const gcdLoadDisplayObjectXMLMsg*>( m ) );
        break;

    //-------------------------------------------------------------------------
    case gcdMsgType::TestColor:
        handleMsgRequestT( dynamic_cast<const gcdTestColorMsg*>( m ) );
        break;

    //-------------------------------------------------------------------------
    case gcdMsgType::ShutdownDevice:
        handleMsgRequestT( dynamic_cast<const gcdShutdownDeviceMsg*>( m ) );
        break;

    //-------------------------------------------------------------------------
    case gcdMsgType::LanguageAndWeighMode:
        handleMsgRequestT( dynamic_cast<const gcdLanguageAndWeighModeMsg*>( m ) );
        break;

    //-------------------------------------------------------------------------
    case gcdMsgType::SetVolume:
        handleMsgRequestT( dynamic_cast<const gcdSetVolumeMsg*>( m ) );
        break;

    //-------------------------------------------------------------------------
    case gcdMsgType::Heartbeat:
        handleMsgRequestT( dynamic_cast<const gcdHeartbeatMsg*>( m ) );
        break;

    //-------------------------------------------------------------------------
    case gcdMsgType::GetStatus:
        handleMsgRequestT( dynamic_cast<const gcdGetStatusMsg*>( m ) );
        break;

    //-------------------------------------------------------------------------
    case gcdMsgType::GetFileInfo:
        handleMsgRequestT( dynamic_cast<const gcdGetFileInfoMsg*>( m ) );
        break;

    //-------------------------------------------------------------------------
    case gcdMsgType::FileUpload:
        handleMsgRequestT( dynamic_cast<const gcdFileUploadMsg*>( m ) );
        break;

    //-------------------------------------------------------------------------
    case gcdMsgType::LoadPlaylist:
        handleMsgRequestT( dynamic_cast<const gcdLoadPlaylistMsg*>( m ) );
        break;

    //-------------------------------------------------------------------------
    case gcdMsgType::StartFirmwareUpdate:
        handleMsgRequestT( dynamic_cast<const gcdStartFirmwareUpdateMsg*>( m ) );
        break;

    //-------------------------------------------------------------------------
    case gcdMsgType::StartPlaylistUpdate:
        handleMsgRequestT( dynamic_cast<const gcdStartPlaylistUpdateMsg*>( m ) );
        break;

    //-------------------------------------------------------------------------
    case gcdMsgType::SetPlaybackWaitTime:
        handleMsgRequestT( dynamic_cast<const gcdSetPlaybackWaitTimeMsg*>( m ) );
        break;

    //-------------------------------------------------------------------------
    case gcdMsgType::SetTimeInfo:
        handleMsgRequestT( dynamic_cast<const gcdSetTimeInfoMsg*>( m ) );
        break;

    //-------------------------------------------------------------------------
    case gcdMsgType::SetUnitSeparator:
        handleMsgRequestT( dynamic_cast<const gcdSetUnitSeparatorMsg*>( m ) );
        break;

    //-------------------------------------------------------------------------
    case gcdMsgType::SetCurrencySymbol:
        handleMsgRequestT( dynamic_cast<const gcdSetCurrencySymbolMsg*>( m ) );
        break;

    //-------------------------------------------------------------------------
    case gcdMsgType::NowServingCustomerNumber:
        handleMsgRequestT( dynamic_cast<const gcdNowServingCustomerNumberMsg*>( m ) );
        break;

    //-------------------------------------------------------------------------
    case gcdMsgType::DismissNowServingCustomerNumber:
        handleMsgRequestT( dynamic_cast<const gcdDismissNowServingCustomerNumberMsg*>( m ) );
        break;

    //-------------------------------------------------------------------------
    default:
        LOG_WARN( "unknown message!", NULL );
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString gcdCommServer::stripPreviousVersionQualifiers( const QString& filename )
{
    QString filenameLower = filename.toLower();

    if ( filenameLower.contains( "/sdmemory/" ) )
        return filename.right( filename.length() - filename.lastIndexOf( "/" ) - 1 );
    else if ( filenameLower.contains( "\\sdmemory\\" ) )
        return filename.right( filename.length() - filename.lastIndexOf( "\\" ) - 1 );

    return filename;
}
