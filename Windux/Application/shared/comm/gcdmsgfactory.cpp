/**
 * @file gcdmsgfactory.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "gcdmsgfactory.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
msgRequest *gcdMsgFactory::allocMsgRequest( const QByteArray& data, unsigned int& dataLen )
{
    msgRequest *m = NULL;

    gcdMsgType::type type;

    if ( gcdMsg::valid( data, type ) )
    {
        // create message object
        if ( gcdDataMsg::typeOf( type ) )
            m = new gcdDataMsg;
        else if ( gcdLoadDisplayObjectXMLMsg::typeOf( type ) )
            m = new gcdLoadDisplayObjectXMLMsg;
        else if ( gcdTestColorMsg::typeOf( type ) )
            m = new gcdTestColorMsg;
        else if ( gcdShutdownDeviceMsg::typeOf( type ) )
            m = new gcdShutdownDeviceMsg;
        else if ( gcdLanguageAndWeighModeMsg::typeOf( type ) )
            m = new gcdLanguageAndWeighModeMsg;
        else if ( gcdSetVolumeMsg::typeOf( type ) )
            m = new gcdSetVolumeMsg;
        else if ( gcdHeartbeatMsg::typeOf( type ) )
            m = new gcdHeartbeatMsg;
        else if ( gcdGetStatusMsg::typeOf( type ) )
            m = new gcdGetStatusMsg;
        else if ( gcdGetFileInfoMsg::typeOf( type ) )
            m = new gcdGetFileInfoMsg;
        else if ( gcdFileUploadMsg::typeOf( type ) )
            m = new gcdFileUploadMsg;
        else if ( gcdLoadPlaylistMsg::typeOf( type ) )
            m = new gcdLoadPlaylistMsg;
        else if ( gcdStartFirmwareUpdateMsg::typeOf( type ) )
            m = new gcdStartFirmwareUpdateMsg;
        else if ( gcdStartPlaylistUpdateMsg::typeOf( type ) )
            m = new gcdStartPlaylistUpdateMsg;
        else if ( gcdSetPlaybackWaitTimeMsg::typeOf( type ) )
            m = new gcdSetPlaybackWaitTimeMsg;
        else if ( gcdSetTimeInfoMsg::typeOf( type ) )
            m = new gcdSetTimeInfoMsg;
        else if ( gcdSetUnitSeparatorMsg::typeOf( type ) )
            m = new gcdSetUnitSeparatorMsg;
        else if ( gcdSetCurrencySymbolMsg::typeOf( type ) )
            m = new gcdSetCurrencySymbolMsg;
        else if ( gcdNowServingCustomerNumberMsg::typeOf( type ) )
            m = new gcdNowServingCustomerNumberMsg;
        else if ( gcdDismissNowServingCustomerNumberMsg::typeOf( type ) )
            m = new gcdDismissNowServingCustomerNumberMsg;

        // no object, so just return message length
        if ( !m )
            dataLen = gcdMsg::parseLength( data );

        // parse data into object
        else if ( !m->parse( data, dataLen ) )
        {
            // parse failed! cleanup...
            delete m;
            m = NULL;
        }
    }

    return m;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcdMsgFactory::freeMsgRequest( msgRequest *m )
{
    if ( m )
        delete m;
}
