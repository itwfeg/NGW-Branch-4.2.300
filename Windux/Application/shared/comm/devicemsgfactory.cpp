/**
 * @file devicemsgfactory.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "devicemsgfactory.h"

#include "devicemsgconfiguration.h"
#include "devicemsgerror.h"
#include "devicemsggcd.h"
#include "devicemsggcdsession.h"
#include "devicemsggraphic.h"
#include "devicemsgsession.h"
#include "devicemsgsoftkey.h"
#include "devicemsgweigher.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
msgRequest *deviceMsgFactory::allocMsgRequest( const QByteArray& data, unsigned int& dataLen )
{
    msgRequest *m = NULL;

    deviceMsgType::type device;
    unsigned int event;

    // validate message
    if ( deviceMsg::valid( data, device, event ) )
    {
        // handle device
        switch ( device )
        {

        ///////////////////////////////////////////////////////////////////////////
        // weigher messages
        ///////////////////////////////////////////////////////////////////////////
        case deviceMsgType::Weigher:

            if ( weigherStatusDeviceMsg::typeOf( device, event ) )
                m = new weigherStatusDeviceMsg;

            break;

        ///////////////////////////////////////////////////////////////////////////
        // session messages
        ///////////////////////////////////////////////////////////////////////////
        case deviceMsgType::Session:

            if ( sessionRegisterDeviceMsg::typeOf( device, event ) )
                m = new sessionRegisterDeviceMsg;
            else if ( sessionTestMessageDeviceMsg::typeOf( device, event ) )
                m = new sessionTestMessageDeviceMsg;

            break;

        ///////////////////////////////////////////////////////////////////////////
        // softkey messages
        ///////////////////////////////////////////////////////////////////////////
        case deviceMsgType::Error:

            if ( errorErrSoftkeyDeviceMsg::typeOf( device, event ) )
                m = new errorErrSoftkeyDeviceMsg;
            else if ( errorErrRemoveAnyModalDialogDeviceMsg::typeOf( device, event ) )
                m = new errorErrRemoveAnyModalDialogDeviceMsg;

            break;


        ///////////////////////////////////////////////////////////////////////////
        // graphic messages
        ///////////////////////////////////////////////////////////////////////////
        case deviceMsgType::Graphic:

            if ( graphicReadAllSoftkeyGraphicNamesDeviceMsg::typeOf( device, event ) )
                m = new graphicReadAllSoftkeyGraphicNamesDeviceMsg;
            else if ( graphicReadAllVideoNamesDeviceMsg::typeOf( device, event ) )
                m = new graphicReadAllVideoNamesDeviceMsg;

            break;

        ///////////////////////////////////////////////////////////////////////////
        // configuration messages
        ///////////////////////////////////////////////////////////////////////////

            if ( configurationScaleReadDeviceMsg::typeOf( device, event ) )
                m = new configurationScaleReadDeviceMsg;
            else if ( configurationScaleWriteDeviceMsg::typeOf( device, event ) )
                m = new configurationScaleWriteDeviceMsg;
            else if ( configurationScaleSaveDeviceMsg::typeOf( device, event ) )
                m = new configurationScaleSaveDeviceMsg;

            else if ( configurationScaleOperationReadDeviceMsg::typeOf( device, event ) )
                m = new configurationScaleOperationReadDeviceMsg;
            else if ( configurationScaleOperationWriteDeviceMsg::typeOf( device, event ) )
                m = new configurationScaleOperationWriteDeviceMsg;
            else if ( configurationScaleOperationSaveDeviceMsg::typeOf( device, event ) )
                m = new configurationScaleOperationSaveDeviceMsg;

            else if ( configurationCurrencyReadDeviceMsg::typeOf( device, event ) )
                m = new configurationCurrencyReadDeviceMsg;
            else if ( configurationCurrencyWriteDeviceMsg::typeOf( device, event ) )
                m = new configurationCurrencyWriteDeviceMsg;
            else if ( configurationCurrencySaveDeviceMsg::typeOf( device, event ) )
                m = new configurationCurrencySaveDeviceMsg;

            break;

        ///////////////////////////////////////////////////////////////////////////
        // softkey messages
        ///////////////////////////////////////////////////////////////////////////
        case deviceMsgType::Softkey:

            if ( softkeyReadSoftkeyDeviceMsg::typeOf( device, event ) )
                m = new softkeyReadSoftkeyDeviceMsg;
            else if ( softkeyWriteSoftkeyDeviceMsg::typeOf( device, event ) )
                m = new softkeyWriteSoftkeyDeviceMsg;
            else if ( softkeyDeleteSoftkeyDeviceMsg::typeOf( device, event ) )
                m = new softkeyDeleteSoftkeyDeviceMsg;
            else if ( softkeySaveSoftkeyDeviceMsg::typeOf( device, event ) )
                m = new softkeySaveSoftkeyDeviceMsg;
            else if ( softkeyCreateNewSoftkeyDeviceMsg::typeOf( device, event ) )
                m = new softkeyCreateNewSoftkeyDeviceMsg;

            else if ( softkeyReadSoftkeyFeatureDeviceMsg::typeOf( device, event ) )
                m = new softkeyReadSoftkeyFeatureDeviceMsg;
            else if ( softkeyWriteSoftkeyFeatureDeviceMsg::typeOf( device, event ) )
                m = new softkeyWriteSoftkeyFeatureDeviceMsg;
            else if ( softkeySaveSoftkeyFeatureDeviceMsg::typeOf( device, event ) )
                m = new softkeySaveSoftkeyFeatureDeviceMsg;

            else if ( softkeyReadSoftkeyStyleDeviceMsg::typeOf( device, event ) )
                m = new softkeyReadSoftkeyStyleDeviceMsg;
            else if ( softkeyWriteSoftkeyStyleDeviceMsg::typeOf( device, event ) )
                m = new softkeyWriteSoftkeyStyleDeviceMsg;
            else if ( softkeySaveSoftkeyStyleDeviceMsg::typeOf( device, event ) )
                m = new softkeySaveSoftkeyStyleDeviceMsg;

            else if ( softkeyReadSoftkeyGroupDeviceMsg::typeOf( device, event ) )
                m = new softkeyReadSoftkeyGroupDeviceMsg;
            else if ( softkeyReadAllSoftkeyGroupsDeviceMsg::typeOf( device, event ) )
                m = new softkeyReadAllSoftkeyGroupsDeviceMsg;
            else if ( softkeyWriteSoftkeyGroupDeviceMsg::typeOf( device, event ) )
                m = new softkeyWriteSoftkeyGroupDeviceMsg;
            else if ( softkeyDeleteSoftkeyGroupDeviceMsg::typeOf( device, event ) )
                m = new softkeyDeleteSoftkeyGroupDeviceMsg;
            else if ( softkeySaveSoftkeyGroupDeviceMsg::typeOf( device, event ) )
                m = new softkeySaveSoftkeyGroupDeviceMsg;
            else if ( softkeyCreateNewSoftkeyGroupDeviceMsg::typeOf( device, event ) )
                m = new softkeyCreateNewSoftkeyGroupDeviceMsg;
            else if ( softkeyLoadSoftkeyGroupDeviceMsg::typeOf( device, event ) )
                m = new softkeyLoadSoftkeyGroupDeviceMsg;

            break;

        ///////////////////////////////////////////////////////////////////////////
        default:
            break;
        }

        // no object, so just return message length
        if ( !m )
            dataLen = deviceMsg::calculateLength( data );

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
void deviceMsgFactory::freeMsgRequest( msgRequest *m )
{
    if ( m )
        delete m;
}
