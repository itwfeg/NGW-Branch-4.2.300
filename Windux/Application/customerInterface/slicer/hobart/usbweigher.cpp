/**
 * @file usbweigher.h
 * Hobart Weigher Adapter class.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "usbweigher.h"

#include "usb/atmelsamba.h"
#include "usb/fpc.h"
#include "usb/prepack.h"
#include "usb/usbdevman.h"
#include "usb/hobartweigher.h"

#include <iostream>

#include <QTimer>
#include "msgbox.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
HobartUsbWeigher::HobartUsbWeigher( QObject *parent ) :
    _Mybase( parent ),
    _Myimpl(),
    weigher_( NULL )
{
    // generate state mapping
    stateMap_[hobartWeigher::zero] = zero;
    stateMap_[hobartWeigher::unstable] = unstable;
    stateMap_[hobartWeigher::stable] = stable;
    stateMap_[hobartWeigher::underWeight] = underWeight;
    stateMap_[hobartWeigher::overWeight] = overWeight;
    stateMap_[hobartWeigher::outOfMaint] = error;
    stateMap_[hobartWeigher::error] = error;

    // create usb manager instance
    usbMan_ = usbDevManager::instance();

    // lock device list
    usbMan_->lockEvents();
    connect( usbMan_, SIGNAL(deviceArrived(usbDev*)), SLOT(onDeviceArrived(usbDev*)) );
    connect( usbMan_, SIGNAL(deviceLeft(usbDev*)), SLOT(onDeviceLeft(usbDev*)) );


    // enumerate devices
    QList<usbDev*> devices( usbMan_->attachedDevices() );

    foreach ( usbDev *device, devices )
        onDeviceArrived( device );

    usbMan_->unlockEvents();

}

///////////////////////////////////////////////////////////////////////////////////////////////////
HobartUsbWeigher::~HobartUsbWeigher()
{
    delete usbMan_;
    usbMan_ = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HobartUsbWeigher::ZeroScale()
{
    if ( weigher_ )
    {
        weigher_->weigherZero();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HobartUsbWeigher::onDeviceArrived( usbDev* device )
{
    // atmel samba bootloader
    atmelSamba *atmelSambaDev;
    if (device->pid() == atmelSamba::ATMEL_PID && device->pid() == atmelSamba::ATMEL_PID)
    {
        atmelSambaDev = device;
        atmelSambaDev->boot();
        return;
    }


/*
    // fpc device
    fpc *fpcDev;

    if (( !weigher_ ) && ( (fpcDev = dynamic_cast<fpc*>( device )) ))
    {
        weigher_ = fpcDev;

        QTimer::singleShot( 0, this, SLOT(onWeigherInit()) );
        return;
    }
*/
    // prepack device
    prepack *prepackDev;

    if (( !weigher_ ) && ( (prepackDev = dynamic_cast<prepack*>( device )) ))
    {
        weigher_ = prepackDev;
        QTimer::singleShot( 0, this, SLOT(onWeigherInit()) );


        return;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HobartUsbWeigher::onDeviceLeft( usbDev* device )
{
    std::cout << "onDeviceLeft" << std::endl;
    if ( device == dynamic_cast<usbDev*>( weigher_ ) )
    {
        std::cout << "weigher DETACH" << std::endl;
        weigher_ = NULL;
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HobartUsbWeigher::onWeigherInit()
{
    QObject *obj;

    if ( !weigher_ )
        return;
    else if ( !(obj = dynamic_cast<QObject*>( weigher_ )) )
        return;

    std::cout << "weigher ATTACH" << std::endl;

    connect( obj, SIGNAL(weigherStateChanged(hobartWeigher::weigherState,float)), SLOT(onWeigherStateChanged(hobartWeigher::weigherState,float)) );
    connect( obj, SIGNAL(sig_WeigherKeyPressed()), SLOT(slt_OnWeigherKeyPressed()));
    connect(obj, SIGNAL(weigherConfigChanged(WeigherConfiguration&)), this, SLOT(onWeigherConfigChanged(WeigherConfiguration&)));

    connect(obj, SIGNAL(sig_WeigherStatus(char*, int)), this, SLOT(slt_WeigherStatus(char*, int)));

    connect(this, SIGNAL(sig_ReqConfig()), dynamic_cast<prepack*>( weigher_ )  , SLOT(slt_ReqConfig()));
    connect(this, SIGNAL(sig_WriteConfig(WeigherConfiguration&)), dynamic_cast<prepack*>( weigher_ ) , SLOT(slt_WriteConfig(WeigherConfiguration&)));

    /***************************************************/
    /*
     * WEIGHER MODES
     * Prepack = 0
     * Weight Tracking = 1
     * Dumb = 2
     * Manual = 3
     */
    /***************************************************/
    weigher_->weigherWakeup();
    weigher_->setWeigherMode(3); //3 = manual
    weigher_->weigherEnable();
    weigher_->weigherWakeup();
    weigher_->reqWeigherConfig();
    //weigher_->sendFactoryReset();

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HobartUsbWeigher::onWeigherStateChanged( hobartWeigher::weigherState s, float weight )
{
    const State state( stateMap_[s] );

    //std::cout << "weigher STATE " << state << " " << weight << std::endl;
    Q_EMIT sig_weightUpdated( state, weight );

}

void HobartUsbWeigher::onWeigherConfigChanged(WeigherConfiguration & config)
{
    qDebug() << "HobartUsbWeigher::onWeigherConfigChanged(" << config.getFlags() << ")";
    Q_EMIT sig_WeigherConfigChanged(config);
}

void HobartUsbWeigher::slt_OnWeigherKeyPressed(){
    std::cout << "slt_OnWeigherKeyPressed()" << std:: endl;
    Q_EMIT sig_OnWeigherKeyPressed();
}

void HobartUsbWeigher::slt_ReqConfig(){
    std::cout << "sendReqConfig " << std::endl;
    Q_EMIT sig_ReqConfig();
}

void HobartUsbWeigher::slt_WriteConfig(WeigherConfiguration &config)
{
    Q_EMIT sig_WriteConfig(config); //Not fully sure where this is coming from, possibly scale.cpp
}

void HobartUsbWeigher::slt_SetWeigherMode(WeighingMode mode)
{
    if(weigher_)
        weigher_->setWeigherMode(mode);
}

void HobartUsbWeigher::slt_WeigherReset()
{
    if(weigher_)
        weigher_->sendColdReset();
}

void HobartUsbWeigher::slt_WeigherStatus(char *buffer, int buffLen)
{
    Q_EMIT sig_WeigherStatus(buffer, buffLen);
}
