#include "CUEvolWrapperStatus.h"
#include "VersionManagerEvent.h"

#include "Debug.h"
#include "HobString.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CUEvolWrapperStatus::_dataType_ = 0;
std::string CUEvolWrapperStatus::_dataTypeName_("CUEvolWrapperStatus"); 
#endif

    CUEvolWrapperStatus::CUEvolWrapperStatus(UINT16 nodeType, UINT16 nodeStatus, Hobart::CEvolMsgReceiver *pHobThread)
: EvolMessage(VersionManagerDev, UsbWrapperUpgradeStatus, pHobThread), nodeType(nodeType), nodeStatus(nodeStatus)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif
}

    CUEvolWrapperStatus::CUEvolWrapperStatus(const char *buffer)
: EvolMessage(buffer)
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
    char *pChar = (char *) &nodeType;
    for (unsigned int i = 0; i < sizeof(nodeType); i++)
    {
        *pChar++ = buffer[j++];
    }

    pChar = (char *) &nodeStatus;
    for (unsigned int i = 0; i < sizeof(nodeStatus); i++)
    {
        *pChar++ = buffer[j++];
    }
}

CUEvolWrapperStatus::~CUEvolWrapperStatus()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif
}

UINT16 CUEvolWrapperStatus::getNodeType()const
{
    return nodeType;
}
void CUEvolWrapperStatus::setNodeType(UINT16 type)
{
    nodeType = type;
}

UINT16 CUEvolWrapperStatus::getNodeStatus()const
{
    return nodeStatus;
}
void CUEvolWrapperStatus::setNodeStatus(UINT16 status)
{
    nodeStatus = status;
}

const char *CUEvolWrapperStatus::serialize() const
{
    EvolMessage::serialize();

    // Now, for the derived class items
    HobString msgDebug("serialize():    nodeType = ");
    msgDebug.append(Integer::toString(nodeType));
    msgDebug.append("    nodeStatus = ").append(Integer::toString(nodeStatus));
    Debug::trace("CUEvolWrapperStatus", msgDebug);

    char *pChar = (char *) &nodeType;
    for (unsigned int i = 0; i < sizeof(nodeType); i++)
    {
        serializedData.append(1, pChar[i]);
    }

    pChar = (char *) &nodeStatus;
    for (unsigned int i = 0; i < sizeof(nodeStatus); i++)
    {
        serializedData.append(1, pChar[i]);
    }

    return serializedData.data();
}

void CUEvolWrapperStatus::show()
{
    if (Debug::isTracing("CUEvolWrapperStatus") == false)
        return;

    Debug::trace("CUEvolWrapperStatus", "CUEvolWrapperStatus: ");

    HobString msgDebug("    nodeType = ");
    msgDebug.append(Integer::toString(nodeType));
    Debug::trace("CUEvolWrapperStatus", msgDebug);

    msgDebug = "    nodeStatus = ";
    msgDebug.append(Integer::toString(nodeStatus));
    Debug::trace("CUEvolWrapperStatus", msgDebug);
}
