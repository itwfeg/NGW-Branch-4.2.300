#include "KPCBootFrameHeader.h"
#include "Integer.h"
#include "Debug.h"
#include "HobString.h"
#include "KpcBootEvent.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterGeneric::dataType = 0;
std::string PrinterGeneric::dataTypeName("KPCBootFrameHeader");
#endif

KPCBootFrameHeader::KPCBootFrameHeader(const KPCBootFrameHeader &newBFrame) : EvolMessage(newBFrame)
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
    numberOfFrames = newBFrame.getNumberOfFrames();
    finalFrameLength = newBFrame.getFinalFrameLength();
}

KPCBootFrameHeader::KPCBootFrameHeader(const char *buffer) : EvolMessage(buffer)
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

KPCBootFrameHeader::KPCBootFrameHeader(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(KPCBootloader, KpcBootFrame, pHobThread)
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
    numberOfFrames = 0;
    finalFrameLength = 0;
}

KPCBootFrameHeader::~KPCBootFrameHeader()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif
}

const char *KPCBootFrameHeader::serialize() const
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

void KPCBootFrameHeader::show()
{
    bool isTracing = Debug::isTracing("KPCBootFrameHeader");

    if (isTracing == false)
    {
        return;
    }
    Debug::trace("KPCBootFrameHeader", "KPCBootFrameHeader: ");

    EvolMessage::show();

    Debug::trace("KPCBootFrameHeader",
                 HobString("Number of Frames: ") + Integer::toString(numberOfFrames));
    Debug::trace("KPCBootFrameHeader",
                 HobString("Final Frame Length: ") + Integer::toString(finalFrameLength));

    if (isTracing == false)
    {
        Debug::removeTraceCategory("KPCBootFrameHeader");
    }
}

    uint8_t KPCBootFrameHeader::getNumberOfFrames() const{
        return numberOfFrames;
    }
    void KPCBootFrameHeader::setNumberOfFrames(uint8_t frames){
        numberOfFrames = frames;
    }

    uint8_t KPCBootFrameHeader::getFinalFrameLength() const{
        return finalFrameLength;
    }
    void KPCBootFrameHeader::setFinalFrameLength(uint8_t length){
        finalFrameLength = length;
    }