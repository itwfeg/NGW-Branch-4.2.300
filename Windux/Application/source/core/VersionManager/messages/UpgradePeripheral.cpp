#include "UpgradePeripheral.h"
//#include "VersionManagerEvent.h"

#include "Debug.h"
#include "HobString.h"
#include "Integer.h"


#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int UpgradePeripheral::_dataType_ = 0;
std::string UpgradePeripheral::_dataTypeName_("UpgradePeripheral"); 
#endif


UpgradePeripheral::UpgradePeripheral(UINT8 event, EvolDeviceType device, UINT16 subType, Hobart::CEvolMsgReceiver *pHobThread)
	: EvolMessage(VersionManagerDev, event, pHobThread), device(device), subType(subType)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif
}

UpgradePeripheral::UpgradePeripheral(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif
    // index into buffer
    EvolMessage::serialize();

    int j = getSerializedLength();

    // Now, for the derived class items
    char *pChar = (char *) &device;
    for (unsigned int i = 0; i < sizeof(device); i++)
    {
        *pChar++ = buffer[j++];
    }

    pChar = (char *) &subType;
    for (unsigned int i = 0; i < sizeof(subType); i++)
    {
        *pChar++ = buffer[j++];
    }
}

UpgradePeripheral::~UpgradePeripheral()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif
}


const char *UpgradePeripheral::serialize() const
{
    EvolMessage::serialize();

    char *pChar = (char *) &device;
    for (unsigned int i = 0; i < sizeof(device); i++)
    {
        serializedData.append(1, pChar[i]);
    }

    pChar = (char *) &subType;
    for (unsigned int i = 0; i < sizeof(subType); i++)
    {
        serializedData.append(1, pChar[i]);
    }

    return serializedData.data();
}

void UpgradePeripheral::show()
{
    if (Debug::isTracing("UpgradePeripheral"))
    {
        Debug::trace("UpgradePeripheral", "UpgradePeripheral: ");

        EvolMessage::show();

        Debug::trace("UpgradePeripheral", HobString(" device: ") + HobString(Integer::toString(device)));
        Debug::trace("UpgradePeripheral", HobString("subType: ") + HobString(Integer::toString(subType)));
    }
}

EvolDeviceType UpgradePeripheral::getDevice()
{
    return device;
}
EvolDeviceType UpgradePeripheral::getDevice()const
{
    return device;
}

void UpgradePeripheral::setDevice(EvolDeviceType dev)
{
    device = dev;
}

UINT16 UpgradePeripheral::getSubType()
{
    return subType;
}

UINT16 UpgradePeripheral::getSubType()const
{
    return subType;
}

void UpgradePeripheral::setSubType(UINT16 type)
{
    subType = type;
}
