#include "KPCBootUpgraderFrameHeader.h"
#include "Integer.h"
#include "Debug.h"
#include "HobString.h"
#include "Event.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterGeneric::dataType = 0;
std::string PrinterGeneric::dataTypeName("KPCBootUpgraderFrameHeader");
#endif

KPCBootUpgraderFrameHeader::KPCBootUpgraderFrameHeader(const KPCBootUpgraderFrameHeader &newBFrame) : EvolMessage(newBFrame)
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
    numberOfFrames = newBFrame.getNumberOfFrames();
    finalFrameLength = newBFrame.getFinalFrameLength();
}

KPCBootUpgraderFrameHeader::KPCBootUpgraderFrameHeader(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

    EvolMessage::serialize();
    int j = getSerializedLength();

    char *pChar = (char *)&numberOfFrames;
    for (unsigned int i = 0; i < sizeof(numberOfFrames); i++)
    {
        *pChar++ = buffer[j++];
    }

    pChar = (char *)&finalFrameLength;
    for (unsigned int i = 0; i < sizeof(finalFrameLength); i++)
    {
        *pChar++ = buffer[j++];
    }
}

KPCBootUpgraderFrameHeader::KPCBootUpgraderFrameHeader(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(KPCBootUpdater, KpcBootUpgraderFrame, pHobThread)
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
    numberOfFrames = 0;
    finalFrameLength = 0;
}

KPCBootUpgraderFrameHeader::~KPCBootUpgraderFrameHeader()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif
}

const char *KPCBootUpgraderFrameHeader::serialize() const
{

    EvolMessage::serialize();

    // Now, for the derived class items
    char *pChar = (char *)&numberOfFrames;
    for (unsigned int i = 0; i < sizeof(numberOfFrames); i++)
    {
        serializedData.append(1, pChar[i]);
    }

    pChar = (char *)&finalFrameLength;
    for (unsigned int i = 0; i < sizeof(finalFrameLength); i++)
    {
        serializedData.append(1, pChar[i]);
    }

    return serializedData.data();
}

void KPCBootUpgraderFrameHeader::show()
{
    bool isTracing = Debug::isTracing("KPCBootUpgraderFrameHeader");

    if (isTracing == false)
    {
        return;
    }
    Debug::trace("KPCBootUpgraderFrameHeader", "KPCBootUpgraderFrameHeader: ");

    EvolMessage::show();

    Debug::trace("KPCBootUpgraderFrameHeader",
                 HobString("Number of Frames: ") + Integer::toString(numberOfFrames));
    Debug::trace("KPCBootUpgraderFrameHeader",
                 HobString("Final Frame Length: ") + Integer::toString(finalFrameLength));

    if (isTracing == false)
    {
        Debug::removeTraceCategory("KPCBootUpgraderFrameHeader");
    }
}

    uint8_t KPCBootUpgraderFrameHeader::getNumberOfFrames() const{
        return numberOfFrames;
    }
    void KPCBootUpgraderFrameHeader::setNumberOfFrames(uint8_t frames){
        numberOfFrames = frames;
    }

    uint8_t KPCBootUpgraderFrameHeader::getFinalFrameLength() const{
        return finalFrameLength;
    }
    void KPCBootUpgraderFrameHeader::setFinalFrameLength(uint8_t length){
        finalFrameLength = length;
    }