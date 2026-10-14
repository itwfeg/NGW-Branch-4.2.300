#include "UpgradeTransferStatus.h"
#include "VersionManagerEvent.h"

#include "Debug.h"
#include "HobString.h"
#include "Long.h"


#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int UpgradeTransferStatus::_dataType_ = 0;
std::string UpgradeTransferStatus::_dataTypeName_("UpgradeTransferStatus"); 
#endif


UpgradeTransferStatus::UpgradeTransferStatus(UINT32 bytesSent, UINT32 totalBytes, Hobart::CEvolMsgReceiver *pHobThread)
	: EvolMessage(VersionManagerDev, UpgradeXferStatus, pHobThread), bytesSent(bytesSent), totalBytes(totalBytes)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif
}

UpgradeTransferStatus::UpgradeTransferStatus(const char *buffer) : EvolMessage(buffer)
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
    char *pChar = (char *) &bytesSent;
    for (unsigned int i = 0; i < sizeof(bytesSent); i++)
    {
        *pChar++ = buffer[j++];
    }

    pChar = (char *) &totalBytes;
    for (unsigned int i = 0; i < sizeof(totalBytes); i++)
    {
        *pChar++ = buffer[j++];
    }
}

UpgradeTransferStatus::~UpgradeTransferStatus()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif
}


const char *UpgradeTransferStatus::serialize() const
{
    EvolMessage::serialize();

    char *pChar = (char *) &bytesSent;
    for (unsigned int i = 0; i < sizeof(bytesSent); i++)
    {
        serializedData.append(1, pChar[i]);
    }

    pChar = (char *) &totalBytes;
    for (unsigned int i = 0; i < sizeof(totalBytes); i++)
    {
        serializedData.append(1, pChar[i]);
    }

    return serializedData.data();
}

void UpgradeTransferStatus::show()
{
    if (Debug::isTracing("UpgradeTransferStatus"))
    {
        Debug::trace("UpgradeTransferStatus", "UpgradeTransferStatus: ");

        EvolMessage::show();

        Debug::trace("UpgradeTransferStatus", HobString(" bytesSent: ") + HobString(Long::toString(bytesSent)));
        Debug::trace("UpgradeTransferStatus", HobString("totalBytes: ") + HobString(Long::toString(totalBytes)));
    }
}

UINT32 UpgradeTransferStatus::getBytesSent()
{
    return bytesSent;
}

UINT32 UpgradeTransferStatus::getBytesSent()const
{
    return bytesSent;
}

void UpgradeTransferStatus::setBytesSent(UINT32 bytes)
{
    bytesSent = bytes;
}

UINT32 UpgradeTransferStatus::getTotalBytes()
{
    return totalBytes;
}

UINT32 UpgradeTransferStatus::getTotalBytes()const
{
    return totalBytes;
}

void UpgradeTransferStatus::setTotalBytes(UINT32 bytes)
{
    totalBytes = bytes;
}
