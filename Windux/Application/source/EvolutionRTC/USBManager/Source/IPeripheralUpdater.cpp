#include "PAL_platformheader.h"


#include "UsbManager.h"
#include "IPeripheralUpdater.h"
//#include "CypressEzUsbUpdater.h"
//#include "CUPrinterCommManager.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int IPeripheralUpdater::dataType = 0;
std::string IPeripheralUpdater::dataTypeName("IPeripheralUpdater"); 
#endif

IPeripheralUpdater::IPeripheralUpdater(USBD_NODE_ID nodeId,
        UINT16 devType 
        ) : 
    usbNodeId(nodeId)
    ,usbDeviceType(devType)
{
    // create path to firmware binaries
    char *env_val = getenv(HOBART_BASE_DIR_ENV);
    if (NULL == env_val)
    {
        //		PAL_PRINT_LOG(PAL_ERR_LOG, "IPeripheralUpdater: Unable to retrieve value for Hobart environment var");
// (haberrc)        hob_base_dir = ".";
        hob_base_dir = HOBART_BASE_DIR_ENV;
    }
    else
    {
        hob_base_dir = env_val;
        hob_base_dir += HOBART_BIN_DIR;
    }

#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
}

IPeripheralUpdater::~IPeripheralUpdater()
{
    Debug::trace("IPeripheralUpdater", "IPeripheralUpdater destructor");
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif
}


USBD_NODE_ID IPeripheralUpdater::getUsbNodeId()
{
    return usbNodeId;
}

UINT16 IPeripheralUpdater::getUsbDeviceType()
{
    return usbDeviceType;
}

//	setUsbNodeId() violates the UsbManager contract, but we need it to swap printerTypes.
void IPeripheralUpdater::setUsbNodeId(USBD_NODE_ID usbNodeId)
{
    this->usbNodeId = usbNodeId;
}

#if 0 // intentionally disabled - violates UsbManager contract
void IPeripheralUpdater::setUsbDeviceType(UINT16 usbDeviceType)
{
    this->usbDeviceType = usbDeviceType;
}
#endif



