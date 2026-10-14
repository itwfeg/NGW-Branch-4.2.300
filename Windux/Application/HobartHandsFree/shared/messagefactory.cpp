/**
 * @file messagefactory.cpp
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#include "deviceclearallmessages.h"
#include "devicecmdresult.h"
#include "deviceconfig.h"
#include "deviceconnect.h"
#include "devicedisconnect.h"
#include "deviceerror.h"
#include "devicefirmwareprogress.h"
#include "devicefirmwarerequest.h"
#include "deviceflashfirmwareota.h"
#include "devicelist.h"
#include "devicelistreq.h"
#include "deviceopnamelist.h"
#include "devicereadallconfig.h"
#include "devicereadconfig.h"
#include "devicesendmessage.h"
#include "devicestate.h"
#include "devicewriteconfig.h"
#include "messagefactory.h"
#include "ping.h"
#include "pong.h"

#if WIN32
#include <WinSock.h>
#else
#include <arpa/inet.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
message *messageFactory::create( const char *s, size_t n, size_t *len )
{
    if ( n < 8 )
        return NULL;

    size_t messageLen( ntohl( reinterpret_cast<const uint32_t*>( s )[0] ) );
    size_t messageType( ntohl( reinterpret_cast<const uint32_t*>( s )[1] ) );

    if ( n < (messageLen+4) )
        return NULL;

    uint8_t messageVer( messageType & 0xFF );
    messageType >>= 8;

    // ---- //

    message *msg( NULL );

    // allocate message
    switch ( messageType )
    {
    case message::HFPD_DEV_CLEAR_ALL_MESSAGES:
        msg = new deviceClearAllMessages;
        break;
    case message::HFPD_DEV_CMD_RESULT:
        msg = new deviceCommandResult;
        break;
    case message::HFPD_DEV_CONFIG:
        msg = new deviceConfig;
        break;
    case message::HFPD_DEV_CONNECT:
        msg = new deviceConnect;
        break;
    case message::HFPD_DEV_DISCONNECT:
        msg = new deviceDisconnect;
        break;
    case message::HFPD_DEV_ERROR:
        msg = new deviceError;
        break;
    case message::HFPD_DEV_FIRMWARE_PROGRESS:
        msg = new deviceFirmwareProgress;
        break;
    case message::HFPD_DEV_FIRMWARE_REQUEST:
        msg = new deviceFirmwareRequest;
        break;
    case message::HFPD_DEV_FLASH_FIRMWARE_OTA:
        msg = new deviceFlashFirmwareOTA;
        break;
    case message::HFPD_DEV_LIST:
        msg = new deviceList;
        break;
    case message::HFPD_DEV_LIST_REQ:
        msg = new deviceListRequest;
        break;
	case message::HFPD_DEV_OPNAME_LIST:
		msg = new deviceOpNameList;
		break;
	case message::HFPD_DEV_READ_ALL_CONFIG:
        msg = new deviceReadAllConfig;
        break;
    case message::HFPD_DEV_READ_CONFIG:
        msg = new deviceReadConfig;
        break;
    case message::HFPD_DEV_SEND_MESSAGE:
        msg = new deviceSendMessage;
        break;
    case message::HFPD_DEV_STATE:
        msg = new deviceState;
        break;
    case message::HFPD_DEV_WRITE_CONFIG:
        msg = new deviceWriteConfig;
        break;
    case message::HFPD_PING:
        msg = new ping;
        break;
    case message::HFPD_PONG:
        msg = new pong;
        break;
    default:
        break;
    }

    // parse message
    if ( msg )
    {
        if (( msg->parse( &s[4], messageLen, len ) ) && ( messageLen == (*len) ))
            (*len) += 4;
        else
        {
            delete msg;
            msg = NULL;
        }
    }

    // unknown or cannot parse message, dump the data
    if ( !msg )
    {
        (*len) = (messageLen+4);
    }

    return msg;
}
